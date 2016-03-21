/*
 * api.h
 *
 *  Created on: Dec 28, 2015
 *      Author: shapa
 */

#ifndef API_H_
#define API_H_

#include "stm32f0xx.h"
#include <stdbool.h>
#include "circleBuffer.h"

typedef void (*control_f) (FunctionalState);
typedef bool (*getState_f) (void);
typedef bool (*sendData_f) (uint32_t id, uint8_t *data, uint8_t size);
typedef bool (*sendAirQuality_f) (uint8_t value);
typedef struct {
	struct {
		control_f setSTB;
		control_f setEN;
		control_f setWAKE;
		getState_f getERR;
	} hardwareLine;
	sendData_f sendData;
	sendAirQuality_f sendAirQuality;
} ifaceControl_t, *ifaceControl_p;


uint8_t BSP_init(void);
ifaceControl_p BSP_CANControl(void);

void Led_Red_SetState(FunctionalState);
void Led_Green_SetState(FunctionalState);

void SystemTimer_init(void);
void SystemTimer_delayMsDummy(uint32_t delay);

void ADC_registerBuffer(CircleBuffer_p);

void AirQuality_startup(void);
void AirQuality_periodic(void);


#endif /* API_H_ */
