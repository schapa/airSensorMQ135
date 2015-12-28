/*
 * api.h
 *
 *  Created on: Dec 28, 2015
 *      Author: shapa
 */

#ifndef API_H_
#define API_H_

#include "stm32f0xx.h"

typedef void (*control_f) (FunctionalState);
typedef FunctionalState (*getState_f) (void);
typedef struct {
	struct {
		control_f setSTB;
		control_f setEN;
		control_f setWAKE;
		getState_f getERR;
	} hardwareLine;
} ifaceControl_t, *ifaceControl_p;


void BSP_init(void);
ifaceControl_p BSP_CANControl(void);


#endif /* API_H_ */
