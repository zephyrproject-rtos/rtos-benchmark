// SPDX-License-Identifier: Apache-2.0

#include "bench_api.h"
#include "bench_utils.h"

extern void bench_basic_thread_ops(void *arg);
extern void bench_interrupt_latency_init(void *arg);
extern void bench_mutex_lock_unlock_test(void *arg);
extern void bench_sem_context_switch_init(void *arg);
extern void bench_sem_signal_release_init(void *arg);
extern void bench_thread_yield(void *arg);

void bench_all(void *arg)
{
	BENCH_PRINTF("\n *** Starting! ***\n\n");

	bench_basic_thread_ops(arg);
	bench_interrupt_latency_init(arg);
	bench_mutex_lock_unlock_test(arg);
	bench_sem_context_switch_init(arg);
	bench_sem_signal_release_init(arg);
	bench_thread_yield(arg);

	BENCH_PRINTF("\n *** Done! ***\n");
}

int main(void)
{
	bench_test_init(bench_all);
	return 0;
}
