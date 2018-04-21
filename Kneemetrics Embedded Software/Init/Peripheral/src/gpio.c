/*******************************************************************************
File    : Gpio.c
Purpose : 
********************************** Includes ***********************************/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h" 
#include "misc.h" 
#include "stm32f4xx_exti.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_syscfg.h"

#include "GPIO.h"

/********************************* Defines ************************************/

//changed PIN A0 to PIN A1 back, to match low power modes 

#define INVEN_INT_PIN                         GPIO_Pin_2
#define INVEN_INT_GPIO_PORT                   GPIOA
#define INVEN_INT_GPIO_CLK                    RCC_AHB1Periph_GPIOA
#define INVEN_INT_EXTI_PORT                   EXTI_PortSourceGPIOA
#define INVEN_INT_EXTI_PIN                    EXTI_PinSource2
#define INVEN_INT_EXTI_LINE                   EXTI_Line2
#define INVEN_INT_EXTI_IRQ                    EXTI2_IRQn

/**********************************Globals ************************************/
/********************************* Prototypes *********************************/

/***********************************Functions *********************************/
void GPIO_Config(void)
{

  /*GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;

//  GPIO_DeInit(GPIOA);
//  GPIO_DeInit(GPIOC);
  
   Enable GPIOB clock
  RCC_AHB1PeriphClockCmd(INVEN_INT_GPIO_CLK, ENABLE);
   Enable SYSCFG clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
   Configure invensense sensor interrupt pin as input floating
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //NOpull
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100
  GPIO_InitStructure.GPIO_Pin = INVEN_INT_PIN;
  GPIO_Init(INVEN_INT_GPIO_PORT, &GPIO_InitStructure); //GPIOA

   Connect EXTI Line to inv sensor interrupt pin
  SYSCFG_EXTILineConfig(INVEN_INT_EXTI_PORT, INVEN_INT_EXTI_PIN);

   Configure EXTI Line1
  EXTI_InitStructure.EXTI_Line = INVEN_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

   Enable and set EXTI Line Interrupt to the highest priority
  NVIC_InitStructure.NVIC_IRQChannel = INVEN_INT_EXTI_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);*/
	 /* Set variables used */
	    GPIO_InitTypeDef GPIO_InitStruct;
	    EXTI_InitTypeDef EXTI_InitStruct;
	    NVIC_InitTypeDef NVIC_InitStruct;

	    /* Enable clock for GPIOD */
	    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
	    /* Enable clock for SYSCFG */
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	    /* Set pin as input */
	    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	    GPIO_Init(GPIOD, &GPIO_InitStruct);

	    /* Tell system that you will use PD0 for EXTI_Line0 */
	    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOH, EXTI_PinSource2);

	    /* PD0 is connected to EXTI_Line0 */
	    EXTI_InitStruct.EXTI_Line = EXTI_Line2;
	    /* Enable interrupt */
	    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	    /* Interrupt mode */
	    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	    /* Triggers on rising and falling edge */
	    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	    /* Add to EXTI */
	    EXTI_Init(&EXTI_InitStruct);

	    /* Add IRQ vector to NVIC */
	    /* PD0 is connected to EXTI_Line0, which has EXTI0_IRQn vector */
	    NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;
	    /* Set priority */
	    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	    /* Set sub priority */
	    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	    /* Enable interrupt */
	    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	    /* Add to NVIC */
	    NVIC_Init(&NVIC_InitStruct);

}

void EnableInvInterrupt(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  /* Configure EXTI Line1 */
  EXTI_InitStructure.EXTI_Line = INVEN_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}
void DisableInvInterrupt(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
    /* Configure EXTI Line1 */
  EXTI_InitStructure.EXTI_Line = INVEN_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);
  EXTI_ClearITPendingBit(INVEN_INT_EXTI_LINE);
}

void InvIntHandler(void)
{

  /* Clear the EXTI line 1 pending bit */
   EXTI_ClearITPendingBit(INVEN_INT_EXTI_LINE);

}
