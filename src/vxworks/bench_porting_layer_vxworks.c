/*
 * Copyright (c) 2023 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "bench_api.h"

static TASK_ID   g_bench_tIds[CONFIG_RTOS_BENCHMARK_MAXTHREADS];
static SEM_ID    g_bench_semaphores[CONFIG_RTOS_BENCHMARK_MAXSEMAPHORES];
static SEM_ID    g_bench_mutex[CONFIG_RTOS_BENCHMARK_MAXMUTEXES];

void bench_test_init(void (*test_init_function)(void *))
{
	test_init_function(NULL);
}

void bench_thread_set_priority(int priority)
{
	taskPrioritySet(taskIdSelf(), priority);
}

int bench_thread_create(int thread_id, const char *thread_name,
	int priority, void (*entry_function)(void *), void *args)
{
	g_bench_tIds[thread_id] = taskCreate((char *)thread_name, priority,
		VX_NO_STACK_FILL, TASK_STACK_SIZE, (FUNCPTR)entry_function,
		(_Vx_usr_arg_t)args, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L);

	if (g_bench_tIds[thread_id] == TASK_ID_NULL) {
		return ERROR;
	}

	return OK;
}

int bench_thread_spawn(int thread_id, const char *thread_name,
    int priority, void (*entry_function)(void *), void *args)
{
	g_bench_tIds[thread_id] = taskSpawn((char *)thread_name, priority,
		VX_NO_STACK_FILL, TASK_STACK_SIZE, (FUNCPTR)entry_function,
		(_Vx_usr_arg_t)args, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L);

	if (g_bench_tIds[thread_id] == TASK_ID_ERROR) {
		return ERROR;
	}

	return OK;
}

void bench_thread_start(int thread_id)
{
	(void)taskActivate(g_bench_tIds[thread_id]);
}

void bench_thread_resume(int thread_id)
{
	(void)taskResume(g_bench_tIds[thread_id]);
}

void bench_thread_suspend(int thread_id)
{
	(void)taskSuspend(g_bench_tIds[thread_id]);
}

void bench_thread_abort(int thread_id)
{
	(void)taskDelete(g_bench_tIds[thread_id]);
}

void bench_thread_exit(void)
{
	taskExit(0);
}

void bench_yield(void)
{
	taskDelay(0);
}

int bench_sem_create(int sem_id, int initial_count, int maximum_count)
{
	g_bench_semaphores[sem_id] = semCCreate(SEM_INTERRUPTIBLE |
		SEM_Q_PRIORITY, initial_count);

	if (g_bench_semaphores[sem_id] == SEM_ID_NULL) {
		return ERROR;
	}

	return OK;
}

void bench_sem_give(int sem_id)
{
	(void)semGive(g_bench_semaphores[sem_id]);
}

void bench_sem_give_from_isr(int sem_id)
{
}

int bench_sem_take(int sem_id)
{
	return semTake(g_bench_semaphores[sem_id], WAIT_FOREVER);
}

int bench_mutex_create(int mutex_id)
{
	g_bench_mutex[mutex_id] = semMCreate(SEM_INTERRUPTIBLE |
		SEM_Q_PRIORITY | SEM_INVERSION_SAFE);

	if (g_bench_mutex[mutex_id] == SEM_ID_NULL) {
		return ERROR;
	}

	return OK;
}

int bench_mutex_lock(int mutex_id)
{
	return semTake(g_bench_mutex[mutex_id], WAIT_FOREVER);
}

int bench_mutex_unlock(int mutex_id)
{
	return semGive(g_bench_mutex[mutex_id]);
}

void * bench_malloc(size_t size)
{
	return malloc(size);
}

void bench_free(void *ptr)
{
	free(ptr);
}
