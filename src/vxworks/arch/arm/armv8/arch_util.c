/*
 * Copyright (c) 2023 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bench_api.h"

/* read PMU cycle counter */

#define GET_PMCCNTR(reg) \
	__asm__ volatile("MRS %0, PMCCNTR_EL0" : "=r" (reg))

static void pmccntr_get(uint64_t *val)
{
	GET_PMCCNTR(*val);
}

bench_time_t time_cnt_get(void)
{
	bench_time_t time = 0UL;
	struct timespec ts;

	if (timerType == TIMER_ARM_PMU) {
		pmccntr_get(&time);
	} else {
		clock_gettime(CLOCK_MONOTONIC, &ts);
		time = (bench_time_t)ts.tv_sec * NSEC_PER_SEC + ts.tv_nsec;
	}
	return time;
}

