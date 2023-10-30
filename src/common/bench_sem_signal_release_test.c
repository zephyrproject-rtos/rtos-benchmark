// SPDX-License-Identifier: Apache-2.0

/**
 * @file Measure time for semaphore give and take
 *
 * This file contains the test that measures semaphore give and
 * take time in one thread.
 */

#include "bench_api.h"
#include "bench_utils.h"

static struct bench_stats take_times;
static struct bench_stats give_times;

/**
 * @brief Test main function
 *
 * Repeatedly give and then take semaphore and calculate average time.
 */
void bench_sem_signal_release()
{
	int i;
	bench_time_t diff;
	bench_time_t timestamp_start;
	bench_time_t timestamp_end;

	/* Measure average semaphore signal time */
	bench_timing_start();
	bench_stats_reset(&give_times);
	bench_stats_report_title("Semaphore stats");

	for (i = 1; i <= ITERATIONS; i++) {
		timestamp_start = bench_timing_counter_get();
		bench_sem_give(0);
		timestamp_end = bench_timing_counter_get();
		diff = bench_timing_cycles_get(&timestamp_start, &timestamp_end);
		bench_stats_update(&give_times, diff, i);
	}

	bench_timing_stop();

	bench_stats_report_line("Give (no context switch)", &give_times);

	/* Measure average semaphore test time */
	bench_timing_start();
	bench_stats_reset(&take_times);

	for (i = 1; i <= ITERATIONS; i++) {
		timestamp_start = bench_timing_counter_get();
		bench_sem_take(0);
		timestamp_end = bench_timing_counter_get();
		diff = bench_timing_cycles_get(&timestamp_start, &timestamp_end);
		bench_stats_update(&take_times, diff, i);
	}

	bench_timing_stop();

	bench_stats_report_line("Take (no context switch)", &take_times);
}

/**
 * @brief Test setup function
 */
void bench_sem_signal_release_init(void *param)
{
	bench_timing_init();

	bench_sem_create(0, 0, ITERATIONS);

	bench_sem_signal_release();
}

#ifdef RUN_SEM_SIGNAL_RELEASE
int main(void)
{
	PRINTF("\n\r *** Starting! ***\n\n\r");

	bench_test_init(bench_sem_signal_release_init);

	PRINTF("\n\r *** Done! ***\n\r");

	return 0;
}
#endif
