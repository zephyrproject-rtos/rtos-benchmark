#include "bench_utils.h"

#include "bench_api.h"

#include <assert.h>
#include <stdint.h>

void bench_stats_reset(struct bench_stats *stats)
{
	bench_time_t start, end;
	uint32_t i;

	stats->avg = 0;
	stats->min = (bench_time_t) -1;
	stats->max = 0;
	stats->total = 0;

	start = bench_timing_counter_get();
	for (i = 0; i < CALIBRATION_LOOPS; i++)
		bench_timing_counter_get();
	end = bench_timing_counter_get();

	stats->calibration = bench_timing_cycles_get(&start, &end) / CALIBRATION_LOOPS;
}

void bench_stats_update(struct bench_stats *stats, bench_time_t value,
			uint32_t iteration)
{
	assert(iteration != 0);

	value -= stats->calibration;

	if (value < stats->min)
		stats->min = value;

	if (value > stats->max)
		stats->max = value;

	stats->total += value;
	stats->avg = stats->total / iteration;
}

void bench_stats_report_title(const char *title)
{
	PRINTF("** %s [avg, min, max] in nanoseconds **\n\r", title);
}

void bench_stats_report_line(const char *summary, const struct bench_stats *stats)
{
	PRINTF(" %-40s: %6llu, %6llu, %6llu\n\r", summary,
	       bench_timing_cycles_to_ns(stats->avg),
	       bench_timing_cycles_to_ns(stats->min),
	       bench_timing_cycles_to_ns(stats->max));
}

void bench_stats_report_na(const char *summary)
{
	PRINTF(" %-40s: %6s, %6s, %6s\n\r", summary, "n/a", "n/a", "n/a");
}


#ifdef THINKER_ENABLED
struct list {
	struct list *next;
	int val;
};

struct list *append(struct list *list, int val)
{
	struct list *curr;

	if (!list) {
		list = bench_malloc(sizeof(*list));
		list->val = val;
		list->next = NULL;
		return list;
	}

	curr = list;
	while (curr->next)
		curr = curr->next;

	curr->next = bench_malloc(sizeof(*list));
	curr->next->val = val;
	curr->next->next = NULL;

	return list;
}

struct list *remove_last(struct list *list)
{
	struct list *curr, *prev;

	if (!list->next) {
		bench_free(list);
		return NULL;
	}

	prev = curr = list;
	while (curr->next) {
		prev = curr;
		curr = curr->next;
	}

	bench_free(curr);
	prev->next = NULL;

	return list;
}

void thinker(void)
{
	struct list *list = NULL;
	int i;

	for (i = 0; i < THINKER_STEPS; i++)
		list = append(list, i);

	while (list)
		list = remove_last(list);
}
#endif
