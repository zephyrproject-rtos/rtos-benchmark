// SPDX-License-Identifier: Apache-2.0

#include "bench_api.h"
#include "bench_utils.h"
#include <stdlib.h>

/* Ensure we context switch enough times that our measurement is precise */
#define NB_OF_YIELD 1000

static uint32_t helper_thread_iterations;

#define Y_PRIORITY 10

void bench_yielding_thread(void)
{
	while (helper_thread_iterations < NB_OF_YIELD) {
		bench_yield();
		helper_thread_iterations++;
	}
}

void bench_thread_switch_yield(void)
{
	bench_set_current_thread_prio(10);

	uint32_t iterations = 0U;
	int32_t delta;
	bench_time_t timestamp_start;
	bench_time_t timestamp_end;
	uint32_t ts_diff;
	int error_count;

	timing_start();

	/* launch helper thread of the same priority as the thread
	 * of routine
	 */
	bench_thread_create(0, "y", Y_PRIORITY, bench_yielding_thread);
	bench_thread_resume(0);

	/* get initial timestamp */
	timestamp_start = bench_timing_counter_get();

	/* loop until either helper or this routine reaches number of yields */
	while (iterations < NB_OF_YIELD &&
	       helper_thread_iterations < NB_OF_YIELD) {
		bench_yield();
		iterations++;
	}

	/* get the number of cycles it took to do the test */
	timestamp_end = bench_timing_counter_get();

	/* Ensure both helper and this routine were context switching back &
	 * forth.
	 * For execution to reach the line below, either this routine or helper
	 * routine reached NB_OF_YIELD. The other loop must be at most one
	 * iteration away from reaching NB_OF_YIELD if execute was switching
	 * back and forth.
	 */
	delta = iterations - helper_thread_iterations;
	if (abs(delta) > 1) {
		/* expecting even alternating context switch, seems one routine
		 * called yield without the other having chance to execute
		 */
		error_count++;
		printf(" Error, iteration:%u, helper iteration:%u",
			     iterations, helper_thread_iterations);
	} else {
		/* thread_yield is called (iterations + helper_thread_iterations)
		 * times in total.
		 */
		ts_diff = bench_timing_cycles_get(timestamp_start, timestamp_end);
		PRINT_STATS_AVG("Average thread context switch using yield",
			ts_diff, (iterations + helper_thread_iterations));
	}

	bench_timing_stop();
}

void bench_thread_switch_yield_init(void)
{
	bench_timing_init();

	bench_thread_switch_yield();
}

int main(void)
{
	bench_test_init(bench_thread_switch_yield_init);
	return 0;
}
