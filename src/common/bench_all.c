// SPDX-License-Identifier: Apache-2.0

#include "bench_api.h"
#include "bench_utils.h"

extern void bench_basic_thread_ops(void *arg);
extern void bench_interrupt_latency_test(void *arg);
extern void bench_mutex_lock_unlock_test(void *arg);
extern void bench_sem_context_switch_init(void *arg);
extern void bench_sem_signal_release_init(void *arg);
extern void bench_thread_yield(void *arg);
extern void bench_malloc_free(void *arg);
extern void bench_message_queue_init(void *arg);

void bench_all(void *arg)
{
	PRINTF("\n\r *** Starting! ***\n\n\r");

	bench_basic_thread_ops(arg);
	bench_mutex_lock_unlock_test(arg);
	bench_sem_context_switch_init(arg);
	bench_sem_signal_release_init(arg);
	bench_thread_yield(arg);
	bench_malloc_free(arg);
	bench_message_queue_init(arg);

	/* This should be the last test as it can muck with the timer */

	bench_interrupt_latency_test(arg);

	PRINTF("\n\r *** Done! ***\n\r");
}

#if RTOS_HAS_MAIN_ENTRY_POINT
int main(void)
{
	bench_test_init(bench_all);
	return 0;
}
#endif
