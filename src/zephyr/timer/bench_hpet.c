/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bench_api.h"
#include "bench_utils.h"

/*
 * Admittedly hard-coding this value is not the best approach, but the aim
 * at the current time is to get something functional.
 */

#define HPET_BASE_ADRS  0xfed00000

#define HPET_REG_ADDR(off)    (HPET_BASE_ADRS + (off))

#define CLK_PERIOD_REG         HPET_REG_ADDR(0x04)
#define MAIN_COUNTER_LOW_REG   HPET_REG_ADDR(0xf0)
#define MAIN_COUNTER_HIGH_REG  HPET_REG_ADDR(0xf4)

#define TIMER0_COMPARATOR_LOW_REG   HPET_REG_ADDR(0x108)
#define TIMER0_COMPARATOR_HIGH_REG  HPET_REG_ADDR(0x10C)

#ifndef HPET_COUNTER_CLK_PERIOD
/* COUNTER_CLK_PERIOD (CLK_PERIOD_REG) is in femtoseconds (1e-15 sec) */
#define HPET_COUNTER_CLK_PERIOD         (1000000000000000ULL)
#endif

#ifndef HPET_CMP_MIN_DELAY
#define HPET_CMP_MIN_DELAY (1000)   /* Minimal delay for comparator */
#endif


/**
 * @brief Get the number of elapsed cycles between two raw cycle counts.
 *
 * The HPET timer is an upward counting timer. The interrupt is triggered
 * when this count reaches the comparator value.
 *
 * This routine is expected only to be used with the interrupt latency test,
 * and is expected to be called shortly after the timer interrupt has fired.
 */
bench_time_t bench_timer_cycles_diff(bench_time_t trigger_point,
				     bench_time_t sample_point)
{
	return (sample_point - trigger_point);
}

/**
 * @brief Get the timer's current cycles value
 *
 * This gets the raw cycle count. The HPET timer is upward counting.
 */
bench_time_t bench_timer_cycles_get(void)
{
	uint32_t  high;
	uint32_t  low;

	do {
		high = sys_read32(MAIN_COUNTER_HIGH_REG);
		low = sys_read32(MAIN_COUNTER_LOW_REG);
	} while (high != sys_read32(MAIN_COUNTER_HIGH_REG));

	return ((uint64_t)high << 32) | low;
}

static inline uint64_t hpet_timer_comparator_set(uint32_t cycles)
{
	uint64_t  value = bench_timer_cycles_get();

	value += cycles;

#if CONFIG_X86_64
        sys_write64(value, TIMER0_COMPARATOR_LOW_REG);
#else
        sys_write32((uint32_t)value, TIMER0_COMPARATOR_LOW_REG);
        sys_write32((uint32_t)(value >> 32), TIMER0_COMPARATOR_HIGH_REG);
#endif

	return value;
}

/**
 * @brief Sets the timer ISR to trigger in @a usec microseconds
 *
 * This routine assumes that @a usec leads to a valid cycles calculation.
 */
bench_time_t bench_timer_isr_expiry_set(uint32_t usec)
{
	uint32_t cyc_per_sec;
	uint32_t cyc_per_tick;
	cyc_per_sec = (uint32_t)
		      (HPET_COUNTER_CLK_PERIOD / sys_read32(CLK_PERIOD_REG));
	cyc_per_tick = (uint32_t)(((uint64_t)cyc_per_sec * usec) / 1000000U);

	if (cyc_per_tick <= HPET_CMP_MIN_DELAY) {
		cyc_per_tick += HPET_CMP_MIN_DELAY;
	}

	return hpet_timer_comparator_set(cyc_per_tick);
}

/**
 * @brief Restore both the old timer ISR handler and its tick rate
 */
void bench_timer_isr_restore(bench_isr_handler_t handler)
{
	bench_timer_isr_set(handler);
}
