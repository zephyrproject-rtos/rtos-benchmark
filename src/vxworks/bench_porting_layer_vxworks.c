/*
 * Copyright (c) 2023 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "bench_api.h"

static TASK_ID   g_bench_tIds[CONFIG_RTOS_BENCHMARK_MAXTHREADS];
static SEM_ID    g_bench_semaphores[CONFIG_RTOS_BENCHMARK_MAXSEMAPHORES];
static SEM_ID    g_bench_mutex[CONFIG_RTOS_BENCHMARK_MAXMUTEXES];
static MSG_Q_ID  g_bench_msgQ[CONFIG_RTOS_BENCHMARK_MAXMSGQS];

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
		return BENCH_ERROR;
	}

	return BENCH_SUCCESS;
}

int bench_thread_spawn(int thread_id, const char *thread_name,
    int priority, void (*entry_function)(void *), void *args)
{
	g_bench_tIds[thread_id] = taskSpawn((char *)thread_name, priority,
		VX_NO_STACK_FILL, TASK_STACK_SIZE, (FUNCPTR)entry_function,
		(_Vx_usr_arg_t)args, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L);

	if (g_bench_tIds[thread_id] == TASK_ID_ERROR) {
		return BENCH_ERROR;
	}

	return BENCH_SUCCESS;
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
		return BENCH_ERROR;
	}

	return BENCH_SUCCESS;
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
	STATUS ret;

	ret = semTake(g_bench_semaphores[sem_id], WAIT_FOREVER);

	if (ret == ERROR) {
		return BENCH_ERROR;
	}

	return BENCH_SUCCESS;
}

int bench_mutex_create(int mutex_id)
{
	g_bench_mutex[mutex_id] = semMCreate(SEM_INTERRUPTIBLE |
		SEM_Q_PRIORITY | SEM_INVERSION_SAFE);

	if (g_bench_mutex[mutex_id] == SEM_ID_NULL) {
		return BENCH_ERROR;
	}

	return BENCH_SUCCESS;
}

int bench_mutex_lock(int mutex_id)
{
	STATUS ret;

	ret = semTake(g_bench_mutex[mutex_id], WAIT_FOREVER);

	if (ret == ERROR) {
		return BENCH_ERROR;
	}

	return BENCH_SUCCESS;
}

int bench_mutex_unlock(int mutex_id)
{
	STATUS ret;

	ret = semGive(g_bench_mutex[mutex_id]);

	if (ret == ERROR) {
		return BENCH_ERROR;
	}

	return BENCH_SUCCESS;
}

void * bench_malloc(size_t size)
{
	return malloc(size);
}

void bench_free(void *ptr)
{
	free(ptr);
}

int bench_message_queue_create(int mq_id, const char *mq_name,
	size_t msg_max_num, size_t msg_max_len)
{
	g_bench_msgQ[mq_id] = msgQCreate(msg_max_num, msg_max_len,
		MSG_Q_FIFO);

	if (g_bench_msgQ[mq_id] == MSG_Q_ID_NULL) {
		return BENCH_ERROR;
	}

	return BENCH_SUCCESS;
}

int bench_message_queue_send(int mq_id, char *msg_ptr, size_t msg_len)
{
	STATUS ret;

	ret = msgQSend(g_bench_msgQ[mq_id], msg_ptr, msg_len, WAIT_FOREVER,
		MSG_PRI_NORMAL);

	if (ret == ERROR) {
		return BENCH_ERROR;
	}

	return BENCH_SUCCESS;
}

int bench_message_queue_receive(int mq_id, char *msg_ptr, size_t msg_len)
{
	ssize_t ret;

	ret = msgQReceive(g_bench_msgQ[mq_id], msg_ptr, msg_len, WAIT_FOREVER);

	if (ret == ERROR) {
		return BENCH_ERROR;
	}

	return BENCH_SUCCESS;
}

int bench_message_queue_delete(int mq_id, const char *mq_name)
{
	STATUS ret;

	ret = msgQDelete(g_bench_msgQ[mq_id]);

	if (ret == ERROR) {
		return BENCH_ERROR;
	}

	return BENCH_SUCCESS;
}

