// SPDX-License-Identifier: Apache-2.0

/**
 * @file
 *
 * @brief Measure time from ISR back to interrupted thread
 *
 * This file contains test that measures time to switch from the interrupt
 * handler back to the interrupted thread.
 */

#include "bench_api.h"
#include "bench_utils.h"

static volatile int flag_var;

static bench_time_t timestamp_start;
static bench_time_t timestamp_end;

/**
 *
 * @brief Test ISR used to measure best case interrupt latency
 *
 * The interrupt handler gets the first timestamp.
 */
static void bench_latency_test_isr(const void *unused)
{
	ARG_UNUSED(unused);
	flag_var = 1;

	timestamp_start = bench_timing_counter_get(); // End of ISR
}

/**
 * @brief Register ISR and invoke software interrupt.
 *
 * Function makes all test preparations: registering interrupt handler,
 * invoking software interrupt, and getting second timestamp.
 */
static void bench_make_interrupt(void)
{
	flag_var = 0;

	bench_irq_offload(bench_latency_test_isr, NULL);

	if (flag_var != 1) {
		printf("Flag variable has not changed. Interrupt latency test FAILED\n");
	} else {
		timestamp_end = bench_timing_counter_get(); // Resume from ISR
	}
}

/**
 * @brief Test main function
 */
void bench_interrupt_to_thread(void)
{
	uint32_t diff;

	bench_timing_start();
	bench_sync_ticks();

	bench_make_interrupt();

	if (flag_var == 1) {
		diff = bench_timing_cycles_get(&timestamp_start, &timestamp_end);
		PRINT_STATS("Switch from ISR back to interrupted thread", diff);
	}

	bench_timing_stop();
}

/**
 * @brief Test setup function
 */
void bench_interrupt_latency_init(void *arg)
{
	bench_timing_init();

	bench_interrupt_to_thread();
}

#ifdef RUN_INTERRUPT_LATENCY
int main(void)
{
	bench_test_init(bench_interrupt_latency_init);
	return 0;
}
#endif
