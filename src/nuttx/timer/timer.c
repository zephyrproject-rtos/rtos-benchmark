/*
 * Copyright (c) 2022 Xiaomi Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bench_api.h"
#include <nuttx/arch.h>
#include <nuttx/clock.h>
#include <nuttx/wdog.h>

static struct wdog_s g_bench_wdog;
static wdentry_t g_bench_handler;

void bench_timing_init(void)
{
}

void bench_sync_ticks(void)
{
}

void bench_timing_start(void)
{
}

void bench_timing_stop(void)
{
}

bench_time_t bench_timing_counter_get(void)
{
	return up_perf_gettime();
}

bench_time_t bench_timing_cycles_get(bench_time_t *time_start,
	bench_time_t *time_end)
{
	return *time_end - *time_start;
}

bench_time_t bench_timing_cycles_to_ns(bench_time_t cycles)
{
	struct timespec ts;
	up_perf_convert(cycles, &ts);
	return (bench_time_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
}

static void dummy_isr(void *ptr)
{
}

bench_isr_handler_t bench_timer_isr_get(void)
{
	return dummy_isr;
}

void bench_timer_isr_set(bench_isr_handler_t handler)
{
	g_bench_handler = (wdentry_t)handler;
}

void bench_timer_isr_restore(bench_isr_handler_t handler)
{
	g_bench_handler = (wdentry_t)handler;
	wd_cancel(&g_bench_wdog);
}

bench_time_t bench_timer_isr_expiry_set(uint32_t usec)
{
	uint32_t ticks = USEC2TICK(usec);
	uint32_t count = (uint64_t)TICK2USEC(ticks) * up_perf_getfreq() / 1000000;

	wd_start(&g_bench_wdog, ticks, g_bench_handler, 0);
	return up_perf_gettime() + count;
}

bench_time_t bench_timer_cycles_diff(bench_time_t trigger,
	bench_time_t sample)
{
	return sample - trigger;
}

bench_time_t bench_timer_cycles_get(void)
{
	return up_perf_gettime();
}

uint32_t bench_timer_cycles_per_second(void)
{
	return up_perf_getfreq();
}

uint32_t bench_timer_cycles_per_tick(void)
{
	return up_perf_getfreq() / TICK_PER_SEC;
}