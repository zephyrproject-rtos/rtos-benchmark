/*
 * Copyright (c) 2023 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bench_api.h"

void bench_timing_init(void)
{
}

void bench_sync_ticks(void)
{
	taskDelay(1);
}

void bench_timing_start(void)
{
}

void bench_timing_stop(void)
{
}

bench_time_t bench_timing_counter_get(void)
{
	bench_time_t time = 0UL;

	time = time_cnt_get();

	return time;
}

bench_time_t bench_timing_cycles_get(bench_time_t *time_start,
		bench_time_t *time_end)
{
	return *time_end - *time_start;
}

bench_time_t bench_timing_cycles_to_ns(bench_time_t cycles)
{
	bench_time_t time = 0UL;

	time = cycles * (NSEC_PER_SEC / timerFreq);

	return time;
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
}

void bench_timer_isr_restore(bench_isr_handler_t handler)
{    
}

bench_time_t bench_timer_isr_expiry_set(uint32_t usec)
{
	return 0;
}

bench_time_t bench_timer_cycles_diff(bench_time_t trigger, bench_time_t sample)
{
	return sample - trigger;
}

bench_time_t bench_timer_cycles_get(void)
{
	return 0;
}

uint32_t bench_timer_cycles_per_second(void)
{
	return (uint32_t)timerFreq;
}

uint32_t bench_timer_cycles_per_tick(void)
{
	return (uint32_t)timerFreq / tickClkRate;
}

