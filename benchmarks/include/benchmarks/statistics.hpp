//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

#pragma once

#ifndef PHD_OUT_PTR_BENCHMARKS_BENCHMARK_COMPUTE_HPP
#define PHD_OUT_PTR_BENCHMARKS_BENCHMARK_COMPUTE_HPP

#include <cstddef>
#include <cstdint>

#include <vector>

double compute_mean(const std::vector<double>& v);

double compute_variance(const std::vector<double>& v);

double compute_stddev(const std::vector<double>& v);

double compute_index_of_dispersion(const std::vector<double>& v);

double compute_max(const std::vector<double>& v);

double compute_min(const std::vector<double>& v);

std::uint64_t rdtsc();

struct rdtsc_time {
	std::uint64_t start;

	rdtsc_time()
	: start(rdtsc()) {
	}

	std::uint64_t time() const {
		return rdtsc() - start;
	}

	double d_time() const {
		return static_cast<double>(this->time());
	}
};

#endif // PHD_OUT_PTR_BENCHMARKS_BENCHMARK_COMPUTE_HPP
