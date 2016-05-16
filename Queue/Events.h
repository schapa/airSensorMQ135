/*
 * Events.h
 *
 *  Created on: Apr 20, 2016
 *      Author: shapa
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include "stm32f0xx_can.h"
#include "stm32f0xx_usart.h"
#include <stdint.h>
#include <stddef.h>

typedef enum {
	EVENT_DUMMY,
	EVENT_SYSTICK,
	EVENT_EXTI,
	EVENT_UART,
	EVENT_USART,
	EVENT_UxART_Buffer,
	EVENT_CAN,
	EVENT_LAST
} EventTypes_t;

typedef enum {
	ES_SYSTICK_TICK,
	ES_SYSTICK_SECOND_ELAPSED
} SystickSubTypes_t;

typedef enum {
	ES_EXTI_DOWN,
	ES_EXTI_UP,
} ExtiSubTypes_t;

typedef enum {
	ES_UxART_RX,
	ES_UxART_TX,
	ES_UxART_RXTX,
	ES_UxART_ERROR
} UxartSubTypes_t;
typedef struct {
	uint8_t id;
	intptr_t buffer;
	size_t size;
} UxartDataTypes_t;

typedef enum {
	ES_CAN_RX,
	ES_CAN_TX,
	ES_CAN_ERROR
} CanSubTypes_t;
typedef struct {
	uint8_t id;
	union {
		CanTxMsg *txMsg;
		CanRxMsg *rxMsg;
	};
} CanDataTypes_t;

typedef union {
	SystickSubTypes_t systick;
	ExtiSubTypes_t exti;
	UxartSubTypes_t uxart;
	CanSubTypes_t can;
} EventSubTypes_t;

typedef union {
	intptr_t intptr;
	UxartDataTypes_t uxart;
	CanDataTypes_t can;
} EventDataTypes_t;


#endif /* EVENTS_H_ */
