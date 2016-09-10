/*
 * canWrapper.c
 *
 *  Created on: Mar 19, 2016
 *      Author: shapa
 */


#include "api.h"
#include "stm32f0xx_can.h"
#include "cortexm/ExceptionHandlers.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx_can.h"
#include "diag/Trace.h"


void CEC_CAN_IRQHandler(void) {

	if (CAN_GetITStatus(CAN, CAN_IT_FMP0)) {
		CanRxMsg rx = {0};
		CAN_Receive(CAN, CAN_FIFO0, &rx);
		BSP_onCanActivity(&rx);
		trace_printf("CAN_IT_FMP0 \n");
		trace_printf("StdId %d \n", rx.StdId);
		trace_printf("ExtId %d \n", rx.ExtId);
		trace_printf("IDE %d \n", rx.IDE);
		trace_printf("RTR %d \n", rx.RTR);
		trace_printf("DLC %d \n", rx.DLC);

		for (uint32_t i = 0 ; i < rx.DLC; i++)
			trace_printf("\t\t D[%d] %d '%c' \n", i, rx.Data[i], rx.Data[i]);
	}
	if (CAN_GetITStatus(CAN, CAN_IT_FMP1)) {
		trace_printf("CAN_IT_FMP1 \n");
	}
	if (CAN_GetITStatus(CAN, CAN_IT_TME)) {
		trace_printf("CAN_IT_TME \n");
		trace_printf("\t MAIL 0 %d\n", CAN_TransmitStatus(CAN, 0));
		trace_printf("\t MAIL 1 %d\n", CAN_TransmitStatus(CAN, 1));
		trace_printf("\t MAIL 2 %d\n", CAN_TransmitStatus(CAN, 2));
		CAN_ClearITPendingBit(CAN, CAN_IT_TME);
	}

	if (CAN_GetITStatus(CAN, CAN_IT_EWG)) {
		trace_printf("EWG \n");
		CAN_ClearITPendingBit(CAN, CAN_IT_EWG);
	}
	if (CAN_GetITStatus(CAN, CAN_IT_EPV)) {
		trace_printf("EPV \n");
		CAN_ClearITPendingBit(CAN, CAN_IT_EPV);
	}
	if (CAN_GetITStatus(CAN, CAN_IT_BOF)) {
		trace_printf("BOF \n");
		CAN_ClearITPendingBit(CAN, CAN_IT_BOF);
	}
	if (CAN_GetITStatus(CAN, CAN_IT_LEC)) {
		trace_printf("LEC \n");
		CAN_ClearITPendingBit(CAN, CAN_IT_LEC);
	}
	if (CAN_GetITStatus(CAN, CAN_IT_ERR)) {
		trace_printf("ERR \n");
		CAN_ClearITPendingBit(CAN, CAN_IT_ERR);
	}
}
