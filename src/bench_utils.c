#include "bench_utils.h"

#include <stdint.h>

void bench_stats(uint32_t *times, int count, uint64_t *avg, uint64_t *min,
		 uint64_t *max)
{
	int save_count = count;
	uint64_t sum = 0;

	*min = UINT64_MAX;
	*max = 0;

	while (--count >= 0) {
		sum += times[count];

		if (times[count] > *max)
			*max = times[count];
		if (times[count] < *min)
			*min = times[count];
	}

	*avg = sum / save_count;
}
