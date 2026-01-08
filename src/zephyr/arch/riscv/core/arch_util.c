#include "bench_api.h"
#include "bench_utils.h"

#ifdef CONFIG_DYNAMIC_INTERRUPTS
extern struct _isr_table_entry _sw_isr_table[];
#else
#error need CONFIG_DYNAMIC_INTERRUPTS enabled to switch timer ISR!
#endif

extern const uint32_t riscv_timer_irq;

uint32_t bench_timer_cycles_per_second(void)
{
	return sys_clock_hw_cycles_per_sec();
}

void bench_timer_isr_set(bench_isr_handler_t  isr)
{
	_sw_isr_table[riscv_timer_irq].isr = (void(*)(const void *))isr;

	return;
}

bench_isr_handler_t bench_timer_isr_get(void)
{

	/*
	 * At the current time, it is assumed that the timer interrupt
	 * is NOT a level 2 interrupt and bench_porting_layer_timer_irq can
	 * simply be an index into _sw_isr_table[].
	 */
	return (bench_isr_handler_t)_sw_isr_table[riscv_timer_irq].isr;
}
