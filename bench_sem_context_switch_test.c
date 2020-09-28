// SPDX-License-Identifier: Apache-2.0

#include "bench_api.h"
#include "bench_utils.h"

/* Number of give / take cycles on semaphore */
#define NUM_TEST_SEM 1000

bench_time_t timestamp_start_sema_t_c;
bench_time_t timestamp_end_sema_t_c;
bench_time_t timestamp_start_sema_g_c;
bench_time_t timestamp_end_sema_g_c;

void bench_sem_context_switch_low_prio_take(void)
{
	timestamp_start_sema_t_c = bench_timing_counter_get();
	bench_sem_take(0);
	timestamp_end_sema_g_c = bench_timing_counter_get();
}

void bench_sem_context_switch_high_prio_give(void)
{
	uint32_t diff;

	bench_timing_start();

	bench_thread_create(1, "low_prio_take", 3, bench_sem_context_switch_low_prio_take);
	bench_thread_resume(1);

	timestamp_end_sema_t_c = bench_timing_counter_get();
	diff = bench_timing_cycles_get(timestamp_start_sema_t_c, timestamp_end_sema_t_c);
	PRINT_STATS("Semaphore take time (context switch)", diff);

	timestamp_start_sema_g_c = bench_timing_counter_get();
	bench_sem_give(0);
	diff = bench_timing_cycles_get(timestamp_start_sema_g_c, timestamp_end_sema_g_c);
	PRINT_STATS("Semaphore give time (context switch)", diff);

	bench_timing_stop();
}

void bench_sem_context_switch_init(void)
{
	bench_timing_init();

	bench_sem_create(0, 0, 1);

	bench_thread_create(0, "high_prio_give", 10, bench_sem_context_switch_high_prio_give);
	bench_thread_resume(0);
}

int main(void)
{
	bench_test_init(bench_sem_context_switch_init);
	return 0;
}
