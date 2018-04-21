/*Definition of Safesens LED*/

#ifndef _LEDS_H_
#define _LEDS_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"


typedef enum
{
	LED_RED = 0,
	LED_YEL,
	LED_GRE,
	LED_ALL
}LED_Type;

typedef enum
{
	LED_OFF,
	LED_ON,
	LED_NA
}LED_State;


void InitializeLEDS(void);
void SwitchOnLED(LED_Type);
void SwitchOffLED(LED_Type);
void SD_enable(void);
LED_State GetLedState(LED_Type);


#ifdef __cplusplus
}
#endif

#endif
