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

#define MAIN_PRIORITY  10    /* Priority of main thread in the system */

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

/**
 * @brief Reset time statistics
 */
static void reset_time_stats(void)
{
	bench_stats_reset(&time_to_create);
	bench_stats_reset(&time_to_start);
	bench_stats_reset(&time_to_suspend);
	bench_stats_reset(&time_to_resume);
	bench_stats_reset(&time_to_terminate);
}

/**
 * @brief Report the collected statistics
 *
 * This routine reports the collected times. It is used for both reporting
 * times that involved a context and those that did not. If there were
 * not any times collected for a given measurement, its mean average is
 * recorded as 0 and nothing will be printed.
 *
 * @param description A string used for additional information about what is
 *        being measured: "(context switch)" or "(no context switch)"
 */
static void report_stats(const char *description)
{
	if (time_to_start.avg != 0) {
		printf("Start a thread %s: min %llu ns, max %llu ns, avg %llu ns\n",
		       description,
		       bench_timing_cycles_to_ns(time_to_start.min),
		       bench_timing_cycles_to_ns(time_to_start.max),
		       bench_timing_cycles_to_ns(time_to_start.avg));
	}

	printf("Suspend a thread %s: min %llu ns, max %llu ns, avg %llu ns\n",
	       description,
	       bench_timing_cycles_to_ns(time_to_suspend.min),
	       bench_timing_cycles_to_ns(time_to_suspend.max),
	       bench_timing_cycles_to_ns(time_to_suspend.avg));

	printf("Resume a thread %s: min %llu ns, max %llu ns, avg %llu ns\n",
	       description,
	       bench_timing_cycles_to_ns(time_to_resume.min),
	       bench_timing_cycles_to_ns(time_to_resume.max),
	       bench_timing_cycles_to_ns(time_to_resume.avg));

	if (time_to_terminate.avg != 0) {
		printf("Terminate a thread %s: min %llu ns, max %llu ns, avg %llu ns\n",
		       description,
		       bench_timing_cycles_to_ns(time_to_terminate.min),
		       bench_timing_cycles_to_ns(time_to_terminate.max),
		       bench_timing_cycles_to_ns(time_to_terminate.avg));
	}
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

	/* Start suspending the thread. This causes a context switch. */

	helper_start = helper_end;
	bench_thread_suspend(THREAD_HIGH);

	/*
	 * The main thread resumed this helper thread. Get the "ending"
	 * timestamp for resume operation. This will also be the same
	 * as the "starting" timestamp for terminating the helper thread.
	 */

	helper_end = bench_timing_counter_get();
	helper_start = helper_end;
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

	/* Create, but do not start the higher priority thread */

	bench_thread_create(THREAD_HIGH, "thread_suspend_resume",
				priority - 1, bench_set2_helper, NULL);

	/* Start the higher priority thread. This causes a context switch. */

	start = bench_timing_counter_get();
	bench_thread_start(THREAD_HIGH);

	/* Helper thread executed and then self-suspended. */

	end = bench_timing_counter_get();

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

	/* Helper thread executed and then terminated. */

	end = bench_timing_counter_get();

	/* Update times for both starting and resuming the thread */

	bench_stats_update(&time_to_resume,
			   bench_timing_cycles_get(&start, &helper_end),
			   iteration);
	bench_stats_update(&time_to_terminate,
			   bench_timing_cycles_get(&helper_start, &end),
			   iteration);
}

/**
 * @brief Entry point to helper thread to gathering set #1 data
 */
static void bench_set1_helper(void *args)
{
	ARG_UNUSED(args);

	/* This routine is intentionally empty. */
}

/**
 * @brief Test basic thread actions that do not result in a context switch
 *
 * Set #1 of the basic thread actions is responsible for ...
 * 1. Time to create and name a thread
 * 2. Time to start a thread of lower priority
 * 3. Time to suspend a thread of lower priority
 * 4. Time to resume a thread of lower priority
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
#if 0
#endif

	/*
	 * Lower and then restore the priority of the current thread to allow
	 * the otherwise lower priority thread to finish.
	 */

	bench_thread_set_priority(priority + 2);
	bench_thread_set_priority(priority);
}

/**
 * @brief Test setup function
 */
static void bench_basic_thread_ops(void *arg)
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

	bench_timing_start();

	for (i = 1; i <= ITERATIONS; i++) {
		gather_set1_stats(MAIN_PRIORITY, i);
	}

	report_stats("(no context switch)");

	/*
	 * Gather stats for basic thread operations for where there are
	 * thread context switches involved.
	 */

	reset_time_stats();

	for (i = 1; i <= ITERATIONS; i++) {
		gather_set2_stats(MAIN_PRIORITY, i);
	}

	bench_timing_stop();

	report_stats("(context switch)");
}

int main(void)
{
	bench_test_init(bench_basic_thread_ops);
	return 0;
}
