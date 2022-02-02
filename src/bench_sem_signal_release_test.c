// SPDX-License-Identifier: Apache-2.0

/**
 * @file Measure time for semaphore give and take
 *
 * This file contains the test that measures semaphore give and
 * take time in one thread.
 */

#include "bench_api.h"
#include "bench_utils.h"

uint32_t take_times[ITERATIONS];
uint32_t give_times[ITERATIONS];

/**
 * @brief Test main function
 *
 * Repeatedly give and then take semaphore and calculate average time.
 */
void bench_sem_signal_release()
{
	int i;
	unsigned int diff;
	uint64_t avg, min, max;
	bench_time_t timestamp_start;
	bench_time_t timestamp_end;

	/* Measure average semaphore signal time */
	bench_timing_start();

	for (i = 0; i < ITERATIONS; i++) {
		timestamp_start = bench_timing_counter_get();
		bench_sem_give(0);
		timestamp_end = bench_timing_counter_get();
		diff = bench_timing_cycles_get(&timestamp_start, &timestamp_end);
		give_times[i] = diff;
		thinker();
	}

	bench_timing_stop();

	bench_stats(give_times, ITERATIONS, &avg, &min, &max);
	BENCH_PRINTF("Semaphore give cycles (no context switch) [average, min, max],"
		     " %llu, %llu, %llu, Time, %llu %llu %llu\n", avg, min, max,
		     bench_timing_cycles_to_ns(avg),
		     bench_timing_cycles_to_ns(min),
		     bench_timing_cycles_to_ns(max));

	/* Measure average semaphore test time */
	bench_timing_start();


	for (i = 0; i < ITERATIONS; i++) {
		timestamp_start = bench_timing_counter_get();
		bench_sem_take(0);
		timestamp_end = bench_timing_counter_get();
		diff = bench_timing_cycles_get(&timestamp_start, &timestamp_end);
		take_times[i] = diff;
		thinker();
	}

	bench_timing_stop();

	bench_stats(take_times, ITERATIONS, &avg, &min, &max);
	BENCH_PRINTF("Semaphore take cycles (no context switch) [average, min, max],"
		     " %llu, %llu, %llu, Time, %llu %llu %llu\n", avg, min, max,
		     bench_timing_cycles_to_ns(avg),
		     bench_timing_cycles_to_ns(min),
		     bench_timing_cycles_to_ns(max));
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

int main(void)
{
	bench_test_init(bench_sem_signal_release_init);
	return 0;
}
