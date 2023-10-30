/*
 * Copyright (c) 2023 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bench_api.h"

extern void bench_all (void *arg);

_Vx_freq_t tickClkRate;
uint64_t   timerFreq;
int        rtpMainPri;
TIMER_TYPE timerType;

/* Do not run interrupt latency test */

void bench_interrupt_latency_test(void *arg)
{
	ARG_UNUSED(arg);
}

int main (int argc, char *argv[])
{
	struct timespec ts;
	uint64_t        res;
	unsigned int    cpuAffIndex;
	cpuset_t        affinity;

	clock_getres (CLOCK_MONOTONIC, &ts);

	res = (bench_time_t)ts.tv_sec * NSEC_PER_SEC + ts.tv_nsec;

	tickClkRate = sysClkRateGet();

	taskCpuAffinityGet(taskIdSelf(), &affinity);
	cpuAffIndex = CPUSET_FIRST_INDEX(affinity);

	taskPriorityGet(taskIdSelf(), &rtpMainPri);

	if (argc > 2) {
		timerType = atoi(argv[1]);
		timerFreq = (uint64_t) atoll(argv[2]);
	} else {
		PRINTF ("\nWrong parameter number!\n");
		return -1;
	}

	PRINTF ("\nSystem Configurations:\n");
	PRINTF ("    - System tick clock frequency: %u Hz\n", tickClkRate);
	PRINTF ("    - Task CPU affinity on core: %d\n", cpuAffIndex);
	PRINTF ("    - Main task priority: %d\n", rtpMainPri);
	if (timerType == TIMER_ARM_PMU) {
		PRINTF ("    - PMU counter (CPU cycle) frequency: %llu Hz\n",
			timerFreq);
	} else {
		PRINTF ("    - POSIX high-res clock timer resolution: %llu ns\n",
			res);
		PRINTF ("        - Timer frequency from clock_gettime(): %llu Hz\n",
			timerFreq);
	}

	bench_test_init(bench_all);
	return 0;
}

