/*
 * Copyright (c) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bench_api.h"
#include "stdio.h"

/* Number of give / take cycles on semaphore */
#define NUM_TEST_SEM 1000

void bench_sem_signal_release_initialize(void);
void bench_sem_signal_release(void);

int main()
{
	bench_initialize_test(bench_sem_signal_release_initialize);
	return 0;
}

void bench_sem_signal_release_initialize() 
{
	bench_sem_create(0);
	bench_timing_init();

	bench_sem_signal_release();
}

void bench_sem_signal_release(void) 
{
	int i;
	uint32_t diff;
	bench_time_t timestamp_start;
	bench_time_t timestamp_end;

	/* Measure average semaphore signal time */
	bench_timing_start();

	timestamp_start = bench_timing_counter_get();

	for (i = 0; i < NUM_TEST_SEM; i++) {
		bench_sem_give(0);
	}

	timestamp_end = bench_timing_counter_get();

	bench_timing_end();

	diff = bench_timing_cycles_get(timestamp_start, timestamp_end);

	printf("Average semaphore signal time %i cycles \n", diff / NUM_TEST_SEM);
	
	/* Measure average semaphore test time */
	bench_timing_start();

	timestamp_start = bench_timing_counter_get();

	for (i = 0; i < NUM_TEST_SEM; i++) {
		bench_sem_take(0);
	}

	timestamp_end = bench_timing_counter_get();

	bench_timing_end();

	diff = bench_timing_cycles_get(timestamp_start, timestamp_end);

	printf("Average semaphore test time %i cycles \n", diff / NUM_TEST_SEM);
}