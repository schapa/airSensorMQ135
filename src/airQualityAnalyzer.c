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

#include "stm32f0xx_can.h"

#define PREHEAT_TIME 20

static uint8_t filtredValue(CircleBuffer_p s_adcBuffer);

static CircleBuffer_p s_adcBuffer = NULL;

void AirQuality_startup(void) {

	while (!BSP_init()) {
		SystemStatus_set(INFORM_ERROR);
		SystemTimer_delayMsDummy(100);
		BSP_CANControl()->hardwareLine.setSTB(ENABLE);
		BSP_CANControl()->hardwareLine.setEN(DISABLE);
//		BSP_CANControl()->hardwareLine.setWAKE(ENABLE);
		SystemTimer_delayMsDummy(2000);
	}
	SystemTimer_delayMsDummy(100);
	SystemStatus_set(INFORM_PREHEAT);
	BSP_CANControl()->sendData(0x22, "HELLO AQ", 8);

	while (SystemStatus_getUptime() < PREHEAT_TIME) {
		SystemTimer_delayMsDummy(100);
	}
	SystemStatus_set(INFORM_IDLE);

	s_adcBuffer = CircleBuffer_new(512);
	ADC_registerBuffer(s_adcBuffer);
}

void AirQuality_periodic(void) {

	static FunctionalState state = ENABLE;

	if (CircleBuffer_getOccupiedSize(s_adcBuffer) == CircleBuffer_size(s_adcBuffer)) {

		Led_Green_SetState(ENABLE);
		BSP_CANControl()->sendAirQuality(filtredValue(s_adcBuffer));
		CircleBuffer_dropData(s_adcBuffer);
		Led_Green_SetState(DISABLE);
		state = !state;
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
