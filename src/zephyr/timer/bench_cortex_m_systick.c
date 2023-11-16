/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cmsis_core.h>

#include "bench_api.h"
#include "bench_utils.h"

/**
 * @brief Get the number of elapsed cycles between two raw cycle counts.
 *
 * The SysTick timer is a downward counting timer. The interrupt is triggered
 * when this count reaches zero. However, the value returned by 
 * bench_timer_isr_expiry_set() is the value from which the countdown begins
 * (and is reset to when the interrupt occurs).
 *
 * This routine is expected only to be used with the interrupt latency test,
 * and is expected to be called shortly after the timer interrupt has fired.
 */
bench_time_t bench_timer_cycles_diff(bench_time_t trigger_point,
				     bench_time_t sample_point)
{
	return (trigger_point - sample_point + 1);
}

/**
 * @brief Get the timer's current cycles value
 *
 * This gets the raw cycle count. The SysTick timer is downward counting.
 */
bench_time_t bench_timer_cycles_get(void)
{
	bench_time_t  rv = (bench_time_t)SysTick->VAL;

	return rv;
}

/**
 * @brief Sets the timer ISR to trigger in @a usec microseconds
 *
 * This routine assumes that @a usec leads to a valid cycles calculation.
 */
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

/**
 * @brief Restore both the old timer ISR handler and its tick rate
 */
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
