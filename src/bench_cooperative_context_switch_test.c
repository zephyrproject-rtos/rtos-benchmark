// SPDX-License-Identifier: Apache-2.0

#include "bench_api.h"
#include "bench_utils.h"

/* number of context switches */
#define NCTXSWITCH 10000

static bench_time_t timestamp_start;
static bench_time_t timestamp_end;

/* context switches counter */
static volatile uint32_t ctx_switch_counter;

/* context switch balancer. Incremented by one thread, decremented by another*/
static volatile int ctx_switch_balancer;

/**
 *
 * thread_one
 *
 * Fiber makes all the test preparations: registers the interrupt handler,
 * gets the first timestamp and invokes the software interrupt.
 *
 * @return N/A
 */
static void bench_thread_one(void)
{
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
 *
 * @brief Check the time when it gets executed after the semaphore
 *
 * Fiber starts, waits on semaphore. When the interrupt handler releases
 * the semaphore, thread  measures the time.
 *
 * @return 0 on success
 */
static void bench_thread_two(void)
{
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
 *
 * @return 0 on success
 */
int bench_coop_ctx_switch(void)
{
	ctx_switch_counter = 0U;
	ctx_switch_balancer = 0;

	bench_timing_start();

	bench_thread_create(0, "t1", 6, bench_thread_one);
	bench_thread_resume(0);

	bench_thread_create(1, "t2", 6, bench_thread_two);
	bench_thread_resume(1);

	if (ctx_switch_balancer > 3 || ctx_switch_balancer < -3) {
		printf("Balance is %d. FAILED", ctx_switch_balancer);
	// MISSING ERROR CHECK
	} else {
		uint32_t diff;

		diff = bench_timing_cycles_get(timestamp_start, timestamp_end);
		PRINT_STATS_AVG("Average context switch time between threads (coop)",
			diff, ctx_switch_counter);
	}

	bench_timing_stop();

	return 0;
}

void bench_interrupt_latency_init(void)
{
	bench_timing_init();

	bench_set_current_thread_prio(10);
	bench_sem_create(0, 0, 1);

	bench_coop_ctx_switch();
}

int main(void)
{
	bench_test_init(bench_interrupt_latency_init);
	return 0;
}
