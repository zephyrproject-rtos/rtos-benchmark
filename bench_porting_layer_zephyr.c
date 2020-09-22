/*
 * Copyright (c) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bench_api.h"
#include "bench_porting_layer_zephyr.h"
#include <zephyr.h>
#include <timing/timing.h>

#define MAX_THREADS 10
#define STACK_SIZE 512
#define MAX_SEMAPHORES 1

static K_THREAD_STACK_ARRAY_DEFINE(stacks, MAX_THREADS, STACK_SIZE);
static struct k_thread threads[MAX_THREADS];
static struct k_sem semaphores[MAX_SEMAPHORES];

void bench_initialize_test(void (*test_initialization_function) (void)) 
{
	(test_initialization_function)();
}

int bench_thread_create(int id, int priority, void (*entry_function) (void))
{
	if (id >= 0 && id < MAX_THREADS) {
		k_thread_create(&threads[id], stacks[id], STACK_SIZE,
		(k_thread_entry_t) entry_function, NULL, NULL, NULL,
		K_PRIO_PREEMPT(priority), 0, K_FOREVER);
		return BENCH_SUCCESS;
	} else {
		return BENCH_ERROR;
	}
}

int bench_thread_resume(int id) 
{
	struct k_thread *thread;

	thread = &threads[id];

	if (thread->base.thread_state & _THREAD_PRESTART) {
		k_thread_start(thread);
	} else {
		k_thread_resume(thread);
	}

	return BENCH_SUCCESS;
}

void bench_timing_init(void) 
{
	timing_init();
}

void bench_timing_start(void)
{
	timing_start();
}

void bench_timing_end(void) 
{
	timing_stop();
}

bench_time_t bench_timing_counter_get(void)
{
	return timing_counter_get();
}

uint32_t bench_timing_cycles_get(bench_time_t time_start, bench_time_t time_end) 
{
	return timing_cycles_get(&time_start, &time_end);
}

int bench_sem_create(int sem_id)
{
	k_sem_init(&semaphores[sem_id], 1, 1);
	return BENCH_SUCCESS;
}

int bench_sem_give(int sem_id) 
{
	k_sem_give(&semaphores[sem_id]);
	return BENCH_SUCCESS;
}

int bench_sem_take(int sem_id) 
{
	int ret;
	ret = k_sem_take(&semaphores[sem_id], K_NO_WAIT);

	if (ret == 0) {
		return BENCH_SUCCESS;
	} else {
		return BENCH_ERROR;
  	}
}
