// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 *
 * @brief Measure thread yielding operations
 *
 * This modules measures two (2) different types of yield operations.
 * 1. Time to yield to a thread of equal priority (thread context switch)
 * 2. Time to yeild to a thread of lower priority (no thread context switch)
 *
 * This test assumes a uniprocessor system.
 */

#include "bench_api.h"
#include "bench_utils.h"
#include <stdlib.h>

#define THREAD_LOW      0
#define THREAD_HELPER   1

#define MAIN_PRIORITY   (BENCH_LAST_PRIORITY - 2)


static bench_time_t  helper_start;

static struct bench_stats time_to_yield;

/**
 * @brief Reset time statistics
 */
static void reset_time_stats(void)
{
	bench_stats_reset(&time_to_yield);
}

/**
 * @brief Entry point to helper thread to gathering set #2 data
 */
static void bench_set2_helper(void *args)
{
	ARG_UNUSED(args);

	/*
	 * This helper thread is expected to execute to the first yield
	 * before the timing begins. The 2nd yield is expected to be part
	 * of the benchmarking.
	 */

	bench_yield();

	helper_start = bench_timing_counter_get();

	bench_yield();

	bench_thread_exit();
}


/**
 * @brief Measure time to yield (thread context switch is performed)
 */
static void gather_set2_stats(int priority, uint32_t iteration)
{
	bench_time_t  end;

	/*
	 * Create and start the low priority helper thread. As it is of
	 * lower priority than the current thread, it is not expected
	 * to execute.
	 */

	bench_thread_create(THREAD_HELPER, "equal_priority_thread",
			    priority, bench_set2_helper, NULL);
	bench_thread_start(THREAD_HELPER);

	/*
	 * Yield to the helper thread so that it can execute to a known
	 * point so we do not include early thread startup code in the.
	 * benchmarking.
	 */

	bench_yield();

	bench_yield();
	end   = bench_timing_counter_get();

	bench_stats_update(&time_to_yield,
			   bench_timing_cycles_get(&helper_start, &end),
			   iteration);

	/*
	 * Abort lower priority thread, it's done its job.
	 */
	bench_thread_abort(THREAD_HELPER);
}

/**
 * @brief Entry point to helper thread to gathering set #1 data
 */
static void bench_set1_helper(void *args)
{
	ARG_UNUSED(args);

	/* This routine is not expected to execute. */

	bench_thread_exit();
}

/**
 * @brief Measure time to yield (but no thread context switch is done)
 */
static void gather_set1_stats(int priority, uint32_t iteration)
{
	bench_time_t  start;
	bench_time_t  end;

	/*
	 * Create and start the low priority helper thread. As it is of
	 * lower priority than the current thread, it is not expected
	 * to execute.
	 */

	bench_thread_create(THREAD_LOW, "low_priority_thread",
			    priority + 1, bench_set1_helper, NULL);
	bench_thread_start(THREAD_LOW);

	start = bench_timing_counter_get();
	bench_yield();
	end   = bench_timing_counter_get();

	bench_stats_update(&time_to_yield,
			   bench_timing_cycles_get(&start, &end),
			   iteration);

	/*
	 * Abort lower priority thread, it's done its job.
	 */
	bench_thread_abort(THREAD_LOW);
}

/**
 * @brief Test for the yield benchmarking
 */
void bench_thread_yield(void *arg)
{
	uint32_t  i;

	bench_timing_init();

	/* Lower main test thread priority */

	bench_thread_set_priority(MAIN_PRIORITY);

	/*
	 * Gather stats for yield operations for cases when there are not
	 * any thread context switches involved.
	 */

	reset_time_stats();
	bench_stats_report_title("Yield stats");

	bench_timing_start();

	for (i = 1; i <= ITERATIONS; i++) {
		gather_set1_stats(MAIN_PRIORITY, i);
	}

	bench_stats_report_line("Yield (no context switch)", &time_to_yield);

	reset_time_stats();

	for (i = 1; i <= ITERATIONS; i++) {
		gather_set2_stats(MAIN_PRIORITY, i);
	}

	bench_timing_stop();

	bench_stats_report_line("Yield (context switch)", &time_to_yield);
}

#ifdef RUN_THREAD_SWITCH_YIELD
int main(void)
{
	PRINTF("\n\r *** Starting! ***\n\n\r");

	bench_test_init(bench_thread_yield);

	PRINTF("\n\r *** Done! ***\n\r");

	return 0;
}
#endif
