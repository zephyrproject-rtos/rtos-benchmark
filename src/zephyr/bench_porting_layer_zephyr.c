// SPDX-License-Identifier: Apache-2.0

#include "bench_api.h"
#include "bench_porting_layer_zephyr.h"
#include <zephyr/kernel.h>
#include <zephyr/timing/timing.h>
#include <zephyr/irq_offload.h>

/*
 * Constants.
 */
#define MAX_THREADS 10
#define STACK_SIZE 512
#define MAX_SEMAPHORES 2
#define MAX_MUTEXES 1

/*
 * Storage for data structures to be declared and used.
 *
 * For our implementation, the ID of a data structure is its position
 * in the array where it is stored.
 */
static K_THREAD_STACK_ARRAY_DEFINE(stacks, MAX_THREADS, STACK_SIZE);
static struct k_thread threads[MAX_THREADS];
static struct k_sem semaphores[MAX_SEMAPHORES];
static struct k_mutex mutexes[MAX_MUTEXES];

void bench_test_init(void (*test_init_function)(void *))
{
	void *param = NULL;
	(test_init_function)(param);
}

void bench_thread_set_priority(int priority)
{
	k_thread_priority_set(k_current_get(), priority);
}

int bench_thread_create(int thread_id, const char *thread_name, int priority,
	void (*entry_function)(void *), void *args)
{
	ARG_UNUSED(args);

	if (thread_id >= 0 && thread_id < MAX_THREADS) {
		k_thread_create(&threads[thread_id], stacks[thread_id],
				STACK_SIZE,	(k_thread_entry_t) entry_function,
				NULL, NULL, NULL,
				priority, 0, K_FOREVER);
		k_thread_name_set(&threads[thread_id], thread_name);
		return BENCH_SUCCESS;
	} else {
		return BENCH_ERROR;
	}
}

int bench_thread_spawn(int thread_id, const char *thread_name, int priority,
	void (*entry_function)(void *), void *args)
{
	ARG_UNUSED(args);

	if ((thread_id < 0) || (thread_id >= MAX_THREADS)) {
		return BENCH_ERROR;
	}

	k_thread_create(&threads[thread_id], stacks[thread_id], STACK_SIZE,
			(k_thread_entry_t) entry_function, NULL, NULL, NULL,
			priority, 0, K_NO_WAIT);

	return BENCH_SUCCESS;
}

void bench_thread_start(int thread_id)
{
	k_thread_start(&threads[thread_id]);
}

void bench_thread_resume(int thread_id)
{
	k_thread_resume(&threads[thread_id]);
}

void bench_thread_suspend(int thread_id)
{
	k_thread_suspend(&threads[thread_id]);
}

void bench_thread_abort(int thread_id)
{
	k_thread_abort(&threads[thread_id]);
}

void bench_yield(void)
{
	k_yield();
}

void bench_timing_init(void)
{
	timing_init();
}

void bench_sync_ticks(void)
{
	k_sleep(K_TICKS(1));
}

void bench_timing_start(void)
{
	timing_start();
}

void bench_timing_stop(void)
{
	timing_stop();
}

bench_time_t bench_timing_counter_get(void)
{
	return timing_counter_get();
}

bench_time_t bench_timing_cycles_get(bench_time_t *time_start, bench_time_t *time_end)
{
	return timing_cycles_get(time_start, time_end);
}

bench_time_t bench_timing_cycles_to_ns(bench_time_t cycles)
{
	return timing_cycles_to_ns(cycles);
}

int bench_sem_create(int sem_id, int initial_count, int maximum_count)
{
	k_sem_init(&semaphores[sem_id], initial_count, maximum_count);
	return BENCH_SUCCESS;
}

void bench_sem_give(int sem_id)
{
	k_sem_give(&semaphores[sem_id]);
}

void bench_sem_give_from_isr(int sem_id)
{
	k_sem_give(&semaphores[sem_id]);
}

int bench_sem_take(int sem_id)
{
	k_sem_take(&semaphores[sem_id], K_FOREVER);
	return BENCH_SUCCESS;
}

int bench_mutex_create(int mutex_id)
{
	k_mutex_init(&mutexes[mutex_id]);
	return BENCH_SUCCESS;
}

int bench_mutex_lock(int mutex_id)
{
	k_mutex_lock(&mutexes[mutex_id], K_FOREVER);
	return BENCH_SUCCESS;
}

int bench_mutex_unlock(int mutex_id)
{
	k_mutex_unlock(&mutexes[mutex_id]);
	return BENCH_SUCCESS;
}

void *bench_malloc(size_t size)
{
	return k_malloc(size);
}

void bench_free(void *ptr)
{
	k_free(ptr);
}

void bench_thread_exit(void)
{
	// NO-op on Zephyr
}
