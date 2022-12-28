// SPDX-License-Identifier: Apache-2.0

/**
 * @file Measuring malloc and free times
 *
 * This test module measures the system usage of
 * malloc and free usage time.
 */

#include "bench_api.h"
#include "bench_utils.h"

#define TEST_SIZE 128

static struct bench_stats time_to_malloc;  /* time to malloc*/
static struct bench_stats time_to_free;    /* time to free */

/**
 * @brief Reset time statistics
 */
static void reset_time_stats(void)
{
	bench_stats_reset(&time_to_malloc);
	bench_stats_reset(&time_to_free);
}

/**
 * @brief Measure time to malloc and free.
 */
static void gather_set1_stats(uint32_t iteration)
{
	bench_time_t start;
	bench_time_t mid;
	bench_time_t end;
	void *p;

	start = bench_timing_counter_get();
	p = bench_malloc(TEST_SIZE);
	mid = bench_timing_counter_get();
	bench_free(p);
	end = bench_timing_counter_get();

	bench_stats_update(&time_to_malloc,
				bench_timing_cycles_get(&start, &mid),
				iteration);
	bench_stats_update(&time_to_free,
				bench_timing_cycles_get(&mid,&end),
				iteration);
}

/**
 * @brief Test setup function
 */
void bench_malloc_free(void *arg)
{
	uint32_t i;

	bench_timing_init();
	reset_time_stats();
	bench_stats_report_title("Allocation stats");

	bench_timing_start();

	for (i = 1; i <= ITERATIONS; i++) {
		gather_set1_stats(i);
	}

	bench_stats_report_line("Malloc", &time_to_malloc);
	bench_stats_report_line("Free", &time_to_free);

	bench_timing_stop();
}

#ifdef RUN_MALLOC_FREE
int main(void)
{
	PRINTF("\n\r *** Starting! ***\n\n\r");

	bench_test_init(bench_malloc_free);

	PRINTF("\n\r *** Done! ***\n\r");

	return 0;
}
#endif