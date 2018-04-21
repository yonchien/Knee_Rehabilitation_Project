#include "stm32f4xx.h"
#include "delay.h"
#include "string.h"

#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_rcc.h>
#include <misc.h>
//*** delays lower than 50us won't be precise***

void delay_us (uint32_t delay){

	uint32_t temp = SystemCoreClock/1000000;

	uint32_t count = (delay*temp)/10;

	for (;count>0;count--){}
}

void delay_ms (uint32_t delay_ms){

	uint32_t delay_us = delay_ms * 1000;

	uint32_t temp = SystemCoreClock/1000000;

	uint32_t count = (delay_us*temp)/10;

	for (;count>0;count--){}
}
void InitializeTimer()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
   TIM_TimeBaseInitTypeDef TIM_InitStruct;
//    timerInitStructure.TIM_Prescaler = 0;
//    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
//    timerInitStructure.TIM_Period = 65535;
//    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//    timerInitStructure.TIM_RepetitionCounter = 0;
//    TIM_TimeBaseInit(TIM2, &timerInitStructure);
//    TIM_Cmd(TIM2, ENABLE);


	TIM_InitStruct.TIM_Prescaler =  84000;//((SystemCoreClock / 10000000) / 2) - 1;
	  TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	  TIM_InitStruct.TIM_Period = 0xFFFF;
	  TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	  TIM_InitStruct.TIM_RepetitionCounter = 0;
	  TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
	  TIM_Cmd(TIM2, ENABLE);

}
