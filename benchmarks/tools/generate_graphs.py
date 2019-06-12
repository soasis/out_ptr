import matplotlib
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import matplotlib.ticker as mticker
import csv
import json
import math
import random
import bisect
import os
import argparse
import fnmatch
import sys


def len_sorter(x):
	return len(x)


def entry_name_sorter(b):
	return b["name"]


def is_noop_category(n):
	noop_names = ["noop", "no-op", "no op"]
	s = n.casefold()
	return s in noop_names


def aggregate_categories(all_benchmarks, data_point_names):
	benchmarks = {}
	# sort by mean
	# so the actual ordering of graphs
	# is lowest to highest
	lower_is_better = data_point_names[0][1]

	def mean_sorter(b):
		if (b.get("error") != None):
			return sys.float_info.max
		mean_group = b["statistics"]["mean"]
		data_point_name = data_point_names[0][0]
		return mean_group[data_point_name]

	# find no-op category and use it in all benchmarks
	noop_benches = None
	for b in all_benchmarks:
		category = b["category"]
		if is_noop_category(category):
			noop_benches = b
			break

	for b in all_benchmarks:
		category = b["category"]
		if is_noop_category(category):
			continue

		if category not in benchmarks:
			benchmarks[category] = {
			    "benchmarks": [],
			    "heuristics": {
			        "min": sys.float_info.max,
			        "max": sys.float_info.min
			    }
			}
			if (noop_benches):
				benchmarks[category]["benchmarks"].append(noop_benches)

		target_category = benchmarks[category]
		target_entries = target_category["benchmarks"]
		target_heuristics = target_category["heuristics"]

		target_entries.append(b)
		target_heuristics["min"] = min(b["heuristics"]["min"],
		                               target_heuristics["min"])
		target_heuristics["max"] = max(b["heuristics"]["max"],
		                               target_heuristics["max"])

	for category_name in benchmarks:
		category_benchmarks = benchmarks[category_name]
		# first, sort by name so we can assign colors to each
		# benchmark appropriately (and make those
		# color assignments stable)
		entries = category_benchmarks["benchmarks"]
		entries.sort(key=entry_name_sorter)
		for bi, entry in enumerate(entries):
			entry["name_index"] = bi
			ci = entry["color_index"]
			if (len(data_point_names) < 2):
				dp = data_point_names[0]
				ci[dp[0]] = bi
			else:
				for dpi, dp in enumerate(data_point_names):
					ci[dp[0]] = dpi

		# then, sort by mean
		entries.sort(key=mean_sorter, reverse=lower_is_better)

	return benchmarks


def parse_csv(c, data_point_names, name_removals, categories, scale,
              scale_categories, time_scales):
	all_benchmarks = []

	return aggregate_categories(all_benchmarks, data_point_names)


def parse_json(j, data_point_names, name_removals, categories, scale,
               scale_categories, time_scales):
	timescale_units = [x[0] for x in time_scales]

	all_benchmarks = []

	j_benchmarks_array = j["benchmarks"]
	for j_benchmark in j_benchmarks_array:
		name = j_benchmark['name']
		run_name = j_benchmark['run_name']
		benchmark = None
		potential_targets = [
		    b for b in all_benchmarks if b['run_name'] == run_name
		]
		potential_categories = None if categories == None else [
		    c for c in categories if c in run_name
		]

		category = ""
		benchmark_name = run_name
		point_scalar = 1
		if (len(potential_categories) > 1):
			potential_categories.sort(key=len_sorter, reverse=True)
		if len(potential_categories) > 0:
			category = potential_categories[0]
			if category in scale_categories:
				point_scalar = 1 / scale

		if (len(potential_targets) < 1):
			benchmark_name = run_name.replace(category, "").strip("_")
			for chunk in name_removals:
				benchmark_name = benchmark_name.replace(chunk, "")
			all_benchmarks.append({
			    "category": category,
			    "name": benchmark_name,
			    "run_name": run_name,
			    "data": {},
			    "statistics": {},
			    "heuristics": {
			        "max": sys.float_info.min,
			        "min": sys.float_info.max,
			    },
			    "name_index": {},
			    "color_index": {},
			    "error": None
			})
			benchmark = all_benchmarks[-1]
		else:
			benchmark = potential_targets[-1]
		data = benchmark["data"]
		statistics = benchmark["statistics"]
		heuristics = benchmark["heuristics"]
		# check for errors
		benchmark_error = j_benchmark.get('error_occurred')
		if benchmark_error != None and benchmark_error:
			benchmark["error"] = j_benchmark['error_message']
			continue
		# populate data
		for point_name_lower in data_point_names:
			point_name = point_name_lower[0]
			if point_name not in data:
				data[point_name] = []
		time_unit = j_benchmark['time_unit']
		unit_index = timescale_units.index(time_unit)
		time_scale = time_scales[unit_index]
		to_seconds_multiplier = time_scale[2]
		if name == run_name:
			# is a data point
			for point_name_lower in data_point_names:
				point_name = point_name_lower[0]
				point_list = data[point_name]
				point = j_benchmark[point_name]
				point_adjusted = point * to_seconds_multiplier * point_scalar
				point_list.append(point_adjusted)
				heuristics["min"] = min(heuristics["min"], point_adjusted)
				heuristics["max"] = max(heuristics["max"], point_adjusted)
		else:
			# is a statistic
			statistic_name = name.replace(run_name, "").strip("_")
			if statistic_name not in statistics:
				statistics[statistic_name] = {}
			statistic = statistics[statistic_name]
			for point_name_lower in data_point_names:
				point_name = point_name_lower[0]
				point = j_benchmark[point_name]
				point_adjusted = point * to_seconds_multiplier * point_scalar
				statistic[point_name] = point_adjusted

	return aggregate_categories(all_benchmarks, data_point_names)


def draw_graph(name, category, benchmarks_heuristics, data_point_names,
               time_scales):
	# initialize figures
	figures, axes = plt.subplots()

	# set name we're going to use
	figure_name = name if name != None and len(
	    name) > 0 else category.replace("_", "")

	# get the values of the time scale to perform bisecting
	time_scale_values_from_seconds = [x[2] for x in time_scales]
	benchmarks = benchmarks_heuristics["benchmarks"]
	heuristics = benchmarks_heuristics["heuristics"]
	benchmarks_max = heuristics["max"]
	benchmarks_min = heuristics["min"]
	absolute_range = benchmarks_max - benchmarks_min

	# some pattern constants, to help us be pretty
	# some color constants, to help us be pretty!
	# and differentiate graphs
	# yapf: disable
	data_point_aesthetics = [
	 ('#a6cee3', '/'),
	 ('#f255bb', 'O'),
	 ('#00c9ab', '\\'),
	 ('#b15928', 'o'),
	 ('#33a02c', '.'),
	 ('#fb9a99', '*'),
	 ('#e31a1c', '+'),
	 ('#fdbf6f', 'x'),
	 ('#ff7f00', '|'),
	 ('#cab2d6', None),
	 ('#6a3d9a', '-'),
	 ('#ffff99', 'xx'),
	 ('#f5f5f5', '..'),
	 ('#1f78b4', '||'),
	 ('#b2df8a', '**'),
	 ('#cc33cc', '--')
	]
	#yapf: enable

	# transpose data into forms we need
	benchmark_names = [b["name"] for b in benchmarks]
	bars = []
	scatters = []
	num_data_points = len(data_point_names)
	bar_padding = 0.15
	bar_height = 0.35
	bar_all_sizes = bar_height * num_data_points + bar_padding
	quarter_bar_height = bar_height * 0.25
	bar_y_positions = []

	# draw mean-based bars with error indicators
	# and draw scatter-plot points
	for bi, benchmark in enumerate(benchmarks):
		statistics = benchmark["statistics"]
		for di, data_point_name_lower in enumerate(data_point_names):
			data_point_name = data_point_name_lower[0]
			bar_y = (bi * bar_all_sizes) + (di * bar_height) + (
			    bar_padding * 0.5)
			bar_y_positions.append(bar_y)
			err = benchmark.get('error')

			color_index = benchmark["color_index"][data_point_name]
			aesthetics = data_point_aesthetics[color_index]
			color = aesthetics[0]
			colorhsv = matplotlib.colors.rgb_to_hsv(
			    matplotlib.colors.hex2color(color))
			colorhsv[2] *= 0.6
			edgecolor = matplotlib.colors.hsv_to_rgb(colorhsv)

			if err != None:
				bars.append(
				    axes.text(
				        absolute_range * 0.02,
				        bar_y + (quarter_bar_height * 2),
				        err,
				        color=color,
				        style='italic',
				        horizontalalignment='left',
				        verticalalignment='center',
				        fontsize='small'))
				continue

			mean = statistics["mean"][data_point_name]
			stddev = statistics["stddev"][data_point_name]
			hatch = aesthetics[1]
			bar = axes.barh(
			    bar_y,
			    mean,
			    height=bar_height,
			    xerr=stddev,
			    linewidth=0.2,
			    edgecolor=edgecolor,
			    color=color,
			    hatch=hatch,
			    align='edge',
			    error_kw={
			        "capsize": 5.0,
			        "mew": 1.2,
			        "ecolor": 'black',
			    },
			    alpha=0.82)
			bars.append(bar)
			# the scatter plot should be semi-transparent in color...
			xscatter = benchmark["data"][data_point_name]
			xscatter_len = len(xscatter)
			yscatter = [
			    bar_y + random.uniform(quarter_bar_height,
			                           bar_height - quarter_bar_height)
			    for _ in xscatter
			]
			scatter_alpha = 0.20 if xscatter_len < 11 else 0.10 if xscatter_len < 101 else 0.05 if xscatter_len < 1001 else 0.002
			scatter = axes.scatter(
			    xscatter,
			    yscatter,
			    color=color,
			    edgecolor='#000000',
			    linewidth=0.5,
			    alpha=scatter_alpha)
			scatters.append(scatter)

	xscaleindex = bisect.bisect_left(time_scale_values_from_seconds,
	                                 benchmarks_max)
	xscale = time_scales[xscaleindex - 1]

	def time_axis_formatting(value, pos):
		if value == 0:
			return '0'
		if value.is_integer():
			return '{0:.0f}'.format(value * xscale[3])
		return '{0:.1f}'.format(value * xscale[3])

	axes.set_xlim([0, benchmarks_max + (absolute_range * 0.25)])
	axes.xaxis.set_major_formatter(
	    mticker.FuncFormatter(time_axis_formatting))

	# have ticks drawn from base of bar graph
	# to text labels
	y_ticks = [((y + 0.5) * bar_all_sizes)
	           for y in range(0, int(len(bar_y_positions) / num_data_points))]
	y_limits = [
	    bar_y_positions[0] - bar_padding,
	    bar_y_positions[-1] + bar_height + bar_padding
	]

	# set the tick spacing
	axes.set_yticks(y_ticks)
	# label each group (each cluster along the y axes)
	# with the names of the benchmarks we ran
	axes.set_yticklabels(benchmark_names)
	# set the visual limits so we have good spacing
	axes.set_ylim(y_limits)

	# if we have 2 or more data points,
	# a legend will help us label it all
	if (num_data_points > 1):
		# a proper legend for each name in data_point_names
		legend_texts = [
		    (data_point_name[0] +
		     ('- lower=good' if data_point_name[1] else 'higher=good')
		     for data_point_name in data_point_names)
		]
		# retrieve the color/shape of the bar as a reference so we can construct
		bar_style_references = [bar[0] for bar in bars]
		# make legend
		axes.legend(bar_style_references, legend_texts)
		axes.set_xlabel('measured in ' + xscale[1])
	else:
		# no need to put a legend, it's basically fine as-is
		data_point_name = data_point_names[0]
		legend_text = (data_point_name[0], 'lower is better'
		               if data_point_name[1] else 'higher is better')
		axes.set_xlabel(legend_text[0] + ' measured in ' + xscale[1] +
		                ' - ' + legend_text[1])

	# set the benchmark name, typically derived from the file name
	axes.set_title(figure_name)
	# get a nice, clean layout
	figures.tight_layout()

	# make sure to adjust top and bottoms
	figures.subplots_adjust(bottom=0.2)

	return figures, axes


def main():
	parser = argparse.ArgumentParser(
	    description=
	    'Generate graphs from a Google-Benchmark compatible json/csv listing of data'
	)
	parser.add_argument(
	    '-i',
	    '--input',
	    nargs='?',
	    default='out_ptr_benchmarks.json',
	    type=argparse.FileType('r'))
	parser.add_argument('-f', '--input_format', nargs='?')
	parser.add_argument('-o', '--output', nargs='?')
	parser.add_argument('-d', '--output_dir', nargs='?')
	parser.add_argument(
	    '-p', '--data_point_names', nargs='+', default=['real_time'])
	parser.add_argument('-l', '--lower', nargs='+', default=['real_time'])
	parser.add_argument('-c', '--categories', nargs='+', default=[])
	parser.add_argument('-s', '--scale', nargs='?', type=int, default=1)
	parser.add_argument('-t', '--scale_categories', nargs='+', default=[])
	parser.add_argument('-r', '--remove_from_names', nargs='+', default=[''])
	parser.add_argument('-z', '--time_format', nargs='?', default='clock')

	args = parser.parse_args()

	args.input_format = args.input_format or ("csv" if fnmatch.fnmatch(
	    args.input.name, "*.csv") else "json")

	if not args.output:
		directoryname, filename = os.path.split(args.input.name)
		file = os.path.splitext(filename)[0]
		args.output = os.path.join(directoryname, file + ".png")

	if args.categories and len(args.categories) > 0 and not args.output_dir:
		directoryname, filename = os.path.split(args.output)
		args.output_dir = directoryname

	if len(args.data_point_names) < 1:
		print(
		    "You must specify 1 or more valid data point names",
		    file=sys.stderr)
		sys.exit(1)

	data_point_names = [(dpn, dpn in args.lower)
	                    for dpn in args.data_point_names]
	name_removals = [n for n in args.remove_from_names]

	random.seed(1782905257495843795)

	name = os.path.split(args.input.name)[1]
	name = os.path.splitext(name)[0]

	clock_time_scales = [
	    ("fs", "femtoseconds", 1e-15, 1e+15),
	    ("ps", "picoseconds", 1e-12, 1e+12),
	    ("ns", "nanoseconds", 1e-9, 1e+9),
	    ("µs", "microseconds", .00001, 1000000),
	    ("us", "microseconds", .00001, 1000000),
	    ("ms", "milliseconds", .001, 1000),
	    ("s", "seconds", 1, 1),
	    ("m", "minutes", 60, 1 / 60),
	    ("h", "hours", 60 * 60, (1 / 60) / 60),
	]

	clock_time_scales = [
	    ("fs", "femtoseconds", 1e-15, 1e+15),
	    ("ps", "picoseconds", 1e-12, 1e+12),
	    ("ns", "nanoseconds", 1e-9, 1e+9),
	    ("µs", "microseconds", .00001, 1000000),
	    ("us", "microseconds", .00001, 1000000),
	    ("ms", "milliseconds", .001, 1000),
	    ("s", "seconds", 1, 1),
	    ("m", "minutes", 60, 1 / 60),
	    ("h", "hours", 60 * 60, (1 / 60) / 60),
	]

	is_csv = args.input_format == "csv"
	is_json = args.input_format == "json"
	if (not is_csv and not is_json):
		print(
		    "You must specify either 'json' or 'csv' as the format.",
		    file=sys.stderr)
		sys.exit(1)

	benchmarks = None
	if is_csv:
		c = csv.reader(args.input)
		benchmarks = parse_csv(c, data_point_names, name_removals,
		                       args.categories, args.scale,
		                       args.scale_categories, clock_time_scales)
	elif is_json:
		j = json.load(args.input)
		benchmarks = parse_json(j, data_point_names, name_removals,
		                        args.categories, args.scale,
		                        args.scale_categories, clock_time_scales)
	else:
		return

	# we are okay to draw
	# draw for each category
	for benchmarks_key in benchmarks:
		b = benchmarks[benchmarks_key]
		category = benchmarks_key
		if category == None or len(category) < 1:
			category = name
		benchmark_name = category.replace("_measure",
		                                  "").replace("_", " ").strip()
		figures, axes = draw_graph(benchmark_name, category, b,
		                           data_point_names, clock_time_scales)
		# save drawn figures
		save_name = benchmark_name
		savetarget = os.path.join(args.output_dir, save_name + '.png')
		print("Saving graph: {} (to '{}')".format(save_name, savetarget))
		plt.savefig(savetarget, format='png')
		plt.close(figures)


if __name__ == "__main__":
	main()