// SPDX-License-Identifier: Apache-2.0

#include "bench_api.h"

void bench_hello_world_initialize(void)
{
	bench_thread_create(0, 10, bench_hello_world);
	bench_thread_create(1, 10, bench_hello_world);
	bench_thread_create(2, 10, bench_hello_world);
	bench_thread_create(3, 10, bench_hello_world);

	bench_thread_resume(0);
	bench_thread_resume(1);
	bench_thread_resume(2);
	bench_thread_resume(3);
}

void bench_hello_world(void)
{
	bench_serial_write("Hello, world!\n");
}

int main(void)
{
	bench_test_init(bench_hello_world_initialize);
	return 0;
}
