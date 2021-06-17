// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 *
 * @brief Measure time from ISR to a rescheduled thread
 *
 * This file contains test that measures time to switch from an interrupt
 * handler to executing a thread after rescheduling. In other words, execution
 * after interrupt handler resumes in a different thread than the one which got
 * interrupted.
 */

#include "bench_api.h"
#include "bench_utils.h"

static bench_time_t timestamp_start;
static bench_time_t timestamp_end;

/**
 *
 * @brief Test ISR used to measure best case interrupt latency
 *
 * The ISR gets the first timestamp.
 */
static void bench_latency_test_isr(const void *unused)
{
	ARG_UNUSED(unused);

	bench_offload_submit_work(); /* Submit work to offload */

	timestamp_start = bench_timing_counter_get();
}

/**
 * @brief Worker processing work offloaded from ISR
 *
 * This routine gets the second timestamp when it begins executing sometime
 * after the ISR finishes executing.
 */
static void bench_worker(bench_work *item)
{
	ARG_UNUSED(item);

	timestamp_end = bench_timing_counter_get();

	bench_sem_give(1); /* Signal test main thread that measurement is done */
}

/**
 * @brief Generate software interrupt
 *
 * Lower priority thread that, when it starts, it waits for a semaphore. When
 * it gets it, released by the main thread, sets up the interrupt handler and
 * generates the software interrupt
 *
 * @return 0 on success
 */
void bench_generate_interrupt_thread(void *args)
{
	ARG_UNUSED(args);

	bench_sem_take(0); /* Wait on interrupt semaphore */
	irq_offload(bench_latency_test_isr, NULL);
	bench_thread_suspend(0);
}

/**
 * @brief Test main function
 */
void bench_interrupt_offload_latency(void)
{
	uint32_t diff;

	/* Prepare to offload work from ISR */
	bench_offload_setup();
	bench_offload_create_work(bench_worker);

	bench_timing_start();
	TICK_SYNCH();

	bench_sem_give(0); /* Give on interrupt semaphore */
	bench_sem_take(1); /* Take on worker semaphore */

	bench_timing_stop();

	diff = timing_cycles_get(&timestamp_start, &timestamp_end);

	PRINT_STATS("Time from ISR to executing a different thread", diff)
}

/**
 * @brief Test setup function
 */
void bench_interrupt_offload_latency_init(void *arg)
{
	bench_timing_init();

	bench_sem_create(0, 0, 1); /* Interrupt semaphore */
	bench_sem_create(1, 0, 1); /* Worker semaphore */

	bench_thread_create(0, "gen_int_thread", 1, bench_generate_interrupt_thread, NULL);
	bench_thread_start(0);

	bench_interrupt_offload_latency();
}

int main(void)
{
	bench_test_init(bench_interrupt_offload_latency_init);
	return 0;
}
