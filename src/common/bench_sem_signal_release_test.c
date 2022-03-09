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
	unsigned int diff;
	bench_time_t timestamp_start;
	bench_time_t timestamp_end;

	/* Measure average semaphore signal time */
	bench_timing_start();
	bench_stats_reset(&give_times);

	for (i = 1; i <= ITERATIONS; i++) {
		timestamp_start = bench_timing_counter_get();
		bench_sem_give(0);
		timestamp_end = bench_timing_counter_get();
		diff = bench_timing_cycles_get(&timestamp_start, &timestamp_end);
		bench_stats_update(&give_times, diff, i);
		thinker();
	}

	bench_timing_stop();

	PRINTF("Semaphore give time (no context switch) [avg min max], "
	       "%llu, %llu, %llu\n",
	       bench_timing_cycles_to_ns(give_times.avg),
	       bench_timing_cycles_to_ns(give_times.min),
	       bench_timing_cycles_to_ns(give_times.max));

	/* Measure average semaphore test time */
	bench_timing_start();
	bench_stats_reset(&take_times);

	for (i = 1; i <= ITERATIONS; i++) {
		timestamp_start = bench_timing_counter_get();
		bench_sem_take(0);
		timestamp_end = bench_timing_counter_get();
		diff = bench_timing_cycles_get(&timestamp_start, &timestamp_end);
		bench_stats_update(&take_times, diff, i);
		thinker();
	}

	bench_timing_stop();

	PRINTF("Semaphore take time (no context switch) [avg min max], "
	       "%llu, %llu, %llu\n",
	       bench_timing_cycles_to_ns(take_times.avg),
	       bench_timing_cycles_to_ns(take_times.min),
	       bench_timing_cycles_to_ns(take_times.max));
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
	bench_test_init(bench_sem_signal_release_init);
	return 0;
}
#endif
