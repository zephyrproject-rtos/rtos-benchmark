#include "bench_api.h"
#include "bench_utils.h"

bench_isr_handler_t bench_timer_isr_get(void)
{
#ifdef CONFIG_HPET_TIMER
	/*
	 * The following is rather "hackish" but it gets the job done.
	 * The address of "hpet_isr()" is stored at &hpet_isr_irq2_stub + 3.
	 * This stems from the fact that ARCH_IRQ_CONNECT() generates ...
	 *    pushl 0x0        (6a 00)
	 *    pushl hpet_isr   (68 XX XX XX XX)
	 */

	extern uint32_t hpet_isr_irq2_stub;
	bench_isr_handler_t handler;
	uint32_t stub = (uint32_t)&hpet_isr_irq2_stub;

	handler = *((bench_isr_handler_t *)(stub + 3));

	return handler;
#else
	#error "Benchmark project does not yet support timer"
#endif
}

uint32_t bench_timer_cycles_per_second(void)
{
	return sys_clock_hw_cycles_per_sec();
}

uint32_t bench_timer_cycles_per_tick(void)
{
	return (bench_timer_cycles_per_second() /
		CONFIG_SYS_CLOCK_TICKS_PER_SEC);
}

void bench_timer_isr_set(bench_isr_handler_t  isr)
{
#ifdef CONFIG_HPET_TIMER
	/*
	 * The following is rather "hackish" but it gets the job done.
	 * The routine registered for handling the HPET's ISR is stored
	 * at &hpet_isr_irq2_stub + 3.
	 */

	extern uint32_t hpet_isr_irq2_stub;
	uint32_t stub = (uint32_t)&hpet_isr_irq2_stub;

	*((bench_isr_handler_t *)(stub + 3)) = isr;
#else
	#error "Benchmark project does not yet support timer"
#endif
	return;
}
