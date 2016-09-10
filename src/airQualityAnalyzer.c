/*
 * airQualityAnalyzer.c
 *
 *  Created on: Mar 21, 2016
 *      Author: shapa
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "diag/Trace.h"
#include "api.h"
#include "systemStatus.h"
#include "circleBuffer.h"
#include "stm32f0xx_pwr.h"

#include "stm32f0xx_can.h"

#define PREHEAT_TIME 20

static uint8_t filtredValue(CircleBuffer_p s_adcBuffer);

static CircleBuffer_p s_adcBuffer = NULL;

static struct {
	_Bool isConnectionOk;
	_Bool isReadyForUse;

	_Bool isErrorPresent;

	uint8_t busErrorCount;
	uint8_t ConnectionLostCount;
} s_systemErrorInfo;

void AirQuality_startup(void) {

	uint8_t startupTryCount = 10;

	s_systemErrorInfo.isConnectionOk = BSP_start();
	if (BSP_start()) {
		s_systemErrorInfo.isConnectionOk = true;
	} else do {
		SystemStatus_set(INFORM_ERROR);
		Led_Green_SetState(ENABLE);
		SystemTimer_delayMsDummy(2000);
		if (!startupTryCount--) {
			s_systemErrorInfo.isConnectionOk = false;
			s_systemErrorInfo.isReadyForUse = false;
			/* if failed to initialize CAN-PHY - go to sleep.
			 * On line restore Transceiver will wake up us */
			BSP_CANControl()->hardwareLine.setSTB(ENABLE);
			SystemTimer_delayMsDummy(20);
			/* If CPU is still running at this time 12v power is absent
			 * Seems that there is external 3v3 power. Exit from Loop.
			 */
			break;
		}
		Led_Green_SetState(DISABLE);
		s_systemErrorInfo.isConnectionOk = BSP_start();
	} while (!s_systemErrorInfo.isConnectionOk);

	s_systemErrorInfo.isConnectionOk = true;
	SystemStatus_set(INFORM_PREHEAT);
	while (SystemStatus_getUptime() < PREHEAT_TIME) {
		SystemTimer_delayMsDummy(500);
//		BSP_CANControl()->sendHartbeat();
	}

	SystemStatus_set(INFORM_IDLE);
	s_adcBuffer = CircleBuffer_new(512);
	ADC_registerBuffer(s_adcBuffer);
	s_systemErrorInfo.isReadyForUse = true;
}

void AirQuality_periodic(void) {

	if (CircleBuffer_getOccupiedSize(s_adcBuffer) == CircleBuffer_size(s_adcBuffer)) {

		Led_Green_SetState(ENABLE);
		BSP_CANControl()->sendAirQuality(filtredValue(s_adcBuffer));
		CircleBuffer_dropData(s_adcBuffer);
		Led_Green_SetState(DISABLE);
	}
}


static uint8_t filtredValue(CircleBuffer_p s_adcBuffer) {
	uint32_t value = 0;
	if (s_adcBuffer) {
		const uint16_t size = CircleBuffer_getOccupiedSize(s_adcBuffer);
		for (uint16_t i = 0; i < size; i++)
			value += CircleBuffer_getAt(s_adcBuffer, i);
		value /= size;
	}
	return value;
}
