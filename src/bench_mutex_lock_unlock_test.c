// SPDX-License-Identifier: Apache-2.0

/**
 * @file Measure time for mutex lock and unlock
 *
 * This file contains the test that measures the time to both lock and unlock
 * a mutex under various conditions.
 * 1. Time to lock and unlock a mutex
 * 2. Time to recursively lock and unlock a mutex
 * 3. Time to unlock a mutex and unpend a lower priority thread
 * 4. Time to unlock a mutex and unpend a higher priority thread
 * 5. Time to pend on a mutex without priority inheritance
 * 6. Time to pend on a mutex with priority inheritance
 */

#include "bench_api.h"
#include "bench_utils.h"

#define MUTEX_ID      0

#define SEM_ID        1

#define THREAD_LOW  0       /* Low priority thread ID */
#define THREAD_HIGH 1       /* High priority thread ID */

#define MAIN_PRIORITY  10

#define TIME_TO_LOCK                0
#define TIME_TO_UNLOCK              1
#define TIME_TO_RECURSIVELY_LOCK    2
#define TIME_TO_RECURSIVELY_UNLOCK  3
#define TIME_TO_UNPEND              4
#define TIME_TO_UNPEND_PRI_INH      5
#define TIME_TO_PEND                6
#define TIME_TO_PEND_PRI_INH        7

#define NUM_TIMES  8

#define AVERAGE  0
#define MINIMUM  1
#define MAXIMUM  2
#define TOTAL    3

static bench_time_t  times[NUM_TIMES][4];

static bench_time_t  calibration;
static bench_time_t  helper_start;
static bench_time_t  helper_end;

static const char *report_strings[NUM_TIMES] = {
    "1. Lock an unowned mutex: min %llu ns, max %llu ns, avg %llu ns\n",
    "2. Unlock a mutex (no waiters): min %llu ns, max %llu ns, avg %llu ns\n",
    "3. Recursively lock a mutex: min %llu ns, max %llu ns, avg %llu ns\n",
    "4. Recursively unlock a mutex: min %llu ns, max %llu ns, avg %llu ns\n",
    "5. Unlock a mutex and unpend thread (no ctx switch): min %llu ns, max %llu ns, avg %llu ns\n",
    "6. Unlock a mutex and unpend thread (with ctx switch): min %llu ns, max %llu ns, avg %llu ns\n",
    "7. Pend on a mutex (no priority inheritance): min %llu ns, max %llu ns, avg %llu ns\n",
    "8. Pend on a mutex (priority inheritance): min %llu ns, max %llu ns, avg %llu ns\n",
};

/**
 * @brief Reset a set of times
 */
static void reset_times(bench_time_t *t)
{
	t[AVERAGE] = 0;                  /* Mean average time */
	t[MINIMUM] = (bench_time_t) -1;  /* Minimum time */
	t[MAXIMUM] = 0;                  /* Maximum time */
	t[TOTAL] = 0;                    /* Total time */
}

/**
 * @brief Reset time statistics
 */
static void reset_time_stats()
{
	for (unsigned i = 0; i < NUM_TIMES; i++) {
		reset_times(times[i]);
	}
}

/**
 * @brief Update time statistics
 */
static void update_times(bench_time_t *t, bench_time_t value,
			 uint32_t iteration)
{
	value -= calibration;

	if (value < t[MINIMUM]) {      /* Update minimum value if necessary */
		t[MINIMUM] = value;
	}

	if (value > t[MAXIMUM]) {      /* Update maximum value if necessary */
		t[MAXIMUM] = value;
	}

	/* Update sum total of times and re-calculate mean average */

	t[TOTAL] += value;
	t[AVERAGE] = t[TOTAL] / iteration;
}

/**
 * @brief Report the collected statistics
 */
static void report_stats(void)
{
	int i;

	for (i = 0; i < NUM_TIMES; i++) {
		printf(report_strings[i],
		       bench_timing_cycles_to_ns(times[i][MINIMUM]),
		       bench_timing_cycles_to_ns(times[i][MAXIMUM]),
		       bench_timing_cycles_to_ns(times[i][AVERAGE]));
	}
}

/**
 * @brief Gather stats for locking/unlocking a mutex
 *
 * Gathers stats for locking an unowned mutex and for unlocking a mutex
 * with no waiters.
 */
static void gather_lock_unlock_stats(uint32_t iteration)
{
	bench_time_t  start;
	bench_time_t  mid;
	bench_time_t  end;

	start = bench_timing_counter_get();
	bench_mutex_lock(MUTEX_ID);
	mid   = bench_timing_counter_get();
	bench_mutex_unlock(MUTEX_ID);
	end   = bench_timing_counter_get();

	update_times(times[TIME_TO_LOCK],
		     bench_timing_cycles_get(&start, &mid),
		     iteration);

	update_times(times[TIME_TO_UNLOCK],
		     bench_timing_cycles_get(&mid, &end),
		     iteration);
}

/**
 * @brief Gather stats for recursively locking a mutex
 */
static void gather_recursive_lock_stats(uint32_t iteration)
{
	bench_time_t  start;
	bench_time_t  end;
	
	start = bench_timing_counter_get();
	bench_mutex_lock(MUTEX_ID);
	end = bench_timing_counter_get();

	update_times(times[TIME_TO_RECURSIVELY_LOCK],
		     bench_timing_cycles_get(&start, &end),
		     iteration);
}

/**
 * @brief Gather stats for recursively unlocking a mutex
 *
 * The mutex was previously recursively locked.
 */
static void gather_recursive_unlock_stats(uint32_t iteration)
{
	bench_time_t  start;
	bench_time_t  end;

	start = bench_timing_counter_get();
	bench_mutex_unlock(MUTEX_ID);
	end = bench_timing_counter_get();
	update_times(times[TIME_TO_RECURSIVELY_UNLOCK],
		     bench_timing_cycles_get(&start, &end),
		     iteration);
}

/**
 * @brief Entry point to helper thread to gather_unpend_stats()
 *
 * This helper thread is used by two different unlock+unpend tests. The
 * global variable <helper_end> is only relevant to the case that involves
 * priority inheritance.
 */
static void bench_unpend_helper(void *args)
{
	ARG_UNUSED(args);

	/* MUTEX_ID is expected to be owned by the main thread. */

	bench_mutex_lock(MUTEX_ID);

	helper_end = bench_timing_counter_get();

	bench_mutex_unlock(MUTEX_ID);
}

/**
 * @brief Gather stats for an unlock that unpends a blocked low priority thread
 */
static void gather_unpend_stats(int priority, uint32_t iteration)
{
	bench_time_t  start;
	bench_time_t  end;

	bench_mutex_lock(MUTEX_ID);

	bench_thread_create(THREAD_LOW, "thread_low",
			    priority, bench_unpend_helper, NULL);
	
	bench_thread_start(THREAD_LOW);

	bench_yield();       /* Yield to helper */

	/*
	 * Helper thread is expected to be pending on MUTEX_ID.
	 * Unlocking MUTEX_ID will give it to the helper thread, but
	 * there will not be any thread context switch.
	 */

	start = bench_timing_counter_get();
	bench_mutex_unlock(MUTEX_ID);
	end = bench_timing_counter_get();

	update_times(times[TIME_TO_UNPEND],
		     bench_timing_cycles_get(&start, &end),
		     iteration);

	/*
	 * Lower the priority of the current thread to ensure the
	 * helper thread executes to completion.
	 */

	bench_thread_set_priority(priority + 2);

	/* Restore current thread priority */

	bench_thread_set_priority(priority);
}

/**
 * @brief Unlock a mutex that unpends a blocked higher priority thread
 */
static void gather_unpend_inheritance_stats(int priority, uint32_t iteration)
{
	bench_time_t  start;

	bench_mutex_lock(MUTEX_ID);

	bench_thread_create(THREAD_HIGH, "thread_high",
			    priority - 1, bench_unpend_helper, NULL);
	
	bench_thread_start(THREAD_HIGH);

	/*
	 * Helper thread is expected to be pending on MUTEX_ID.
	 * Unlocking MUTEX_ID will give it to the helper thread, which
	 * is expected to run to completion.
	 */

	start = bench_timing_counter_get();
	bench_mutex_unlock(MUTEX_ID);

	update_times(times[TIME_TO_UNPEND_PRI_INH],
		     bench_timing_cycles_get(&start, &helper_end),
		     iteration);
}

/**
 * @brief Low priority helper thread
 */
static void bench_pend_low(void *args)
{
	ARG_UNUSED(args);

	/* Step 2 */

	bench_sem_give(SEM_ID);

	/* Step 5 */

	helper_end = bench_timing_counter_get();

	bench_sem_give(SEM_ID);    /* Unblock the main thread */

	/* Step 8 - clean up and finish */
}

/**
 * @brief High priority helper thread
 */
static void bench_pend_high(void *args)
{
	/* Step 4 */

	helper_start = bench_timing_counter_get();

	bench_mutex_lock(MUTEX_ID);

	/* Step 7 - clean up and finish */

	bench_mutex_unlock(MUTEX_ID);
}

/**
 * @brief Determine time to pend on a mutex (no priority inheritance)
 *
 * To measure the time to pend on a mutex that does not involve priority
 * inheritance requires three threads, two of which (THREAD_HIGH and
 * THREAD_LOW) are of lower priority than the main thread.
 *
 * 1. Main thread locks the mutex
 * 2. THREAD_HIGH pends on the mutex
 * 3. THREAD_LOW gets the final timestamp
 *
 * Execution order is ...
 *
 * 1. Main thread (locks mutex)
 * 2. Low priority helper (execute to known point)
 * 3. Main thread
 * 4. High priority helper (blocks on locking mutex)
 * 5. Low priority helper (unblocks main thread)
 * 6. Main thread (unblocks high priority helper, lowers own priority)
 * 7. High priority helper (cleans up and finishes)
 * 8. Low priority helper (finishes)
 * 9. Main thread (restores priority and finishes)
 */
static void gather_pend_stats(int priority, uint32_t iteration)
{
	/* Step 1 */

	bench_mutex_lock(MUTEX_ID);

	bench_thread_create(THREAD_LOW, "thread_low",
			    priority + 2, bench_pend_low, NULL);
	bench_thread_start(THREAD_LOW);

	bench_sem_take(SEM_ID);    /* Switch to low priority helper */

	/* Step 3 */

	bench_thread_create(THREAD_HIGH, "thread_high",
			    priority + 1, bench_pend_high, NULL);
	bench_thread_start(THREAD_HIGH);

	bench_sem_take(SEM_ID);    /* Block so high priority helper runs */

	/* Step 6. */

	update_times(times[TIME_TO_PEND],
		     bench_timing_cycles_get(&helper_start, &helper_end),
		     iteration);

	bench_mutex_unlock(MUTEX_ID);

	bench_thread_set_priority(priority + 3);

	/* Step 9 */

	bench_thread_set_priority(priority);
}

/**
 * @brief Determine time to pend on a mutex (with priority inheritance)
 *
 * To measure the time to pend on a mutex that involves priority inheritance
 * requires two threads--a high priority helper and the main thread.
 *
 * Execution order is ...
 *
 * 1. Main thread (locks mutex)
 * 4. High priority helper (blocks on locking mutex)
 * 5. Main thread unlocks mutex (unblocking high priority helper)
 * 7. High priority helper (cleans up and finishes)
 * 8. Main thread (cleans up and finishes)
 *
 * It is recognized that the enumeration of steps above may appear off.
 * This is done to remain consistent with the numbering and comments used
 * by gather_pend_stats() and its helper routines.
 */
static void gather_pend_inheritance_stats(int priority, uint32_t iteration)
{
	bench_time_t  end;

	/* Step 1 */

	bench_mutex_lock(MUTEX_ID);

	bench_thread_create(THREAD_HIGH, "thread_high",
			    priority - 1, bench_pend_high, NULL);
	bench_thread_start(THREAD_HIGH);

	end = bench_timing_counter_get();

	/* Step 5 */

	bench_mutex_unlock(MUTEX_ID);

	/* Step 8 */

	update_times(times[TIME_TO_PEND_PRI_INH],
		     bench_timing_cycles_get(&helper_start, &end),
		     iteration);
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
 * @brief Test setup function
 */
static void bench_mutex_lock_unlock_test(void *arg)
{
	uint32_t  i;

	bench_mutex_create(MUTEX_ID);
	bench_sem_create(SEM_ID, 0, 1);

	bench_thread_set_priority(MAIN_PRIORITY);

	bench_timing_init();

	bench_calibrate();

	reset_time_stats();

	for (i = 1; i <= ITERATIONS; i++) {
		gather_lock_unlock_stats(i);
	}

	bench_mutex_lock(MUTEX_ID);        /* Prep mutex so it is locked */

	for (i = 1; i <= ITERATIONS; i++) {
		gather_recursive_lock_stats(i);
	}

	for (i = 1; i <= ITERATIONS; i++) {
		gather_recursive_unlock_stats(i);
	}

	bench_mutex_unlock(MUTEX_ID);      /* Undo final lock */

	for (i = 1; i <= ITERATIONS; i++) {
		gather_unpend_stats(MAIN_PRIORITY, i);
	}

	for (i = 1; i <= ITERATIONS; i++) {
		gather_unpend_inheritance_stats(MAIN_PRIORITY, i);
	}

	for (i = 1; i <= ITERATIONS; i++) {
		gather_pend_stats(MAIN_PRIORITY, i);
	}

	for (i = 1; i <= ITERATIONS; i++) {
		gather_pend_inheritance_stats(MAIN_PRIORITY, i);
	}

	report_stats();
}

int main(void)
{
	bench_test_init(bench_mutex_lock_unlock_test);
	return 0;
}
