// SPDX-License-Identifier: Apache-2.0

#include "bench_api.h"
#include "bench_utils.h"

static bench_time_t timestamp_start_create_c;
static bench_time_t timestamp_end_create_c;
static bench_time_t timestamp_start_start_c;
static bench_time_t timestamp_end_start_c;
static bench_time_t timestamp_start_suspend_c;
static bench_time_t timestamp_end_suspend_c;
static bench_time_t timestamp_start_resume_c;
static bench_time_t timestamp_end_resume_c;

static bench_time_t timestamp_start_abort_1;
static bench_time_t timestamp_end_abort_1;

void bench_thread_suspend_resume(void)
{
	timestamp_start_suspend_c = timing_counter_get();
	bench_thread_suspend(1);

	timestamp_start_resume_c = timing_counter_get();
}

void bench_suspend_resume(void)
{
	bench_set_current_thread_prio(10);

	uint32_t diff;

	timing_start();

	timestamp_start_create_c = bench_timing_counter_get();

	bench_thread_create(0, "thread_suspend_resume", 6, bench_thread_suspend_resume);

	timestamp_end_create_c = bench_timing_counter_get();

	timestamp_start_start_c = bench_timing_counter_get();
	bench_thread_resume(0); // Starts

	timestamp_end_suspend_c = bench_timing_counter_get();
	bench_thread_resume(0);

	timestamp_end_resume_c = bench_timing_counter_get();

	diff = bench_timing_cycles_get(timestamp_start_create_c,
				 timestamp_end_create_c);
	PRINT_STATS("Time to create a thread (without start)", diff);

	diff = bench_timing_cycles_get(timestamp_start_start_c,
				 timestamp_start_suspend_c);
	PRINT_STATS("Time to start a thread", diff);

	diff = bench_timing_cycles_get(timestamp_start_suspend_c,
				 timestamp_end_suspend_c);
	PRINT_STATS("Time to suspend a thread", diff);

	diff = bench_timing_cycles_get(timestamp_start_resume_c,
				 timestamp_end_resume_c);
	PRINT_STATS("Time to resume a thread", diff);

	timestamp_start_abort_1 = bench_timing_counter_get();
	bench_thread_abort(0);
	timestamp_end_abort_1 = bench_timing_counter_get();

	diff = bench_timing_cycles_get(timestamp_start_abort_1,
				 timestamp_end_abort_1);
	PRINT_STATS("Time to abort a thread (not running)", diff);

	bench_timing_stop();
}

void bench_interrupt_latency_init(void)
{
	bench_timing_init();

	bench_suspend_resume();
}

int main(void)
{
	bench_test_init(bench_interrupt_latency_init);
	return 0;
}
