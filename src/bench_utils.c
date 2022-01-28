#include "bench_utils.h"

#include "bench_api.h"

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
