// SPDX-License-Identifier: Apache-2.0

#include <rtems.h>
#include <stdlib.h>
#include <stdio.h>

#include "bench_api.h"
#include "bench_utils.h"

extern void bench_all(void *arg);

rtems_task Init(rtems_task_argument ignored)
{
	bench_test_init(bench_all);
	exit (0);
}
