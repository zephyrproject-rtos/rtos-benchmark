// SPDX-License-Identifier: Apache-2.0

#include "bench_api.h"
#include "bench_utils.h"

static bench_time_t timestamp_start;
static bench_time_t timestamp_end;

static void bench_latency_test_isr(const void *unused)
{
	ARG_UNUSED(unused);

	bench_offload_submit_work();

	timestamp_start = bench_timing_counter_get();
}

static void bench_worker(bench_work *item)
{
	(void)item;

	timestamp_end = bench_timing_counter_get();

	bench_sem_give(1);
}

void bench_generate_interrupt_thread(void)
{
	bench_sem_take(0);
	irq_offload(bench_latency_test_isr, NULL);
	bench_thread_suspend(0);
}

void bench_interrupt_offload_latency_init(void)
{
	bench_timing_init();

	bench_sem_create(0, 0, 1); // Interrupt semaphore
	bench_sem_create(1, 0, 1); // Worker semaphore

	bench_thread_create(0, "gen_int_thread", 1, bench_generate_interrupt_thread);
	bench_thread_resume(0);

	uint32_t diff;

	bench_offload_create_work(bench_worker);

	bench_timing_start();
	TICK_SYNCH();

	bench_sem_give(0); // Give on interrupt semaphore
	bench_sem_take(1); // Take on worker semaphore

	bench_timing_stop();

	diff = timing_cycles_get(&timestamp_start, &timestamp_end);

	PRINT_STATS("Time from ISR to executing a different thread", diff)
}

int main(void)
{
	bench_test_init(bench_interrupt_offload_latency_init);
	return 0;
}
