#include "bench_api.h"
#include "bench_utils.h"

extern struct _isr_table_entry _sw_isr_table[];

bench_isr_handler_t bench_timer_isr_get(void)
{

	/*
	 * At the current time, it is assumed that the timer interrupt
	 * is NOT a level 2 interrupt and RISCV_MACHINE_TIMER_IRQ can
	 * simply be an index into _sw_isr_table[].
	 */

	return (bench_isr_handler_t)_sw_isr_table[RISCV_MACHINE_TIMER_IRQ].isr;
}

uint32_t bench_timer_cycles_per_second(void)
{
	return sys_clock_hw_cycles_per_sec();
}

uint32_t bench_timer_cycles_per_tick(void)
{
	uint64_t cyc_per_tick;

	cyc_per_tick = (uint64_t) bench_timer_cycles_per_second();
	cyc_per_tick >>= CONFIG_RISCV_MACHINE_TIMER_SYSTEM_CLOCK_DIVIDER;
	cyc_per_tick /= CONFIG_SYS_CLOCK_TICKS_PER_SEC;

	return (uint32_t) cyc_per_tick;;
}

void bench_timer_isr_set(bench_isr_handler_t  isr)
{
	_sw_isr_table[RISCV_MACHINE_TIMER_IRQ].isr = (void(*)(const void *))isr;

	return;
}
