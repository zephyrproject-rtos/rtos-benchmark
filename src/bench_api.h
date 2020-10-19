/* SPDX-License-Identifier: Apache-2.0 */

#ifndef  BENCH_API_H
#define  BENCH_API_H

#define BENCH_SUCCESS 0
#define BENCH_ERROR 1

#include "bench_porting_layer_zephyr.h"

void bench_test_init(void (*test_init_function)(void));

void bench_set_current_thread_prio(int prio);
int bench_thread_create(int thread_id, const char *thread_name, int priority,
	void (*entry_function)(void));
int bench_thread_resume(int thread_id);
int bench_thread_suspend(int thread_id);
int bench_thread_abort(int thread_id);

void bench_yield(void);

int bench_offload_setup(void);
int bench_offload_create_work(void (*worker_function)(void));
int bench_offload_submit_work(void);

void bench_timing_init(void);
void bench_sync_ticks(void);
void bench_timing_start(void);
void bench_timing_stop(void);
bench_time_t bench_timing_counter_get(void);
uint32_t bench_timing_cycles_get(bench_time_t time_start, bench_time_t time_end);
uint64_t bench_timing_cycles_to_ns(uint64_t cycles);

int bench_sem_create(int sem_id, int initial_count, int maximum_count);
int bench_sem_give(int id);
int bench_sem_take(int id);

int bench_mutex_create(int mutex_id);
int bench_mutex_lock(int mutex_id);
int bench_mutex_unlock(int mutex_id);

void bench_irq_offload(const void *irq_offload_routine, const void *parameter);

#endif
