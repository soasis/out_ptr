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

#include <benchmarks/statistics.hpp>

#include <benchmark/benchmark.h>

#include <ztd/out_ptr/out_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <memory>
#include <cstdlib>


static void manual_shared_reset_out_ptr(benchmark::State& state) {
	int64_t x = 0;
	std::shared_ptr<ficapi::opaque> p(nullptr, ficapi::handle_no_alloc_deleter());
	for (auto _ : state) {
		(void)_;
		ficapi_opaque_handle temp_p = NULL;
		ficapi_handle_no_alloc_create(&temp_p);
		p.reset(temp_p, ficapi::handle_no_alloc_deleter());
		x += ficapi_handle_get_data(p.get());
	}
	int64_t expected = int64_t(state.iterations()) * ficapi_get_data();
	if (x != expected) {
		state.SkipWithError("Unexpected result");
		return;
	}
}
BENCHMARK(manual_shared_reset_out_ptr)
	->ComputeStatistics("max", &compute_max)
	->ComputeStatistics("min", &compute_min)
	->ComputeStatistics("dispersion", &compute_index_of_dispersion);

static void out_ptr_shared_reset_out_ptr(benchmark::State& state) {
	int64_t x = 0;
	std::shared_ptr<ficapi::opaque> p(nullptr, ficapi::handle_no_alloc_deleter());
	for (auto _ : state) {
		(void)_;
		ficapi_handle_no_alloc_create(ztd::out_ptr::op_detail::clever_out_ptr(p, ficapi::handle_no_alloc_deleter()));
		x += ficapi_handle_get_data(p.get());
	}
	int64_t expected = int64_t(state.iterations()) * ficapi_get_data();
	if (x != expected) {
		state.SkipWithError("Unexpected result");
		return;
	}
}
BENCHMARK(out_ptr_shared_reset_out_ptr)
	->ComputeStatistics("max", &compute_max)
	->ComputeStatistics("min", &compute_min)
	->ComputeStatistics("dispersion", &compute_index_of_dispersion);
