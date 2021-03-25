#include <benchmarks/statistics.hpp>

#include <benchmark/benchmark.h>

#include <ztd/out_ptr/out_ptr.hpp>

#include <ficapi/ficapi.hpp>

#include <memory>
#include <cstdlib>

std::shared_ptr<ficapi::opaque> rvo_shared_allocate() {
	ficapi_opaque_handle temp_p = NULL;
	ficapi_handle_no_alloc_create(&temp_p);
	return std::shared_ptr<ficapi::opaque>(temp_p, ficapi::handle_no_alloc_deleter());
}

std::shared_ptr<ficapi::opaque> shared_allocate() {
	std::shared_ptr<ficapi::opaque> p(nullptr, ficapi::handle_no_alloc_deleter());
	ficapi_opaque_handle temp_p = NULL;

	ficapi_handle_no_alloc_create(&temp_p);
	p.reset(temp_p, ficapi::handle_no_alloc_deleter());

	return p;
}

std::shared_ptr<ficapi::opaque> out_ptr_shared_allocate() {
	std::shared_ptr<ficapi::opaque> p(nullptr);
	ficapi_handle_no_alloc_create(ztd::out_ptr::out_ptr(p, ficapi::handle_no_alloc_deleter()));
	return p;
}

static void manual_inline_shared_local_out_ptr(benchmark::State& state) {
	int64_t x = 0;
	for (auto _ : state) {
		(void)_;
		std::shared_ptr<ficapi::opaque> p(nullptr, ficapi::handle_no_alloc_deleter());
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
BENCHMARK(manual_inline_shared_local_out_ptr)
	->ComputeStatistics("max", &compute_max)
	->ComputeStatistics("min", &compute_min)
	->ComputeStatistics("dispersion", &compute_index_of_dispersion);

static void manual_inline_no_reset_shared_local_out_ptr(benchmark::State& state) {
	int64_t x = 0;
	for (auto _ : state) {
		(void)_;
		ficapi_opaque_handle temp_p = NULL;
		ficapi_handle_no_alloc_create(&temp_p);
		std::shared_ptr<ficapi::opaque> p(temp_p, ficapi::handle_no_alloc_deleter());

		x += ficapi_handle_get_data(p.get());
	}
	int64_t expected = int64_t(state.iterations()) * ficapi_get_data();
	if (x != expected) {
		state.SkipWithError("Unexpected result");
		return;
	}
}
BENCHMARK(manual_inline_no_reset_shared_local_out_ptr)
	->ComputeStatistics("max", &compute_max)
	->ComputeStatistics("min", &compute_min)
	->ComputeStatistics("dispersion", &compute_index_of_dispersion);


static void manual_rvo_shared_local_out_ptr(benchmark::State& state) {
	int64_t x = 0;
	for (auto _ : state) {
		(void)_;
		std::shared_ptr<ficapi::opaque> p = rvo_shared_allocate();
		x += ficapi_handle_get_data(p.get());
	}
	int64_t expected = int64_t(state.iterations()) * ficapi_get_data();
	if (x != expected) {
		state.SkipWithError("Unexpected result");
		return;
	}
}
BENCHMARK(manual_rvo_shared_local_out_ptr)
	->ComputeStatistics("max", &compute_max)
	->ComputeStatistics("min", &compute_min)
	->ComputeStatistics("dispersion", &compute_index_of_dispersion);


static void manual_return_shared_local_out_ptr(benchmark::State& state) {
	int64_t x = 0;
	for (auto _ : state) {
		(void)_;
		std::shared_ptr<ficapi::opaque> p = shared_allocate();
		x += ficapi_handle_get_data(p.get());
	}
	int64_t expected = int64_t(state.iterations()) * ficapi_get_data();
	if (x != expected) {
		state.SkipWithError("Unexpected result");
		return;
	}
}
BENCHMARK(manual_return_shared_local_out_ptr)
	->ComputeStatistics("max", &compute_max)
	->ComputeStatistics("min", &compute_min)
	->ComputeStatistics("dispersion", &compute_index_of_dispersion);

static void return_out_ptr_shared_local_out_ptr(benchmark::State& state) {
	int64_t x = 0;
	for (auto _ : state) {
		(void)_;
		std::shared_ptr<ficapi::opaque> p = out_ptr_shared_allocate();
		x += ficapi_handle_get_data(p.get());
	}
	int64_t expected = int64_t(state.iterations()) * ficapi_get_data();
	if (x != expected) {
		state.SkipWithError("Unexpected result");
		return;
	}
}
BENCHMARK(return_out_ptr_shared_local_out_ptr)
	->ComputeStatistics("max", &compute_max)
	->ComputeStatistics("min", &compute_min)
	->ComputeStatistics("dispersion", &compute_index_of_dispersion);

static void inline_out_ptr_shared_local_out_ptr(benchmark::State& state) {
	int64_t x = 0;
	for (auto _ : state) {
		(void)_;
		std::shared_ptr<ficapi::opaque> p(nullptr, ficapi::handle_no_alloc_deleter());
		ficapi_handle_no_alloc_create(ztd::out_ptr::op_detail::clever_out_ptr(p, ficapi::handle_no_alloc_deleter()));
		x += ficapi_handle_get_data(p.get());
	}
	int64_t expected = int64_t(state.iterations()) * ficapi_get_data();
	if (x != expected) {
		state.SkipWithError("Unexpected result");
		return;
	}
}
BENCHMARK(inline_out_ptr_shared_local_out_ptr)
	->ComputeStatistics("max", &compute_max)
	->ComputeStatistics("min", &compute_min)
	->ComputeStatistics("dispersion", &compute_index_of_dispersion);
