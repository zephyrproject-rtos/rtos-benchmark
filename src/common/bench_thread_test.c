// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 *
 * @brief Measure common thread operations
 *
 * Broadly speaking, this module measures five (5) basic thread operations.
 * 1. Time to create and name a thread.
 * 2. Time to start a thread.
 *    A. Started thread is of lower priority (no context switch)
 *    B. Started thread is of higher priority (context switch)
 * 3. Time to suspend a thread.
 *    A. Suspended thread was not running when suspended (no context switch)
 *    B. Suspended thread was running when suspended (context switch)
 * 4. Time to resume a thread
 *    A. Resumed thread is of lower priority (no context switch)
 *    B. Resumed thread is of higher priority (context switch)
 * 5. Time to naturally terminate a running thread
 *
 * This benchmark test assumes a uniprocessor system.
 */

#include "bench_api.h"
#include "bench_utils.h"

#define THREAD_LOW      0
#define THREAD_HIGH     1
#define THREAD_SPAWN    2

#define MAIN_PRIORITY   (BENCH_LAST_PRIORITY - 2)    /* Priority of main thread in the system */

static bench_time_t helper_start;        /* helper thread start timestamp */
static bench_time_t helper_end;          /* helper thread end timestamp */

/*
 * Each set of times are comprsied of the following:
 * [0]: mean average value
 * [1]: minimum value
 * [2]: maximum value
 * [3]: total of all iterations
 */

static struct bench_stats time_to_create;    /* time to create a thread */
static struct bench_stats time_to_start;     /* time to start a thread */
static struct bench_stats time_to_suspend;   /* time to suspend a thread */
static struct bench_stats time_to_resume;    /* time to resume a thread */
static struct bench_stats time_to_terminate; /* time to terminate a thread */
static struct bench_stats time_to_spawn;     /* time to spawn a thread */

/**
 * @brief Reset time statistics
 */
static void reset_time_stats(void)
{
	bench_stats_reset(&time_to_spawn);
	bench_stats_reset(&time_to_create);
	bench_stats_reset(&time_to_start);
	bench_stats_reset(&time_to_suspend);
	bench_stats_reset(&time_to_resume);
	bench_stats_reset(&time_to_terminate);
}

/**
 * @brief Entry point to helper thread to gathering set #1 data
 *
 * This routine intentionally does nothing.
 */
static void bench_set2_helper(void *args)
{
	ARG_UNUSED(args);

	/* End-timestamp for starting the thread */

	helper_end = bench_timing_counter_get();

#if RTOS_HAS_SUSPEND_RESUME
	/* Start suspending the thread. This causes a context switch. */

	helper_start = helper_end;
	bench_thread_suspend(THREAD_HIGH);

	/*
	 * The main thread resumed this helper thread. Get the "ending"
	 * timestamp for resume operation. This will also be the same
	 * as the "starting" timestamp for terminating the helper thread.
	 */

#endif
	helper_end = bench_timing_counter_get();
	helper_start = helper_end;

	bench_thread_exit();
}

/**
 * @brief Entry point to helper thread to gathering thread spawn times
 */
static void bench_spawn_helper(void *args)
{
	ARG_UNUSED(args);

	helper_end = bench_timing_counter_get();
	bench_thread_exit();
}

/**
 * @brief Test basic thread actions that result in a context switch
 *
 * Set #2 of the basic thread actions is responsible for ...
 * 1. Time to start a thread of higher priority (context switch)
 * 2. Time to suspend a thread (context switch)
 * 3. Time to resume a thread of higher priority (context switch)
 * 4. Time to terminate a thread (context switch)
 */
static void gather_set2_stats(int priority, uint32_t iteration)
{
	bench_time_t  start;
	bench_time_t  end;

#if RTOS_HAS_THREAD_CREATE_START
	/* Create, but do not start the higher priority thread */

	bench_thread_create(THREAD_HIGH, "thread_suspend_resume",
			    priority - 1, bench_set2_helper, NULL);

	/* Start the higher priority thread. This causes a context switch. */

	start = bench_timing_counter_get();
	bench_thread_start(THREAD_HIGH);

	/* Helper thread executed and then self-suspended. */

	end = bench_timing_counter_get();
#else
	start = bench_timing_counter_get();
	bench_thread_spawn(THREAD_HIGH, "thread_suspend_resume",
			   priority - 1, bench_set2_helper, NULL);
	end = bench_timing_counter_get();
#endif

#if RTOS_HAS_SUSPEND_RESUME
	/* Update times for both starting and resuming the thread */

	bench_stats_update(&time_to_start,
			   bench_timing_cycles_get(&start, &helper_end),
			   iteration);
	bench_stats_update(&time_to_suspend,
			   bench_timing_cycles_get(&helper_start, &end),
			   iteration);

	/* Resume the higher priority thread. This causes a context switch. */

	start = bench_timing_counter_get();
	bench_thread_resume(THREAD_HIGH);
#endif

	/* Helper thread executed and then terminated. */

	end = bench_timing_counter_get();

	/* Update times for both starting and resuming the thread */

#if RTOS_HAS_SUSPEND_RESUME
	bench_stats_update(&time_to_resume,
			   bench_timing_cycles_get(&start, &helper_end),
			   iteration);
#endif

	bench_stats_update(&time_to_terminate,
			   bench_timing_cycles_get(&helper_start, &end),
			   iteration);

#if RTOS_HAS_THREAD_SPAWN

	/* Spawn a higher priority thread. This causes a context switch. */

	start = bench_timing_counter_get();
	bench_thread_spawn(THREAD_SPAWN, "thread_spawn",
			   priority - 1, bench_spawn_helper, NULL);
	bench_stats_update(&time_to_spawn,
			   bench_timing_cycles_get(&start, &helper_end),
			   iteration);
#endif
}

/**
 * @brief Entry point to helper thread to gathering set #1 data
 */
static void bench_set1_helper(void *args)
{
	ARG_UNUSED(args);

	bench_thread_exit();
}

/**
 * @brief Test basic thread actions that do not result in a context switch
 *
 * Set #1 of the basic thread actions is responsible for ...
 * 1. Time to create and name a thread
 * 2. Time to start a thread of lower priority
 * 3. Time to suspend a thread of lower priority
 * 4. Time to resume a thread of lower priority
 * 5. Time to spawn a thread of lower priority
 */

static void gather_set1_stats(int priority, uint32_t iteration)
{
	bench_time_t  start;
	bench_time_t  end;

	/* Create, but do not start the lower priority thread */

	start = bench_timing_counter_get();
	bench_thread_create(THREAD_LOW, "thread_suspend_resume",
				priority + 1, bench_set1_helper, NULL);
	end = bench_timing_counter_get();
	bench_stats_update(&time_to_create,
			   bench_timing_cycles_get(&start, &end),
			   iteration);

	/* Start the lower priority thread, but do not schedule it */

	start = bench_timing_counter_get();
	bench_thread_start(THREAD_LOW);
	end = bench_timing_counter_get();
	bench_stats_update(&time_to_start,
			   bench_timing_cycles_get(&start, &end),
			   iteration);

#if RTOS_HAS_SUSPEND_RESUME
	/* Suspend the low priority thread (no context switch) */

	start = bench_timing_counter_get();
	bench_thread_suspend(THREAD_LOW);
	end = bench_timing_counter_get();
	bench_stats_update(&time_to_suspend,
			   bench_timing_cycles_get(&start, &end),
			   iteration);

	/* Resume the low priority thread (no context switch) */

	start = bench_timing_counter_get();
	bench_thread_resume(THREAD_LOW);
	end = bench_timing_counter_get();
	bench_stats_update(&time_to_resume,
			   bench_timing_cycles_get(&start, &end),
			   iteration);
#endif

	/*
	 * Lower the priority to let the helper thread self-terminate
	 * and then restore the priority.
	 */

	bench_thread_set_priority(priority + 2);
	bench_thread_set_priority(priority);

#if RTOS_HAS_THREAD_SPAWN

	/* Spawn a low priority thread (no context switch) */

	start = bench_timing_counter_get();
	bench_thread_spawn(THREAD_SPAWN, "thread_spawn",
			   priority + 1, bench_spawn_helper, NULL);
	end = bench_timing_counter_get();
	bench_stats_update(&time_to_spawn,
			   bench_timing_cycles_get(&start, &end),
			   iteration);

	/* Abort lower priority thread. */

	bench_thread_abort(THREAD_SPAWN);
#endif
}

/**
 * @brief Test setup function
 */
void bench_basic_thread_ops(void *arg)
{
	uint32_t  i;

	bench_timing_init();

	/* Lower main test thread priority */

	bench_thread_set_priority(MAIN_PRIORITY);

	/*
	 * Gather stats for basic thread operations for where there are not
	 * any thread context switches involved.
	 */

	reset_time_stats();
	bench_stats_report_title("Thread stats");

	bench_timing_start();

	for (i = 1; i <= ITERATIONS; i++) {
		gather_set1_stats(MAIN_PRIORITY, i);
		bench_collect_resources();
	}

#if RTOS_HAS_THREAD_SPAWN
	bench_stats_report_line("Spawn (no context switch)",
				&time_to_spawn);
#else
	bench_stats_report_na("Spawn (no context switch)");
#endif

#if RTOS_HAS_THREAD_CREATE_START
	bench_stats_report_line("Create (no context switch)",
				&time_to_create);
#else
	bench_stats_report_na("Create (no context switch)");
#endif

#if RTOS_HAS_THREAD_CREATE_START
	bench_stats_report_line("Start  (no context switch)",
				&time_to_start);
#else
	bench_stats_report_na("Start (no context switch)");
#endif

#if RTOS_HAS_SUSPEND_RESUME
	bench_stats_report_line("Suspend (no context switch)",
				&time_to_suspend);
	bench_stats_report_line("Resume (no context switch)",
				&time_to_resume);
#else
	bench_stats_report_na("Suspend (no context switch)");
	bench_stats_report_na("Resume (no context switch)");
#endif

	/*
	 * Gather stats for basic thread operations for where there are
	 * thread context switches involved.
	 */

	reset_time_stats();

	for (i = 1; i <= ITERATIONS; i++) {
		gather_set2_stats(MAIN_PRIORITY, i);
		bench_collect_resources();
	}

	bench_timing_stop();

#if RTOS_HAS_THREAD_SPAWN
	bench_stats_report_line("Spawn (context switch)",
				&time_to_spawn);
#else
	bench_stats_report_na("Spawn (context switch)");
#endif

#if RTOS_HAS_THREAD_CREATE_START
	bench_stats_report_line("Start  (context switch)",
				&time_to_start);
#else
	bench_stats_report_na("Start  (context switch)");
#endif

#if RTOS_HAS_SUSPEND_RESUME
	bench_stats_report_line("Suspend (context switch)",
				&time_to_suspend);
	bench_stats_report_line("Resume (context switch)",
				&time_to_resume);
#else
	bench_stats_report_na("Suspend (context switch)");
	bench_stats_report_na("Resume (context switch)");
#endif
	bench_stats_report_line("Terminate (context switch)",
				&time_to_terminate);
}

#ifdef RUN_THREAD
int main(void)
{
	PRINTF("\n\r *** Starting! ***\n\n\r");

	bench_test_init(bench_basic_thread_ops);

	PRINTF("\n\r *** Done! ***\n\r");

	return 0;
}
#endif
