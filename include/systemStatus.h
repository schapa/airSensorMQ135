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
	INFORM_IDLE,
	INFORM_CONNECTION_LOST,
	INFORM_SLEEP,
	INFORM_ERROR,
	INFORM_LAST
} systemStatus_t;

typedef void (*ledOutputControl_t)(FunctionalState);

void SystemStatus_setLedControl(ledOutputControl_t);
void SystemStatus_set(systemStatus_t);

#endif /* SYSTEMSTATUS_H_ */
