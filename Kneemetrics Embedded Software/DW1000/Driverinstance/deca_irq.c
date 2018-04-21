#include "deca_irq.h"
#include "core_cm4.h"

void InitializeDecaIRQ(void)
{
	GPIO_InitTypeDef DecaIRQ_InitTypedef;
	DecaIRQ_InitTypedef.GPIO_Mode = GPIO_Mode_IN;
	DecaIRQ_InitTypedef.GPIO_OType = GPIO_OType_PP;
	DecaIRQ_InitTypedef.GPIO_PuPd = GPIO_PuPd_DOWN;
	DecaIRQ_InitTypedef.GPIO_Speed = GPIO_Speed_50MHz;
	DecaIRQ_InitTypedef.GPIO_Pin = 	DECAIRQ_Pin;			//GPIO_Pin_0; (Working)

	RCC_AHB1PeriphClockCmd(DECAIRQ_GPIO_Clock, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(DECAIRQ_EXTI_Port_Source, DECAIRQ_EXTI_Pin);
	GPIO_Init(DECAIRQ_GPIO_Port, &DecaIRQ_InitTypedef);

	//Now enable EXT1 on particular GPIO
	EXTI_InitTypeDef DecaIRQ_Exti_Init;
	DecaIRQ_Exti_Init.EXTI_Line = DECAIRQ_EXTI_Line;
	DecaIRQ_Exti_Init.EXTI_Trigger = EXTI_Trigger_Rising;
	DecaIRQ_Exti_Init.EXTI_Mode = EXTI_Mode_Interrupt;
	DecaIRQ_Exti_Init.EXTI_LineCmd = ENABLE;

	EXTI_Init(&DecaIRQ_Exti_Init);

	//Now enable NVIC for EXTI0
	NVIC_InitTypeDef DecaIRQ_NVIC_Init;
	DecaIRQ_NVIC_Init.NVIC_IRQChannel = DECAIRQ_EXTI_IRQn;
	DecaIRQ_NVIC_Init.NVIC_IRQChannelPreemptionPriority = 0x00;
	DecaIRQ_NVIC_Init.NVIC_IRQChannelSubPriority = 0x00;
	DecaIRQ_NVIC_Init.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&DecaIRQ_NVIC_Init);
}

uint8_t GetDECAIRQLineState(void)
{
	return (GPIO_ReadInputDataBit(DECAIRQ_EXTI_Port, DECAIRQ_Pin));
}

void EnableDECAIRQ(void)
{
	NVIC_EnableIRQ(DECAIRQ_EXTI_IRQn);
}

void DisableDECAIRQ(void)
{
	NVIC_DisableIRQ(DECAIRQ_EXTI_IRQn);
}

uint8_t Sim_SW1_Switch(uint8_t sw) {
	return sw;
}

uint32_t GetDECAIRQ_Status(void)
{
	return NVIC_GetPendingIRQ(DECAIRQ_EXTI_IRQn);
}


