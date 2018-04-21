//#ifndef  _ENABLEANDRESET_H_
#define  _ENABLEANDRESET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#define DW1000_UWB_Reset_Pin		GPIO_Pin_2
#define DW1000_UWB_Reset_Port		GPIOG
#define DW1000_UWB_Reset_GPIO_RCC	RCC_AHB1Periph_GPIOG

void InitializeUWBReset(void);
