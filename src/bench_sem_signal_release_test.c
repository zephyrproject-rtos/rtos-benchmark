// SPDX-License-Identifier: Apache-2.0

/**
 * @file Measure time for semaphore give and take
 *
 * This file contains the test that measures semaphore give and
 * take time in one thread.
 */

#include "bench_api.h"
#include "bench_utils.h"


/* Number of give / take cycles on semaphore */
#define NUM_TEST_SEM 1000

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

	timestamp_start = bench_timing_counter_get();

	for (i = 0; i < NUM_TEST_SEM; i++) {
		bench_sem_give(0);
	}

	timestamp_end = bench_timing_counter_get();

	bench_timing_stop();

	diff = bench_timing_cycles_get(&timestamp_start, &timestamp_end);

	PRINT_STATS("Average semaphore signal time", diff / NUM_TEST_SEM);

	/* Measure average semaphore test time */
	bench_timing_start();

	timestamp_start = bench_timing_counter_get();

	for (i = 0; i < NUM_TEST_SEM; i++) {
		bench_sem_take(0);
	}

	timestamp_end = bench_timing_counter_get();

	bench_timing_stop();

	diff = bench_timing_cycles_get(&timestamp_start, &timestamp_end);

	PRINT_STATS("Average semaphore test time", diff / NUM_TEST_SEM);
}

/**
 * @brief Test setup function
 */
void bench_sem_signal_release_init(void *param)
{
	bench_timing_init();

	bench_sem_create(0, 0, NUM_TEST_SEM);

	bench_sem_signal_release();
}

int main(void)
{
	bench_test_init(bench_sem_signal_release_init);
	return 0;
}
