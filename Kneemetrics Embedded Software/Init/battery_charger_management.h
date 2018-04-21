/*Definition of Battery charger M0 and M1 pins for handling of input and charge current limit
 *
 * !!!*****ATTENTION*****!!!: consult datasheet for perfect current limit settings! */

#ifndef _BATTERYCHARGERMANAGEMENT_H_
#define _BATTERYCHARGERMANAGEMENT_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

typedef enum
{
	M0 = 0,
	M1,
	BOTH_M
}Mx_Type;

typedef enum
{
	Mx_High,
	Mx_Low,
	Mx_NA
}Mx_State;

void InitializeM0M1(void);
void SetMxHigh(Mx_Type);
void SetMxLow(Mx_Type);
Mx_State GetMxState(Mx_Type);

#ifdef __cplusplus
}
#endif

#endif
