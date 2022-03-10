#include "bench_api.h"
#include "core_cm4.h"

/*
 * For the Cortex-M, the system tick ISR is at index 15 (offset 0x3c) in
 * the vector table. Simply updating this entry has been observed to cause
 * issues when the vector table resides at address 0. On Cortex-M systems
 * that have a VTOR (vector table offset register) we get around this by
 * creating a copy of the vector table and switch to using that one. This
 * allows us to easily change the timer ISR.
 */

#define TIMER_ISR_VECTOR_TABLE_INDEX  15

#if CONFIG_CPU_CORTEX_M_HAS_VTOR
static bench_isr_handler_t my_vector_table[16] __aligned(0x80);
#endif

bench_isr_handler_t bench_timer_isr_get(void)
{
	bench_isr_handler_t *table = (bench_isr_handler_t *)SCB->VTOR;

	return table[TIMER_ISR_VECTOR_TABLE_INDEX];
}

uint32_t bench_timer_cycles_per_second(void)
{
	return SYS_CLOCK_HW_CYCLES_PER_SEC;
}

uint32_t bench_timer_cycles_per_tick(void)
{
	return (bench_timer_cycles_per_second() /
		configTICK_RATE_HZ);
}

void bench_timer_isr_set(bench_isr_handler_t  isr)
{
#if CONFIG_CPU_CORTEX_M_HAS_VTOR
	int i;
	bench_isr_handler_t *src = (bench_isr_handler_t *)SCB->VTOR;

	for (i = 0; i < 16; i++) {
		my_vector_table[i] = src[i];
	}

	my_vector_table[TIMER_ISR_VECTOR_TABLE_INDEX] = isr;
	SCB->VTOR = (uint32_t)my_vector_table & SCB_VTOR_TBLOFF_Msk;

	__DSB();
	__ISB();

#else
#error "Unable to set ISR handler for Cortex-M"
#endif
}

bench_time_t bench_timer_cycles_diff(bench_time_t trigger_point,
				     bench_time_t sample_point)
{
	return (trigger_point - sample_point + 1);
}

bench_time_t bench_timer_cycles_get(void)
{
	bench_time_t  rv = (bench_time_t)SysTick->VAL;

	return rv;
}

bench_time_t bench_timer_isr_expiry_set(uint32_t usec)
{
	uint32_t  cycles_per_usec;
	uint32_t  cycles;

	cycles_per_usec = (bench_timer_cycles_per_second() + 999999) / 1000000;
	cycles = cycles_per_usec * usec;

	SysTick->LOAD = cycles;
	SysTick->VAL = cycles - 1;
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |
			  SysTick_CTRL_TICKINT_Msk |
			  SysTick_CTRL_CLKSOURCE_Msk);

	return (bench_time_t)cycles;
}

void bench_timer_isr_restore(bench_isr_handler_t handler)
{
	uint32_t  cycles;

	cycles = bench_timer_cycles_per_tick() - 1;
	SysTick->LOAD = cycles;
	SysTick->VAL = 0;             /* resets timer to cycles */
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |
			  SysTick_CTRL_TICKINT_Msk |
			  SysTick_CTRL_CLKSOURCE_Msk);

	bench_timer_isr_set(handler);
}
