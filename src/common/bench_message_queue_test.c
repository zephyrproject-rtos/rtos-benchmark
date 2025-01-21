/*
 * Copyright (c) 2023 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Measure time for message queue create/send/receive.
 *
 * This file contains the test that measures message queue create,
 * and send/receive time between threads.
 */

#include "bench_api.h"
#include "bench_utils.h"

#if RTOS_HAS_MESSAGE_QUEUE

#define MAIN_PRIORITY (BENCH_LAST_PRIORITY - 3)
#define MSG_NUM       1
#define MSG_LEN       1
#define MQ_NAME       "bench_message_queue"
#define MQ_ID         0

#define THREAD_HIGH   1

static char msg_send_buf [MSG_LEN + 1] = "1";
static char msg_rcv_buf [MSG_LEN + 1] = "0";

static bench_time_t timestamp_start_mq_r_c;
static bench_time_t timestamp_end_mq_r_c;
static bench_time_t timestamp_start_mq_s_c;
static bench_time_t timestamp_end_mq_s_c;

static struct bench_stats create_times;
static struct bench_stats receive_times;
static struct bench_stats send_times;

/**
 * @brief Gather stats for creating message queue
 */
static void gather_create_stats(uint32_t iteration)
{
	bench_time_t  start;
	bench_time_t  end;

	start = bench_timing_counter_get();
	bench_message_queue_create(MQ_ID, MQ_NAME, MSG_NUM, MSG_LEN);
	end = bench_timing_counter_get();

	bench_message_queue_delete(MQ_ID, MQ_NAME);

	bench_stats_update(&create_times,
		   bench_timing_cycles_get(&start, &end),
		   iteration);

}

/**
 * @brief Gather stats for sending/recving message without context switch.
 */
static void gather_send_receive_stats(uint32_t iteration)
{
	bench_time_t  start;
	bench_time_t  mid;
	bench_time_t  end;

	start = bench_timing_counter_get();
	bench_message_queue_send(MQ_ID, msg_send_buf, MSG_LEN);
	mid = bench_timing_counter_get();
	bench_message_queue_receive(MQ_ID, msg_rcv_buf, MSG_LEN);
	end = bench_timing_counter_get();

	bench_stats_update(&send_times,
		   bench_timing_cycles_get(&start, &mid),
		   iteration);

	bench_stats_update(&receive_times,
		   bench_timing_cycles_get(&mid, &end),
		   iteration);

}

/**
 * @brief Test main function.
 *
 * High priority thread that receives message.
 */
static void bench_message_queue_high_prio_receive(void *args)
{
	ARG_UNUSED(args);

	bench_message_queue_receive(MQ_ID, msg_rcv_buf, MSG_LEN);
	timestamp_end_mq_s_c = bench_timing_counter_get();

	bench_thread_exit();
}

/**
 * @brief Gather stats for sending message with context switch.
 *
 * Low priority thread that sends message.
 */
static void gather_send_context_switch_stats(int priority, int iteration)
{
	bench_time_t diff;

	bench_thread_create(THREAD_HIGH, "high_prio_receive", priority - 1,
			    bench_message_queue_high_prio_receive, NULL);
	bench_thread_start(THREAD_HIGH);

	timestamp_start_mq_s_c = bench_timing_counter_get();
	bench_message_queue_send(MQ_ID, msg_send_buf, MSG_LEN);
	diff = bench_timing_cycles_get(&timestamp_start_mq_s_c,
				       &timestamp_end_mq_s_c);
	bench_stats_update(&send_times, diff, iteration);

}

/**
 * @brief Test main function.
 *
 * High priority thread that sends message.
 */
static void bench_message_queue_high_prio_send(void *args)
{
	ARG_UNUSED(args);

	bench_message_queue_send(MQ_ID, msg_send_buf, MSG_LEN);
	timestamp_end_mq_r_c = bench_timing_counter_get();

	bench_thread_exit();
}

/**
 * @brief Gather stats for recving message with context switch.
 *
 * Low priority thread that receives message.
 */
static void gather_receive_context_switch_stats(int priority, int iteration)
{
	bench_time_t diff;

	bench_thread_create(THREAD_HIGH, "high_prio_send", priority - 1,
			    bench_message_queue_high_prio_send, NULL);
	bench_thread_start(THREAD_HIGH);

	timestamp_start_mq_r_c = bench_timing_counter_get();
	bench_message_queue_receive(MQ_ID, msg_rcv_buf, MSG_LEN);

	diff = bench_timing_cycles_get(&timestamp_start_mq_r_c,
				       &timestamp_end_mq_r_c);
	bench_stats_update(&receive_times, diff, iteration);

}

#endif /* RTOS_HAS_MESSAGE_QUEUE */

/**
 * @brief Test setup function
 */
void bench_message_queue_init(void *arg)
{
#if RTOS_HAS_MESSAGE_QUEUE
	int i;

	bench_timing_init();
	bench_timing_start();

	bench_stats_reset(&create_times);
	bench_stats_reset(&receive_times);
	bench_stats_reset(&send_times);

	bench_stats_report_title("Message queue stats");

	bench_thread_set_priority(MAIN_PRIORITY);

	for (i = 1; i <= ITERATIONS; i++) {
		gather_create_stats(i);
	}

	bench_stats_report_line("Create", &create_times);

	bench_message_queue_create(MQ_ID, MQ_NAME, MSG_NUM, MSG_LEN);

	for (i = 1; i <= ITERATIONS; i++) {
		gather_send_receive_stats(i);
	}

	bench_stats_report_line("Send (no context switch)", &send_times);
	bench_stats_report_line("Receive (no context switch)", &receive_times);

	bench_stats_reset(&send_times);
	bench_stats_reset(&receive_times);

	for (i = 1; i <= ITERATIONS; i++) {
		gather_send_context_switch_stats(MAIN_PRIORITY, i);
		bench_collect_resources();
	}

	/* Send one message, so the message queue is full */

	bench_message_queue_send(MQ_ID, msg_send_buf, MSG_LEN);

	for (i = 1; i <= ITERATIONS; i++) {
		gather_receive_context_switch_stats(MAIN_PRIORITY, i);
		bench_collect_resources();
	}

	bench_message_queue_receive(MQ_ID, msg_rcv_buf, MSG_LEN);

	bench_stats_report_line("Send (context switch)", &send_times);
	bench_stats_report_line("Receive (context switch)", &receive_times);

	bench_message_queue_delete(MQ_ID, MQ_NAME);

	bench_timing_stop();
#else
	bench_stats_report_title("Message queue stats");

	bench_stats_report_na("Create");
	bench_stats_report_na("Send (no context switch)");
	bench_stats_report_na("Receive (no context switch)");
	bench_stats_report_na("Send (context switch)");
	bench_stats_report_na("Receive (context switch)");
#endif
}

#ifdef RUN_MESSAGE_QUEUE
int main(void)
{
	PRINTF("\n\r *** Starting! ***\n\n\r");

	bench_test_init(bench_message_queue_init);

	PRINTF("\n\r *** Done! ***\n\r");

	return 0;
}
#endif
