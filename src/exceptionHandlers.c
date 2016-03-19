/*
 * exceprionHandler.c
 *
 *  Created on: Mar 4, 2016
 *      Author: shapa
 */



#include <stdbool.h>
#include "cortexm/ExceptionHandlers.h"
#include "stm32f0xx.h"
#include "diag/Trace.h"

void Default_ExceptionHandler(void);

void Default_ExceptionHandler(void) {

#if defined(DEBUG)
	const uint8_t irq0_Offset = 16;
	uint32_t interruptStatus = SCB->ICSR & 0x3F;
	uint8_t interruptNumber = SCB->ICSR & 0x3F;
#if defined(TRACE)
	trace_printf("Default_ExceptionHandler:");
	if ((interruptStatus & 0x3F) >= irq0_Offset) {
		interruptNumber -= irq0_Offset;
		trace_printf("\t IRQ%d\n", interruptNumber);
	} else {
		trace_printf("\t System ISR [%d]\n", interruptNumber);
	}
#endif
	__DEBUG_BKPT();
#endif

	while (true);
}

