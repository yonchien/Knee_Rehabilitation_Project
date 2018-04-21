/*
 * Interrupt initialisation file
 */

#ifndef _IRQ_H_
#define _IRQ_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"

//ACOK pin
//#define ACOKn_Pin             	    GPIO_Pin_2
//#define ACOKn_GPIO_Port           	GPIOI
//#define ACOKn_GPIO_Clock 			RCC_AHB1Periph_GPIOI
//
//#define ACOKn_EXTI_Line           	EXTI_Line2
//#define ACOKn_EXTI_Port_Source    	EXTI_PortSourceGPIOI
//#define ACOKn_EXTI_Port				GPIOI
//#define ACOKn_EXTI_Pin         		GPIO_PinSource2
//
//#define ACOKn_EXTI_IRQn          	EXTI2_IRQn
//#define ACOKn_EXTI_USEIRQ        	ENABLE
//#define ACOKn_EXTI_NOIRQ         	DISABLE
//
////CHGOK pin
//#define CHGOKn_Pin             	    GPIO_Pin_8
//#define CHGOKn_GPIO_Port           	GPIOA
//#define CHGOKn_GPIO_Clock 			RCC_AHB1Periph_GPIOA
//
//#define CHGOKn_EXTI_Line           	EXTI_Line8
//#define CHGOKn_EXTI_Port_Source    	EXTI_PortSourceGPIOA
//#define CHGOKn_EXTI_Port			GPIOA
//#define CHGOKn_EXTI_Pin         	GPIO_PinSource8
//
//#define CHGOKn_EXTI_IRQn          	EXTI9_5_IRQn
//#define CHGOKn_EXTI_USEIRQ        	ENABLE
//#define CHGOKn_EXTI_NOIRQ         	DISABLE
//
//
//void InitializeBattChargerIRQ(void);
//uint8_t GetACOKIRQLineState(void);
//uint8_t GetCHGOKIRQLineState(void);
//void EnableACOKIRQ(void);
//void EnableCHGOKIRQ(void);
//void DisableACOKIRQ(void);
//void DisableCHGOKIRQ(void);
//uint32_t GetACOKIRQ_Status(void);
//uint32_t GetCHGOKIRQ_Status(void);


#ifdef __cplusplus
}
#endif


#endif
