
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "diag/Trace.h"
#include "api.h"
#include "systemStatus.h"
#include "circleBuffer.h"

#include "stm32f0xx_can.h"

int main(int argc, char* argv[]) {

	(void)argc;
	(void)argv;

	CircleBuffer_p adcBuffer = CircleBuffer_new(512);
	ADC_registerBuffer(adcBuffer);

	SystemStatus_set(BSP_init() ? INFORM_IDLE : INFORM_ERROR);
	FunctionalState state = ENABLE;
	while (true) {
		CanTxMsg txMess = {
				0x123,
				0x12345678,
				CAN_Id_Standard,
				CAN_RTR_Data,
				1,
				.Data[0] = CircleBuffer_getAt(adcBuffer, CircleBuffer_getOccupiedSize(adcBuffer)-1)
		};

		uint8_t mailBox = CAN_Transmit(CAN, &txMess);
		if (mailBox == CAN_TxStatus_NoMailBox) {
//			trace_printf("[CAN] errno %p\n", CAN_GetLastErrorCode(CAN));
		}
		SystemStatus_set(BSP_CANControl()->hardwareLine.getERR() ? INFORM_ERROR : INFORM_IDLE);
		Led_Green_SetState(state);
		state = !state;
		SystemTimer_delayMsDummy(100);
	}
	return 0;
}

