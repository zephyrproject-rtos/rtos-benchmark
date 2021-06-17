// SPDX-License-Identifier: Apache-2.0

/**
 * @file Measure time for mutex lock and unlock
 *
 * This file contains the test that measures mutex lock and unlock
 * time on one thread.
 */

#include "bench_api.h"
#include "bench_utils.h"

/* the number of mutex lock/unlock cycles */
#define N_TEST_MUTEX 1000

/**
 *
 * @brief Test for the multiple mutex lock/unlock time
 *
 * The routine performs multiple mutex locks and then multiple mutex
 * unlocks to measure the necessary time.
 */
void bench_mutex_lock_unlock(void)
{
	int i;
	uint32_t diff;
	bench_time_t timestamp_start;
	bench_time_t timestamp_end;

	bench_timing_start();

	timestamp_start = bench_timing_counter_get();

	for (i = 0; i < N_TEST_MUTEX; i++) {
		bench_mutex_lock(0);
	}

	timestamp_end = bench_timing_counter_get();

	diff = bench_timing_cycles_get(&timestamp_start, &timestamp_end);
	PRINT_STATS_AVG("Average time to lock a mutex", diff, N_TEST_MUTEX);

	timestamp_start = bench_timing_counter_get();

	for (i = 0; i < N_TEST_MUTEX; i++) {
		bench_mutex_unlock(0);
	}

	timestamp_end = bench_timing_counter_get();

	diff = bench_timing_cycles_get(&timestamp_start, &timestamp_end);

	PRINT_STATS_AVG("Average time to unlock a mutex", diff, N_TEST_MUTEX);

	bench_timing_stop();
}

/**
 * @brief Test setup function
 */
void bench_mutex_lock_unlock_init(void *arg)
{
	bench_mutex_create(0);
	bench_timing_init();

	bench_mutex_lock_unlock();
}

int main(void)
{
	bench_test_init(bench_mutex_lock_unlock_init);
	return 0;
}
