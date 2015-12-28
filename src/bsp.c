/*
 * bsp.c
 *
 *  Created on: Dec 28, 2015
 *      Author: shapa
 */

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "api.h"
#include <stdbool.h>
#include <stdint.h>

static void initialize_RCC_GPIO(void);
static void initialize_RCC_CAN(void);
static void initialize_GPIO_CAN(void);
static void configure_CAN(void);
static void configure_CAN_NVIC(void);

static void setSTBState(FunctionalState);
static void setENState(FunctionalState);
static void setWAKEState(FunctionalState);
static bool getERRState(void);

static ifaceControl_t canInterface = { {setSTBState, setENState, setWAKEState, getERRState} };

void BSP_init(void) {
	initialize_RCC_GPIO();
	initialize_RCC_CAN();
	initialize_GPIO_CAN();
	configure_CAN();
	configure_CAN_NVIC();
}

ifaceControl_p BSP_CANControl(void) {
	return &canInterface;
}

static void initialize_RCC_GPIO(void) {
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_GPIOB, ENABLE);
}

static void initialize_RCC_CAN(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);
}

static void initialize_GPIO_CAN(void) {

	GPIO_InitTypeDef iface = {0};
	GPIO_InitTypeDef ifaceControl = {0};
	GPIO_InitTypeDef ifaceFeedback = {0};

	iface.GPIO_Mode = GPIO_Mode_AF;
	iface.GPIO_OType = GPIO_OType_PP;
	iface.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	iface.GPIO_PuPd = GPIO_PuPd_NOPULL;
	iface.GPIO_Speed = GPIO_Speed_Level_1;

	ifaceControl.GPIO_Mode = GPIO_Mode_OUT;
	ifaceControl.GPIO_OType = GPIO_OType_PP;
	ifaceControl.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	ifaceControl.GPIO_PuPd = GPIO_PuPd_NOPULL;
	ifaceControl.GPIO_Speed = GPIO_Speed_Level_1;

	ifaceFeedback.GPIO_Mode = GPIO_Mode_IN;
	ifaceFeedback.GPIO_OType = GPIO_OType_PP;
	ifaceFeedback.GPIO_Pin = GPIO_Pin_1;
	ifaceFeedback.GPIO_PuPd = GPIO_PuPd_UP;
	ifaceFeedback.GPIO_Speed = GPIO_Speed_Level_1;

	GPIO_Init(GPIOA, &iface);
	GPIO_Init(GPIOA, &ifaceControl);
	GPIO_Init(GPIOB, &ifaceFeedback);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_4);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_4);
}

static void setSTBState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, val);
}

static void setENState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(GPIOA, GPIO_Pin_6, val);
}

static void setWAKEState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(GPIOA, GPIO_Pin_7, val);
}

static bool getERRState(void) {
	return (bool)GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_1);
}

static void configure_CAN(void) {
	uint8_t initResult = 0;
	CAN_InitTypeDef iface = {0};
	CAN_FilterInitTypeDef  ifaceFilter ={0};
	RCC_ClocksTypeDef RCC_Clocks;
	const uint32_t baudRate = 125000;

	RCC_GetClocksFreq(&RCC_Clocks);

	iface.CAN_TTCM = DISABLE;
	iface.CAN_ABOM = DISABLE;
	iface.CAN_AWUM = ENABLE;
	iface.CAN_NART = ENABLE;
	iface.CAN_RFLM = DISABLE;
	iface.CAN_TXFP = DISABLE;
#ifndef DEBUG
	iface.CAN_Mode = CAN_Mode_Normal;
#else
	iface.CAN_Mode = CAN_Mode_LoopBack;
#endif
	iface.CAN_SJW = CAN_SJW_1tq;
	iface.CAN_BS1 = CAN_BS1_4tq;
	iface.CAN_BS2 = CAN_BS2_3tq;
	iface.CAN_Prescaler = RCC_Clocks.PCLK_Frequency/(baudRate*(1+4+3)); //(CAN_SJW + CAN_BS1 + CAN_BS2)

	initResult = CAN_Init(CAN, &iface);

	ifaceFilter.CAN_FilterNumber = 0;
	ifaceFilter.CAN_FilterMode = CAN_FilterMode_IdMask;
	ifaceFilter.CAN_FilterScale = CAN_FilterScale_32bit;
	ifaceFilter.CAN_FilterIdHigh = 0x0000;
	ifaceFilter.CAN_FilterIdLow = 0x0000;
	ifaceFilter.CAN_FilterMaskIdHigh = 0x0000;
	ifaceFilter.CAN_FilterMaskIdLow = 0x0000;
	ifaceFilter.CAN_FilterFIFOAssignment = CAN_FIFO0;

	ifaceFilter.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&ifaceFilter);

	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_FMP1, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_TME, ENABLE);
}

static void configure_CAN_NVIC(void){
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = CEC_CAN_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
