#ifndef _DECA_IRQ_H_
#define _DECA_IRQ_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"

#define DECAIRQ_Pin                 GPIO_Pin_1 //(working)
#define DECAIRQ_GPIO_Port           GPIOI
#define DECAIRQ_GPIO_Clock 			RCC_AHB1Periph_GPIOI


#define DECAIRQ_EXTI_Line           EXTI_Line1
#define DECAIRQ_EXTI_Port_Source    EXTI_PortSourceGPIOI
#define DECAIRQ_EXTI_Port			GPIOI
#define DECAIRQ_EXTI_Pin            GPIO_PinSource1 //GPIO_PinSource3

#define DECAIRQ_EXTI_IRQn           EXTI1_IRQn
#define DECAIRQ_EXTI_USEIRQ         ENABLE
#define DECAIRQ_EXTI_NOIRQ          DISABLE


void InitializeDecaIRQ(void);
uint8_t GetDECAIRQLineState(void);
void EnableDECAIRQ(void);
void DisableDECAIRQ(void);
uint32_t GetDECAIRQ_Status(void);

uint8_t Sim_SW1_Switch(uint8_t sw);


#ifdef __cplusplus
}
#endif


#endif
