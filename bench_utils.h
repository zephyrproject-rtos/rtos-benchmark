/* SPDX-License-Identifier: Apache-2.0 */

#ifndef _LATENCY_MEASURE_UNIT_H
#define _LATENCY_MEASURE_UNIT_H

#include <stdio.h>

#ifdef CSV_FORMAT_OUTPUT
#define FORMAT "%-60s,%8u,%8u\n"
#else
#define FORMAT "%-60s:%8u cycles , %8u ns\n"
#endif

#define PRINT_F(...)						\
	{							\
		char sline[256];				\
		snprintk(sline, 254, FORMAT, ##__VA_ARGS__);	\
		printf("%s", sline);					\
	}

#define PRINT_STATS(x, y) \
	PRINT_F(x, y, (uint32_t) bench_timing_cycles_to_ns(y))

#define PRINT_STATS_AVG(x, y, counter)	\
	PRINT_F(x, y / counter, (uint32_t)timing_cycles_to_ns_avg(y, counter))


#endif
