/*
 * Copyright (c) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bench_api.h"
#include "bench_utils.h"

#define MIN_DELAY  1000ULL

/* MTIME and MTIMECMP base registers currently hard-coded for use with QEMU */

#define RISCV_MTIME_BASE             0x0200BFF8
#define RISCV_MTIMECMP_BASE          0x02004000

/* Hard-coded divider--based upon zephyr's value for QEMU riscv32*/

#define CONFIG_RISCV_MACHINE_TIMER_SYSTEM_CLOCK_DIVIDER  0

/**
 * @brief Read the current cycle count from the MTIME register
 */
static uint64_t mtime(void)
{
#ifdef CONFIG_64BIT
        return *(volatile uint64_t *)RISCV_MTIME_BASE;
#else
        volatile uint32_t *r = (uint32_t *)RISCV_MTIME_BASE;
        uint32_t  lo;
        uint32_t  hi;

        do {
                hi = r[1];
                lo = r[0];
        } while (r[1] != hi);

        return (((uint64_t)hi) << 32) | lo;
#endif
}

/**
 * @brief Get the address of the MTIMECMP base register
 *
 * Unlike the zephyr version of this routine, this routine assumes a
 * single processor system.
 */
static uint64_t get_hart_mtimecmp(void)
{
	return RISCV_MTIMECMP_BASE;
}

/**
 * @brief Set the MTIME comparison register
 */
static void set_mtimecmp(uint64_t time)
{
#ifdef CONFIG_64BIT
        *(volatile uint64_t *)get_hart_mtimecmp() = time;
#else
        volatile uint32_t *r = (uint32_t *)(uint32_t)get_hart_mtimecmp();

        r[1] = 0xffffffff;
        r[0] = (uint32_t)time;
        r[1] = (uint32_t)(time >> 32);
#endif
}

/**
 * @brief Get the number of elapsed cycles between two raw cycle counts.
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
	return (bench_time_t)mtime();
}

/**
 * @brief Sets the timer ISR to trigger in @a usec microseconds
 *
 * This routine assumes that @a usec leads to a valid cycles calculation.
 */
bench_time_t bench_timer_isr_expiry_set(uint32_t usec)
{
	uint64_t cyc_per_sec;
	uint64_t cyc_per_tick;
	uint64_t next;

	cyc_per_sec = (uint64_t) bench_timer_cycles_per_second();
	cyc_per_sec >>= CONFIG_RISCV_MACHINE_TIMER_SYSTEM_CLOCK_DIVIDER;
	cyc_per_tick = (cyc_per_sec * usec) / 1000000ULL;

	if (cyc_per_tick < MIN_DELAY) {
		cyc_per_tick += MIN_DELAY;
	}

	next = mtime() + cyc_per_tick;

	set_mtimecmp(next);

	return (bench_time_t) next;
}

/**
 * @brief Restore both the old timer ISR handler and its tick rate
 */
void bench_timer_isr_restore(bench_isr_handler_t handler)
{
	bench_timer_isr_set(handler);
}
