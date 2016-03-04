/*
 * adcWrapper.c
 *
 *  Created on: Mar 4, 2016
 *      Author: shapa
 */


#include "api.h"
#include "stm32f0xx_adc.h"
#include "cortexm/ExceptionHandlers.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

static bool handleAdcITFlag(uint32_t flag);

void ADC1_IRQHandler(void) {

	static const uint32_t adcITFlags[] = {
			ADC_IT_ADRDY,
			ADC_IT_EOSMP,
			ADC_IT_EOC,
			ADC_IT_EOSEQ,
			ADC_IT_OVR,
			ADC_IT_AWD,
	};
	static const size_t adcITFlagsSize = sizeof(adcITFlags)/(sizeof(*adcITFlags));
	size_t i;

	for (i = 0; i < adcITFlagsSize; i++) {
		if (ADC_GetITStatus(ADC1, adcITFlags[i]) && handleAdcITFlag(adcITFlags[i])) {
			ADC_ClearITPendingBit(ADC1, adcITFlags[i]);
		}
	}
}

static bool handleAdcITFlag(uint32_t flag) {
	bool clear = true;
	switch (flag) {
		case ADC_IT_ADRDY: {
			ADC_StartOfConversion(ADC1);
		}break;
		case ADC_IT_EOSMP: {
		}break;
		case ADC_IT_EOC: {
			uint32_t converted = ADC_GetConversionValue(ADC1);
			converted = converted*2844/1000;
			ADC_StartOfConversion(ADC1);
		}break;
		case ADC_IT_EOSEQ: {
		}break;
		case ADC_IT_OVR: {
		}break;
		case ADC_IT_AWD: {
		}break;
		default: {
			clear = false;
		} break;
	}
	return clear;
}
