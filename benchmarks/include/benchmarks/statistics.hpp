// Copyright ⓒ 2018-2021 ThePhD.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

#pragma once

#ifndef ZTD_OUT_PTR_BENCHMARKS_BENCHMARK_COMPUTE_HPP
#define ZTD_OUT_PTR_BENCHMARKS_BENCHMARK_COMPUTE_HPP

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

#endif // ZTD_OUT_PTR_BENCHMARKS_BENCHMARK_COMPUTE_HPP
