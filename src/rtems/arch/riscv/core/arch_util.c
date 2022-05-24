#include "bench_api.h"
#include "bench_utils.h"

#include <rtems.h>
#include <bsp/irq.h>

extern void Clock_isr(void *arg);

bench_isr_handler_t bench_timer_isr_get(void)
{
	/*
	 * Unlike Zephyr, RTEMS has access to the name of the ISR handler.
	 * Assume that it is always this.
	 */

	return Clock_isr;
}

uint32_t bench_timer_cycles_per_second(void)
{
	return 1000;
}

void bench_timer_isr_set(bench_isr_handler_t  isr)
{
	rtems_status_code sc;

	sc = rtems_interrupt_handler_install(RISCV_INTERRUPT_VECTOR_TIMER,
					"Clock",
					RTEMS_INTERRUPT_UNIQUE,
					(rtems_interrupt_handler) isr,
					NULL);

	if (sc != RTEMS_SUCCESSFUL) {
		PRINTF("Failed to replace ISR\n");
	}

	return;
}
