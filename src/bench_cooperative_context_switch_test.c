// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 *
 * @brief Measure context switch time between cooperative threads
 *
 * This file contains thread (coop) context switch time measurement.
 * The thread starts two cooperative thread. One thread waits on a semaphore. The other,
 * after starting, releases a semaphore which enable the first thread to run.
 * Each thread increases a common global counter and context switch back and
 * forth by yielding the cpu. When counter reaches the maximal value, threads
 * stop and the average time of context switch is displayed.
 */

#include "bench_api.h"
#include "bench_utils.h"

#define NCTXSWITCH 10000 /* Number of context switches */

static bench_time_t timestamp_start;
static bench_time_t timestamp_end;

/* Context switches counter */
static volatile uint32_t ctx_switch_counter;

/* Context switch balancer. Incremented by one thread, decremented by another. */
static volatile int ctx_switch_balancer;

/**
 * @brief Measure time at beginning and end of context switching through yields
 *
 * The average time to context switch will be calculated by dividing by number
 * of context switches.
 */
static void bench_thread_one(void *args)
{
	ARG_UNUSED(args);

	bench_sem_take(0);

	timestamp_start = bench_timing_counter_get();

	while (ctx_switch_counter < NCTXSWITCH) {
		bench_yield();
		ctx_switch_counter++;
		ctx_switch_balancer--;
	}

	timestamp_end = bench_timing_counter_get();
}

/**
 * @brief Check the time when it gets executed after the semaphore
 *
 * Fiber starts, waits on semaphore. When the interrupt handler releases
 * the semaphore, other thread measures the time.
 */
static void bench_thread_two(void *args)
{
	ARG_UNUSED(args);

	bench_sem_give(0);

	while (ctx_switch_counter < NCTXSWITCH) {
		bench_yield();
		ctx_switch_counter++;
		ctx_switch_balancer++;
	}
}

/**
 *
 * @brief The test main function
 */
void bench_coop_ctx_switch(void)
{
	ctx_switch_counter = 0U;
	ctx_switch_balancer = 0;

	bench_timing_start();

	bench_thread_create(0, "t1", 6, bench_thread_one, NULL);
	bench_thread_start(0);

	bench_thread_create(1, "t2", 6, bench_thread_two, NULL);
	bench_thread_start(1);

	if (ctx_switch_balancer > 3 || ctx_switch_balancer < -3) {
		printf("Balance is %d. FAILED", ctx_switch_balancer);
	// MISSING ERROR CHECK
	} else {
		uint32_t diff;

		diff = bench_timing_cycles_get(&timestamp_start, &timestamp_end);
		PRINT_STATS_AVG("Average context switch time between threads (coop)",
			diff, ctx_switch_counter);
	}

	bench_timing_stop();
}

void bench_interrupt_latency_init(void)
{
	bench_timing_init();

	/* Ensure test main thread has lower priority than switching threads */
	bench_thread_set_priority(10);
	bench_sem_create(0, 0, 1);

	bench_coop_ctx_switch();
}

int main(void)
{
	bench_test_init(bench_interrupt_latency_init);
	return 0;
}
