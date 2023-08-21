/*
 * Copyright (c) 2023 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bench_api.h"

bench_time_t time_cnt_get(void)
{
	bench_time_t time = 0UL;
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	time = (bench_time_t)ts.tv_sec * NSEC_PER_SEC + ts.tv_nsec;

	return time;
}

