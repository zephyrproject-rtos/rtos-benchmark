#include <cmsis_core.h>

#include "bench_api.h"
#include "bench_utils.h"

extern void z_arm_int_exit();
extern void *_vector_table[];

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
	return sys_clock_hw_cycles_per_sec();
}

uint32_t bench_timer_cycles_per_tick(void)
{
	return (bench_timer_cycles_per_second() /
		CONFIG_SYS_CLOCK_TICKS_PER_SEC);
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
