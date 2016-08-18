/*
 * systemTimer.c
 *
 *  Created on: Jan 8, 2016
 *      Author: pavelgamov
 */

#include "api.h"
#include "systemStatus.h"
#include <stddef.h>

#define TICKS_PER_SECOND 1000

static struct {
	uint32_t activeTime;
	uint32_t passiveTime;
} s_timing[] = {
		[INFORM_INIT] = { 0.1*TICKS_PER_SECOND, 0.3*TICKS_PER_SECOND },
		[INFORM_PREHEAT] = { 1*TICKS_PER_SECOND, 0.25*TICKS_PER_SECOND },
		[INFORM_IDLE] = { 0.1*TICKS_PER_SECOND, TICKS_PER_SECOND },
		[INFORM_SLEEP] = { 0.05*TICKS_PER_SECOND, 2*TICKS_PER_SECOND},
		[INFORM_CONNECTION_LOST] = { 0.1*TICKS_PER_SECOND, 0.5*TICKS_PER_SECOND},
		[INFORM_ERROR] = { 0.05*TICKS_PER_SECOND, 0.05*TICKS_PER_SECOND},
};

static systemStatus_t s_systemStatus = INFORM_ERROR;
static uint32_t s_systemStatusTimer = 0;
static ledOutputControl_t s_systemLed = NULL;
static volatile uint32_t s_delayDecrement = 0;
static volatile uint32_t s_uptimeSeconds = 0;
static volatile uint32_t s_uptimeTicks = 0;

static struct {
	uint32_t sec;
	uint32_t msec;
} s_uptime;

void SystemStatus_setLedControl(ledOutputControl_t control) {
	s_systemLed = control;
}

void SystemStatus_set(systemStatus_t status) {

	if(status < INFORM_LAST) {
		s_systemStatus = status;
	}
}

void SystemTimer_init(void) {

	RCC_ClocksTypeDef RCC_ClockFreq;
	RCC_GetClocksFreq(&RCC_ClockFreq);
	SysTick_Config(RCC_ClockFreq.HCLK_Frequency / TICKS_PER_SECOND);

	SystemStatus_set(INFORM_SLEEP);
}

void SysTick_Handler(void) {
	uint32_t period = s_timing[s_systemStatus].activeTime + s_timing[s_systemStatus].passiveTime;
	if (s_systemLed)
		s_systemLed(s_systemStatusTimer <= s_timing[s_systemStatus].activeTime);
	if (++s_systemStatusTimer > period) {
		s_systemStatusTimer = 0;
	}
	if (++s_uptime.msec >= TICKS_PER_SECOND) {
		s_uptime.msec = 0;
		s_uptime.sec++;
		Event_t seconds = { EVENT_SYSTICK, { ES_SYSTICK_SECOND_ELAPSED }, .data.intptr = s_uptime.sec };
		BSP_queuePush(&seconds);
	}
	if (s_delayDecrement && s_delayDecrement--){};

	if (!(s_uptimeTicks++ % TICKS_PER_SECOND)) {
		s_uptimeSeconds++;
	}
}

void SystemTimer_delayMsDummy(uint32_t delay) {
	s_delayDecrement = delay;
	while (s_delayDecrement);
}

uint32_t SystemStatus_getUptime(void) {
	return s_uptimeSeconds;
}
