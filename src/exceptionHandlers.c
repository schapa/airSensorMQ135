/*
 * exceprionHandler.c
 *
 *  Created on: Mar 4, 2016
 *      Author: shapa
 */



#include <stdbool.h>
#include "cortexm/ExceptionHandlers.h"
#include "stm32f0xx.h"

void Default_ExceptionHandler(void);

void Default_ExceptionHandler(void) {

#if defined(DEBUG)
	uint32_t interruptStatus = SCB->ICSR;
	uint8_t interruptNumber = (interruptStatus & 0xFF);
	__DEBUG_BKPT();
#endif

	while (true);
}

