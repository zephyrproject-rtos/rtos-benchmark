/*
 * Copyright (c) 2026 tinyVision.ai Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bench_api.h"
#include "bench_utils.h"

#define DT_DRV_COMPAT litex_timer0

#include <zephyr/kernel.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/init.h>
#include <zephyr/irq.h>
#include <zephyr/spinlock.h>
#include <zephyr/drivers/timer/system_timer.h>
#include <zephyr/sys/byteorder.h>

#include <soc.h>

#define TIMER_LOAD_ADDR				DT_INST_REG_ADDR_BY_NAME(0, load)
#define TIMER_RELOAD_ADDR			DT_INST_REG_ADDR_BY_NAME(0, reload)
#define TIMER_EN_ADDR				DT_INST_REG_ADDR_BY_NAME(0, en)
#define TIMER_UPDATE_VALUE_ADDR		DT_INST_REG_ADDR_BY_NAME(0, update_value)
#define TIMER_VALUE_ADDR			DT_INST_REG_ADDR_BY_NAME(0, value)
#define TIMER_EV_STATUS_ADDR		DT_INST_REG_ADDR_BY_NAME(0, ev_status)
#define TIMER_EV_PENDING_ADDR		DT_INST_REG_ADDR_BY_NAME(0, ev_pending)
#define TIMER_EV_ENABLE_ADDR		DT_INST_REG_ADDR_BY_NAME(0, ev_enable)
#define TIMER_UPTIME_LATCH_ADDR		DT_INST_REG_ADDR_BY_NAME(0, uptime_latch)
#define TIMER_UPTIME_CYCLES_ADDR	DT_INST_REG_ADDR_BY_NAME(0, uptime_cycles)

#define TIMER_EV			0x1
#define TIMER_IRQ			DT_INST_IRQN(0)
#define TIMER_DISABLE		0x0
#define TIMER_ENABLE		0x1
#define TIMER_UPTIME_LATCH	0x1

const uint32_t riscv_timer_irq = TIMER_IRQ;

BUILD_ASSERT(sizeof(bench_time_t) >= sizeof(uint64_t) && (bench_time_t)(-1) == (uint64_t)(-1),
			 "bench_time_t expected to be unsigned and able to contain 64-bits");

/* Instantly get cycles, no spinlocking */
static inline bench_time_t litex_cycles(void)
{
	uint64_t uptime_cycles;

	litex_write8(TIMER_UPTIME_LATCH, TIMER_UPTIME_LATCH_ADDR);
	uptime_cycles = litex_read64(TIMER_UPTIME_CYCLES_ADDR);

	return uptime_cycles;
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
	return (bench_time_t)litex_cycles();
}

/**
 * @brief Sets the timer ISR to trigger in @a usec microseconds
 *
 * This routine assumes that @a usec leads to a valid cycles calculation.
 */
bench_time_t bench_timer_isr_expiry_set(uint32_t usec)
{
	bench_time_t cycles_next;

	litex_write8(TIMER_DISABLE, TIMER_EN_ADDR);

	litex_write32(k_us_to_cyc_floor32(usec), TIMER_RELOAD_ADDR);
	litex_write32(k_us_to_cyc_floor32(usec), TIMER_LOAD_ADDR);

	litex_write8(litex_read8(TIMER_EV_PENDING_ADDR), TIMER_EV_PENDING_ADDR);
	litex_write8(TIMER_EV, TIMER_EV_ENABLE_ADDR);

	cycles_next = litex_cycles() + k_us_to_cyc_floor64(usec);

	litex_write8(TIMER_ENABLE, TIMER_EN_ADDR);

	return cycles_next;
}

/**
 * @brief Restore both the old timer ISR handler and its tick rate
 */
void bench_timer_isr_restore(bench_isr_handler_t handler)
{
	litex_write8(TIMER_DISABLE, TIMER_EN_ADDR);

	litex_write32(k_ticks_to_cyc_floor32(1), TIMER_RELOAD_ADDR);
	litex_write32(k_ticks_to_cyc_floor32(1), TIMER_LOAD_ADDR);

	litex_write8(TIMER_ENABLE, TIMER_EN_ADDR);
	litex_write8(litex_read8(TIMER_EV_PENDING_ADDR), TIMER_EV_PENDING_ADDR);
	litex_write8(TIMER_EV, TIMER_EV_ENABLE_ADDR);

	bench_timer_isr_set(handler);
}

/**
 * @brief Get cycles per tick
 */
uint32_t bench_timer_cycles_per_tick(void)
{
	return k_ticks_to_cyc_floor32(1);
}
