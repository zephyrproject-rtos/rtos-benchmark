// SPDX-License-Identifier: Apache-2.0

/**
 * @file Measure time for interrupt latency
 *
 * This file contains the test that measures the time from when a hardware
 * interrupt is triggered, until its associated interrupt handler begins
 * to execute.
 */

#include "bench_api.h"
#include "bench_utils.h"

#define SEM_ID      0

#define THREAD_LOW  0

#define MAIN_THREAD_PRIORITY   (BENCH_LAST_PRIORITY - 3)

#define ISR_DELAY  1000     /* Time in microseconds until ISR fires */

volatile bench_time_t  bench_isr_cycles;
volatile bench_time_t  bench_trigger_cycles;
volatile bench_time_t  diff_cycles;

struct bench_stats latency_times;

bench_isr_handler_t  old_timer_isr;

volatile bool valid_measurement = false;

static volatile bool run_thread_low = true;

/**
 * @brief Display the interrupt latency stats
 */
void report_stats(void)
{
	bench_stats_report_line("Latency", &latency_times);
}

/**
 * @brief Routine called at of irq_latency_isr()
 *
 * This routine is intended to only be called prior to leaving the ISR
 * irq_latency_isr(). The default implementation here invokes the previously
 * installed timer ISR. If alternative actions are desired, a customized
 * version can be used in its place.
 */
__weak void bench_exit_timer_isr(void)
{
	old_timer_isr(NULL);
}

/**
 * @brief Special ISR used to measure irq latency
 */
static void irq_latency_isr(void *arg)
{
	bench_isr_cycles = bench_timer_cycles_get();

	diff_cycles = bench_timer_cycles_diff(bench_trigger_cycles,
					      bench_isr_cycles);

	valid_measurement = ((int)(diff_cycles) >= 0);
	bench_sem_give_from_isr(SEM_ID);

	/*
	 * There are two possible ways to this ISR. If the old timer ISR exists
	 * we invoke it and let it take care of any required ISR exitting
	 * instructions. If it does not, then we invoke a generic timer ISR
	 * exit routine.
	 */

	bench_exit_timer_isr();
}

/**
 * @brief Gather IRQ latency statistics
 *
 * @return true if a valid sample obtained
 */
bool gather_irq_latency_stats(uint32_t  i)
{
	valid_measurement = false;

	bench_trigger_cycles = bench_timer_isr_expiry_set(ISR_DELAY);

	bench_sem_take(SEM_ID);

	/*
	 * Since we are dealing with timer interrupts and cycle register reads,
	 * we want to simplify things so that we do not have to do deal with
	 * hardware corner cases (such as register wrap-around).
	 *
	 * Thus we only deal with clearly valid measurements.
	 */

	if (valid_measurement) {
		bench_stats_update(&latency_times, diff_cycles, i);
	}

	return valid_measurement;
}

/**
 * @brief A low priority thread to prevent the system from going to idle
 */
static void bench_thread_low(void *args)
{
	ARG_UNUSED(args);

	while (run_thread_low) {
		/*
		 * Just spin. By preventing the system from going to "idle",
		 * we are really preventing the possibility of it from
		 * powering down.
		 */
	}

	bench_thread_exit();
}

/**
 * @brief Test setup function
 */
void bench_interrupt_latency_test(void *arg)
{
	uint32_t  i;

	bench_stats_reset(&latency_times);
	bench_stats_report_title("Interrupt Stats");

	bench_sem_create(SEM_ID, 0, 1);

	bench_thread_set_priority(MAIN_THREAD_PRIORITY);

	bench_thread_create(THREAD_LOW, "thread_low",
			    MAIN_THREAD_PRIORITY + 1, bench_thread_low, NULL);
	bench_thread_start(THREAD_LOW);

	/*
	 * Align to a tick boundary to eliminate likelihood of a system tick
	 * unintentionally interrupting the test.
	 */

	bench_sync_ticks();

	old_timer_isr = bench_timer_isr_get();
	bench_timer_isr_set(irq_latency_isr);

	bench_sync_ticks();

	for (i = 1; i <= ITERATIONS; i++) {
		if (!gather_irq_latency_stats(i)) {
			/* Repeat the test if the data was not good */
			i--;
		}
	}

	/*
	 * Reset timer interrupt frequency to 1 Hz
	 * (or closest that is allowed).
	 */

	bench_timer_isr_restore(old_timer_isr);

	run_thread_low = false;

	/*
	 * Lower the priority of the main thread to allow the low priority
	 * thread to finish and then restore the priority of the main thread.
	 */

	bench_thread_set_priority(MAIN_THREAD_PRIORITY + 2);
	bench_thread_set_priority(MAIN_THREAD_PRIORITY);

	report_stats();
}

#ifdef RUN_INTERRUPT_LATENCY
int main(void)
{
	PRINTF("\n\r *** Starting! ***\n\n\r");

	bench_test_init(bench_interrupt_latency_test);

	PRINTF("\n\r *** Done! ***\n\r");

	return 0;
}
#endif
