/*
 * systemStatus.h
 *
 *  Created on: Jan 9, 2016
 *      Author: pavelgamov
 */

#ifndef SYSTEMSTATUS_H_
#define SYSTEMSTATUS_H_

#include "api.h"

typedef enum {
	INFORM_INIT,
	INFORM_PREHEAT,
	INFORM_IDLE,
	INFORM_SLEEP,
	INFORM_CONNECTION_LOST,
	INFORM_ERROR,
	INFORM_LAST
} systemStatus_t;

typedef void (*ledOutputControl_t)(FunctionalState);

void SystemStatus_setLedControl(ledOutputControl_t);
void SystemStatus_set(systemStatus_t);

void SystemTimer_delayMsDummy(uint32_t delay);
uint32_t SystemStatus_getUptime(void);

#endif /* SYSTEMSTATUS_H_ */
