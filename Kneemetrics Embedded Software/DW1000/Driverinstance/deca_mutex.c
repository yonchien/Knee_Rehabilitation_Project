#include "deca_device_api.h"
#include "deca_port.h"

decaIrqStatus_t decamutexon(void)
{
	decaIrqStatus_t s = port_GetEXT_IRQStatus();

	if(s) {
		port_DisableEXT_IRQ(); //disable the external interrupt line
	}
	return s ;   // return state before disable, value is used to re-enable in decamutexoff call
}

void decamutexoff(decaIrqStatus_t s)
{
	if(s) { //need to check the port state as we can't use level sensitive interrupt on the STM ARM
		port_EnableEXT_IRQ();
	}
}
