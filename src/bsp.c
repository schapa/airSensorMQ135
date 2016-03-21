/*
 * bsp.c
 *
 *  Created on: Dec 28, 2015
 *      Author: shapa
 */

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_adc.h"
#include "stm32f0xx_tim.h"
#include "stm32f0xx_can.h"
#include <stdbool.h>
#include <stdint.h>
#include "api.h"
#include "systemStatus.h"

static void initialize_RCC(void);
static void initialize_GPIO_CAN(void);
static void initialize_GPIO_LED(void);
static void initialize_GPIO_ADC(void);

static uint8_t configure_CAN(void);
static void configure_CAN_NVIC(void);
static void configure_ADC(void);
static void configure_ADC_NVIC(void);
static void configure_TIM1(void);

static void setSTBState(FunctionalState);
static void setENState(FunctionalState);
static void setWAKEState(FunctionalState);
static bool getERRState(void);

static bool sendData(uint32_t id, uint8_t *data, uint8_t size) ;
static bool sendAirQuality(uint8_t value);

static ifaceControl_t s_canInterface = {
		{setSTBState, setENState, setWAKEState, getERRState},
		.sendData = sendData,
		.sendAirQuality = sendAirQuality
};

uint8_t BSP_init(void) {
	uint8_t result = true;
	SystemTimer_init();
	SystemStatus_setLedControl(Led_Red_SetState);
	SystemStatus_set(INFORM_INIT);
	initialize_RCC();
	initialize_GPIO_CAN();
	initialize_GPIO_LED();
	initialize_GPIO_ADC();
	configure_CAN_NVIC();
	configure_ADC();
	configure_ADC_NVIC();
	configure_TIM1();
	result &= configure_CAN();
	return result;
}

ifaceControl_p BSP_CANControl(void) {
	return &s_canInterface;
}

void Led_Red_SetState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(GPIOA, GPIO_Pin_0, val);
}

void Led_Green_SetState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, val);
}


static void initialize_RCC(void) {

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
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

	/* remap pins */
	SYSCFG->CFGR1 |= (uint32_t)SYSCFG_CFGR1_PA11_PA12_RMP;

	GPIO_Init(GPIOA, &iface);
	GPIO_Init(GPIOA, &ifaceControl);
	GPIO_Init(GPIOB, &ifaceFeedback);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_4);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_4);

	/* turn on transmitter */
	s_canInterface.hardwareLine.setSTB(DISABLE);
	s_canInterface.hardwareLine.setEN(ENABLE);
	s_canInterface.hardwareLine.setWAKE(DISABLE);
}

static void initialize_GPIO_LED(void) {

	GPIO_InitTypeDef iface = {0};
	iface.GPIO_Mode = GPIO_Mode_OUT;
	iface.GPIO_OType = GPIO_OType_PP;
	iface.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	iface.GPIO_PuPd = GPIO_PuPd_NOPULL;
	iface.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_Init(GPIOA, &iface);
}

static void initialize_GPIO_ADC(void) {

	GPIO_InitTypeDef iface = {0};
	iface.GPIO_Mode = GPIO_Mode_AN;
	iface.GPIO_OType = GPIO_OType_OD;
	iface.GPIO_Pin = GPIO_Pin_4;
	iface.GPIO_PuPd = GPIO_PuPd_NOPULL;
	iface.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_Init(GPIOA, &iface);
}

static void setSTBState(FunctionalState state) {
	/* low means StandBy */
	BitAction val = (state == DISABLE) ? Bit_SET : Bit_RESET;
	GPIO_WriteBit(GPIOA, GPIO_Pin_7, val);
}

static void setENState(FunctionalState state) {
	BitAction val = (state == DISABLE) ? Bit_RESET : Bit_SET;
	GPIO_WriteBit(GPIOA, GPIO_Pin_6, val);
}

static void setWAKEState(FunctionalState state) {
	/* low means WakeUp */
	BitAction val = (state == DISABLE) ? Bit_SET : Bit_RESET;
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, val);
}

static bool getERRState(void) {
	/* low means Error, or WakeUp - handled by interrupt */
	return (bool)!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
}

static uint8_t configure_CAN(void) {
	uint8_t initResult = 0;
	CAN_InitTypeDef iface = {0};
	CAN_FilterInitTypeDef  ifaceFilter ={0};
	const uint32_t baudRate = 125000;
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	iface.CAN_TTCM = DISABLE;
	iface.CAN_ABOM = DISABLE;
//	iface.CAN_AWUM = ENABLE;
	iface.CAN_AWUM = DISABLE;
	iface.CAN_NART = ENABLE;
	iface.CAN_RFLM = DISABLE;
	iface.CAN_TXFP = DISABLE;
	iface.CAN_Mode = CAN_Mode_Normal;
	iface.CAN_SJW = CAN_SJW_1tq;
	iface.CAN_BS1 = CAN_BS1_4tq;
	iface.CAN_BS2 = CAN_BS2_3tq;
	iface.CAN_Prescaler = RCC_Clocks.PCLK_Frequency/(baudRate*(1+4+3)); //(CAN_SJW + CAN_BS1 + CAN_BS2)

	CAN_DeInit(CAN);
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

	CAN_ITConfig(CAN, CAN_IT_EWG, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_EPV, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_BOF, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_LEC, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_ERR, ENABLE);

	return initResult;
}

static void configure_CAN_NVIC(void){
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = CEC_CAN_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void configure_ADC(void) {
	ADC_InitTypeDef iface;

	iface.ADC_ContinuousConvMode = DISABLE;
	iface.ADC_DataAlign = ADC_DataAlign_Right;
	iface.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_TRGO;
	iface.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Falling;//ADC_ExternalTrigConvEdge_None;
	iface.ADC_Resolution = ADC_Resolution_8b;
	iface.ADC_ScanDirection = ADC_ScanDirection_Upward;

	ADC_Init(ADC1, &iface);
	ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv4);
	ADC_ChannelConfig(ADC1, ADC_Channel_4, ADC_SampleTime_239_5Cycles);

	ADC_ContinuousModeCmd(ADC1, DISABLE);
	ADC_DiscModeCmd(ADC1, ENABLE);

	ADC_ITConfig(ADC1, ADC_IT_ADRDY, ENABLE);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	ADC_GetCalibrationFactor(ADC1);
	ADC_Cmd(ADC1, ENABLE);
}

static void configure_ADC_NVIC(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void configure_TIM1(void) {
	TIM_TimeBaseInitTypeDef iface;
	iface.TIM_ClockDivision = TIM_CKD_DIV4;
	iface.TIM_CounterMode = TIM_CounterMode_Up;
	iface.TIM_Period = 0xFF;
	iface.TIM_Prescaler = 470;
	iface.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM1, &iface);
	TIM_SetAutoreload(TIM1, iface.TIM_Period);
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);

	TIM_Cmd(TIM1, ENABLE);
}

static bool sendData(uint32_t id, uint8_t *data, uint8_t size) {
	CanTxMsg txMess = {
			id,
			id,
			CAN_Id_Standard,
			CAN_RTR_Data,
			size,
			{0}
	};
	if (size > 8 || id > 0x1FFFFFFF)
		return false;

	txMess.RTR = size ? CAN_RTR_Data : CAN_RTR_Remote;
	txMess.IDE = id > 0x7FF ? CAN_Id_Extended : CAN_Id_Standard;
	memcpy(txMess.Data, data, size);

	return CAN_Transmit(CAN, &txMess) != CAN_TxStatus_NoMailBox;
}

static bool sendAirQuality(uint8_t value) {
	const uint32_t id = 0x1DEADFAB;
	return sendData(id, &value, 1);
}
