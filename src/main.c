
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "diag/Trace.h"
#include "api.h"
#include "systemStatus.h"

int main(int argc, char* argv[]) {

	SystemStatus_set(BSP_init() ? INFORM_IDLE : INFORM_ERROR);

	while (true) {

    }
	return 0;
}
