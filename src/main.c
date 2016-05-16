
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "diag/Trace.h"
#include "api.h"
#include "systemStatus.h"
#include "circleBuffer.h"

#include "stm32f0xx_can.h"


int main(int argc, char* argv[]) {

	(void)argc;
	(void)argv;

	BSP_init();

	AirQuality_startup();

	while (true) {
		Event_t event;
		BSP_pendEvent(&event);
		AirQuality_periodic();
//		SystemTimer_delayMsDummy(100);
	}
	return 0;
}

