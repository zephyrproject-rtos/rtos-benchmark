// SPDX-License-Identifier: Apache-2.0

#include "bench_api.h"
#include "bench_porting_layer_rtems.h"

#include <rtems.h>
#include <rtems/counter.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Constants.
 */

#define MAX_THREADS 10
#define MAX_SEMAPHORES 2
#define MAX_MUTEXES 1

#define BASE_PRIORITY 200

/*
 * Storage for data structures to be declared and used.
 *
 * For our implementation, the ID of a data structure is its position
 * in the array where it is stored.
 */

static rtems_id  sem_test_complete;
static rtems_id  semaphores[MAX_SEMAPHORES];
static rtems_id  mutexes[MAX_MUTEXES];
static rtems_id  threads[MAX_THREADS];
static rtems_task_entry  entries[MAX_THREADS];

void bench_test_init(void (*test_init_function)(void *))
{
	rtems_id  main_thread_id;
	rtems_task_priority  old_priority;
	rtems_status_code status;

	rtems_name sem_name;
	sem_name = rtems_build_name( 'd', 'o', 'n', 'e' );

	/*
	 * Lower the priority of the system's starting thread.
	 * Then create a new thread to run the test.
	 */

	status = rtems_task_set_priority(rtems_task_self(), 50, &old_priority);
	if (status != 0) {
		printf("**** Failed to set priority ****\n");
	}

	status = rtems_semaphore_create(sem_name, 0, RTEMS_FIFO,
					0, &sem_test_complete);
	if (status != 0) {
		printf(" **** Failed to initialize semaphore ****\n");
	}

	rtems_name main_thread_name;
	main_thread_name = rtems_build_name( 'm', 'a', 'i', 'n' );
	status = rtems_task_create(main_thread_name, BASE_PRIORITY,
				   RTEMS_CONFIGURED_MINIMUM_STACK_SIZE,
				   RTEMS_PREEMPT | RTEMS_NO_TIMESLICE,
				   RTEMS_LOCAL | RTEMS_NO_FLOATING_POINT,
				   &main_thread_id);
	if (status != 0) {
		printf(" **** Failed to start main thread ****\n");
	}

	status = rtems_task_start(main_thread_id,
				  (rtems_task_entry)test_init_function, 0);

	/* Ensure that we switch to test_init_function()'s thread */

	rtems_semaphore_obtain(sem_test_complete, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
}

void bench_test_complete()
{
	rtems_semaphore_release(sem_test_complete);
}

static rtems_task_priority map_prio(int prio)
{
	return (rtems_task_priority)(BASE_PRIORITY + prio);
}

void bench_thread_set_priority(int priority)
{
	rtems_task_priority old_priority;

	rtems_task_set_priority(RTEMS_SELF, map_prio(priority),
				&old_priority);
}

int bench_thread_create(int thread_id, const char *thread_name, int priority,
	void (*entry_function)(void *), void *args)
{
	static rtems_name  name = 0;
	rtems_status_code  status;

	entries[thread_id] = (rtems_task_entry)entry_function;
	name++;

	status = rtems_task_create(name, map_prio(priority),
				   RTEMS_CONFIGURED_MINIMUM_STACK_SIZE,
				   RTEMS_PREEMPT | RTEMS_NO_TIMESLICE,
				   RTEMS_LOCAL | RTEMS_NO_FLOATING_POINT,
				   &threads[thread_id]);

	return (status == 0) ? BENCH_SUCCESS : BENCH_ERROR;
}


int bench_thread_spawn(int thread_id, const char *thread_name, int priority,
		       void (*entry_function)(void *), void *args)
{
	/* Not supported by RTEMS */

	return BENCH_ERROR;
}

void bench_thread_start(int thread_id)
{
	rtems_task_start(threads[thread_id],
			 (rtems_task_entry)entries[thread_id], 0);
}

void bench_thread_resume(int thread_id)
{
	rtems_task_resume(threads[thread_id]);
}

void bench_thread_suspend(int thread_id)
{
	rtems_task_suspend(threads[thread_id]);
}

void bench_thread_abort(int thread_id)
{
	rtems_task_delete(threads[thread_id]);
}

void bench_yield(void)
{
	sched_yield();
}

void bench_timing_init(void)
{
	/* Nothing to do. */
}

void bench_sync_ticks(void)
{
	usleep(1000000);
}

void bench_timing_start(void)
{
	/* Nothing to do. */
}

void bench_timing_stop(void)
{
	/* Nothing to do. */
}

bench_time_t bench_timing_counter_get(void)
{
	return (bench_time_t)rtems_counter_read();
}

bench_time_t bench_timing_cycles_get(bench_time_t *time_start,
				     bench_time_t *time_end)
{
	return bench_timer_cycles_diff(*time_start, *time_end);
}

bench_time_t bench_timing_cycles_to_ns(bench_time_t cycles)
{
	return (1000000000ULL * cycles) / rtems_counter_frequency();
}

int bench_sem_create(int sem_id, int initial_count, int maximum_count)
{
	rtems_status_code  status;

	(void) maximum_count;

	rtems_name sem_name;
	sem_name = rtems_build_name( 's', 'e', 'm', '4' );

	status = rtems_semaphore_create(sem_name, (uint32_t)initial_count, 
					RTEMS_COUNTING_SEMAPHORE | RTEMS_LOCAL |
					RTEMS_PRIORITY,
					0,
					&semaphores[sem_id]);

	return (status != 0) ? BENCH_ERROR : BENCH_SUCCESS;
}

void bench_sem_give(int sem_id)
{
	rtems_semaphore_release(semaphores[sem_id]);
}

void bench_sem_give_from_isr(int sem_id)
{
	rtems_semaphore_release(semaphores[sem_id]);
}

int bench_sem_take(int sem_id)
{
	rtems_semaphore_obtain(semaphores[sem_id],
			       RTEMS_DEFAULT_OPTIONS,
			       RTEMS_NO_TIMEOUT);

	return BENCH_SUCCESS;
}

int bench_mutex_create(int mutex_id)
{
	rtems_status_code  status;

	rtems_name sem_name;
	sem_name = rtems_build_name( 'm', 'u', 't', 'x' );

	status = rtems_semaphore_create(sem_name, 1,
					RTEMS_BINARY_SEMAPHORE | RTEMS_LOCAL |
					RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
					0,
					&mutexes[mutex_id]);

	return (status != 0) ? BENCH_ERROR : BENCH_SUCCESS;
}

int bench_mutex_lock(int mutex_id)
{
	rtems_status_code  status;

	rtems_id  id;
	rtems_task_ident(RTEMS_WHO_AM_I, RTEMS_SEARCH_LOCAL_NODE, &id);

	status = rtems_semaphore_obtain(mutexes[mutex_id],
					RTEMS_WAIT,
					RTEMS_NO_TIMEOUT);

	return (status == 0) ? BENCH_SUCCESS : BENCH_ERROR;
}

int bench_mutex_unlock(int mutex_id)
{
	rtems_status_code  status;

	status = rtems_semaphore_release(mutexes[mutex_id]);

	return (status == 0) ? BENCH_SUCCESS : BENCH_ERROR;
}

void *bench_malloc(size_t size)
{
	return malloc(size);
}

void bench_free(void *ptr)
{
	free(ptr);
}

void bench_thread_exit(void)
{
	rtems_task_delete(RTEMS_SELF);
}
