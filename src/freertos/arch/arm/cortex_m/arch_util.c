#include "arch_api.h"
#include "core_cm4.h"
#include "bench_api.h"
#include "bench_utils.h"
#include "FreeRTOS.h"

#include <assert.h>

#define NSEC_PER_SEC 1000000000ULL

void arch_timing_init(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	/* Asssert cycle counter is indeed implemented. */
	assert((DWT->CTRL & DWT_CTRL_NOCYCCNT_Msk) == 0);
}

void arch_timing_start(void)
{
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void arch_timing_stop(void)
{
	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
}

bench_time_t arch_timing_counter_get(void)
{
	return (bench_time_t)DWT->CYCCNT;
}

bench_time_t arch_timing_cycles_get(volatile bench_time_t *const start,
				    volatile bench_time_t *const end)
{
	return (*(uint32_t *)end - *(uint32_t *)start);
}

bench_time_t arch_timing_cycles_to_ns(bench_time_t cycles)
{
	return (cycles * NSEC_PER_SEC) / SYS_CLOCK_HW_CYCLES_PER_SEC;
}
