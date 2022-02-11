/* SPDX-License-Identifier: Apache-2.0 */

#ifndef PORTING_LAYER_FREERTOS_H_
#define PORTING_LAYER_FREERTOS_H_

#include "FreeRTOS.h"

typedef uint32_t bench_time_t;
typedef void * bench_work;

#define TICK_SYNCH()  k_sleep(K_TICKS(1))

#include "fsl_debug_console.h"

#define BENCH_PRINTF(FMT, ...) PRINTF(FMT, ##__VA_ARGS__)

#define PRINT_F(...) \
	do { \
		PRINTF("%s \t:\t%u cycles \t,\t%u ns\n", ##__VA_ARGS__); \
	} while(0)

/* https://mcuoneclipse.com/2017/01/30/cycle-counting-on-arm-cortex-m-with-dwt/ */

/* DWT (Data Watchpoint and Trace) registers, only exists on ARM Cortex with a DWT unit */
#define KIN1_DWT_CONTROL			(*((volatile uint32_t*)0xE0001000))
/*!< DWT Control register */
#define KIN1_DWT_CYCCNTENA_BIT		(1UL<<0)
/*!< CYCCNTENA bit in DWT_CONTROL register */
#define KIN1_DWT_CYCCNT				(*((volatile uint32_t*)0xE0001004))
/*!< DWT Cycle Counter register */
#define KIN1_DEMCR 					(*((volatile uint32_t*)0xE000EDFC))
/*!< DEMCR: Debug Exception and Monitor Control Register */
#define KIN1_TRCENA_BIT				(1UL<<24)
/*!< Trace enable bit in DEMCR register */

#define KIN1_InitCycleCounter() \
  KIN1_DEMCR |= KIN1_TRCENA_BIT
  /*!< TRCENA: Enable trace and debug block DEMCR (Debug Exception and Monitor Control Register */

#define KIN1_ResetCycleCounter() \
  KIN1_DWT_CYCCNT = 0
  /*!< Reset cycle counter */

#define KIN1_EnableCycleCounter() \
  KIN1_DWT_CONTROL |= KIN1_DWT_CYCCNTENA_BIT
  /*!< Enable cycle counter */

#define KIN1_DisableCycleCounter() \
  KIN1_DWT_CONTROL &= ~KIN1_DWT_CYCCNTENA_BIT
  /*!< Disable cycle counter */

#define KIN1_GetCycleCounter() \
  KIN1_DWT_CYCCNT
  /*!< Read cycle counter register */

#define ARG_UNUSED(x) (void)(x)

#endif /* PORTING_LAYER_FREERTOS_H_ */
