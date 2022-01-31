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

#define MAIN_PRIORITY   10


static bench_time_t  helper_start;
static bench_time_t  calibration;

/*
 * Each set of times is comprised of the following :
 * [0]: mean average value
 * [1]: minimum value
 * [2]: maximum value
 * [3]: total of all iterations
 */

static bench_time_t time_to_yield[4];

/**
 * @brief Reset a set of times
 */
static void reset_times(bench_time_t *t)
{
	t[0] = 0;                    /* Mean average time */
	t[1] = (bench_time_t) -1;    /* Minimum time */
	t[2] = 0;                    /* Maximum time */
	t[3] = 0;                    /* Total time */
}

/**
 * @brief Reset time statistics
 */
static void reset_time_stats(void)
{
	reset_times(time_to_yield);
}

/**
 * @brief Update time statistics
 *
 * Update the mean, minimum, maximum and totals for the given time set.
 */
static void update_times(bench_time_t *t, bench_time_t value,
			 uint32_t iteration)
{
	value -= calibration;

	if (value < t[1]) {     /* Update minimum value if necessary */
		t[1] = value;
	}

	if (value > t[2]) {     /* Update maximum value if necessary */
		t[2] = value;
	}

	/* Update sum total of times and re-calculate mean average */

	t[3] += value;
	t[0] = t[3] / iteration;
}

/**
 * @brief Report the collected statistics
 */
static void report_stats(const char *description)
{
        printf("Yield %s: min %llu ns, max %llu ns, avg %llu ns\n",
               description,
               bench_timing_cycles_to_ns(time_to_yield[1]),
               bench_timing_cycles_to_ns(time_to_yield[2]),
               bench_timing_cycles_to_ns(time_to_yield[0]));
}

/**
 * @brief Calculate average time spent issuing bench_timing_counter_get()
 */
static void bench_calibrate(void)
{
	bench_time_t  start = bench_timing_counter_get();
	bench_time_t  end;
	uint32_t  i;

	for (i = 0; i < 1000000; i++) {
		bench_timing_counter_get();
	}
	end = bench_timing_counter_get();

	calibration = bench_timing_cycles_get(&start, &end) / 1000000;
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

	update_times(time_to_yield,
		     bench_timing_cycles_get(&helper_start, &end),
		     iteration);

	/*
	 * Lower the priority of the current thread to ensure that the
	 * helper thread can finish.
	 */

	bench_thread_set_priority(priority + 2);
	bench_thread_set_priority(priority);
}

/**
 * @brief Entry point to helper thread to gathering set #1 data
 */
static void bench_set1_helper(void *args)
{
        ARG_UNUSED(args);

        /* This routine intentionally does nothing */
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

	update_times(time_to_yield,
		     bench_timing_cycles_get(&start, &end),
		     iteration);

	/*
	 * Lower and then restore the priority of the current thread to allow
	 * the otherwise lower priority thread to finish.
	 */

	bench_thread_set_priority(priority + 2);
	bench_thread_set_priority(priority);
}

/**
 * @brief Test for the yield benchmarking
 */
static void bench_thread_yield(void *arg)
{
	uint32_t  i;

	bench_timing_init();

	bench_calibrate();

	/* Lower main test thread priority */

	bench_thread_set_priority(MAIN_PRIORITY);

	/*
	 * Gather stats for yield operations for cases when there are not
	 * any thread context switches involved.
	 */

	reset_time_stats();

	for (i = 1; i <= ITERATIONS; i++) {
		gather_set1_stats(MAIN_PRIORITY, i);
	}

	report_stats("(no context switch)");

	reset_time_stats();

	for (i = 1; i < ITERATIONS; i++) {
		gather_set2_stats(MAIN_PRIORITY, i);
	}

	report_stats("(context switch)");
}

int main(void)
{
	bench_test_init(bench_thread_yield);
	return 0;
}
