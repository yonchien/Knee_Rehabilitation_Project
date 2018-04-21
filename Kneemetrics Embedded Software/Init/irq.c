#include "irq.h"
#include "core_cm4.h"

/*
void InitializeBattChargerIRQ(void)
{
	//initialise ACOK pin

	GPIO_InitTypeDef ACOKIRQ_InitTypedef;
	ACOKIRQ_InitTypedef.GPIO_Mode = GPIO_Mode_IN;
	ACOKIRQ_InitTypedef.GPIO_PuPd = GPIO_PuPd_UP;
	ACOKIRQ_InitTypedef.GPIO_Speed = GPIO_Speed_50MHz;
	ACOKIRQ_InitTypedef.GPIO_Pin = 	ACOKn_Pin;

	RCC_AHB1PeriphClockCmd(ACOKn_GPIO_Clock, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(ACOKn_EXTI_Port_Source, ACOKn_EXTI_Pin);
	GPIO_Init(ACOKn_EXTI_Port, &ACOKIRQ_InitTypedef);

	//Now enable EXT1 on particular GPIO
	EXTI_InitTypeDef ACOKIRQ_Exti_Init;
	ACOKIRQ_Exti_Init.EXTI_Line = ACOKn_EXTI_Line;
	ACOKIRQ_Exti_Init.EXTI_Trigger = EXTI_Trigger_Falling;
	ACOKIRQ_Exti_Init.EXTI_Mode = EXTI_Mode_Interrupt;
	ACOKIRQ_Exti_Init.EXTI_LineCmd = ENABLE;

	EXTI_Init(&ACOKIRQ_Exti_Init);

	//Now enable NVIC for EXTI2
	NVIC_InitTypeDef ACOKIRQ_NVIC_Init;
	ACOKIRQ_NVIC_Init.NVIC_IRQChannel = ACOKn_EXTI_IRQn;
	ACOKIRQ_NVIC_Init.NVIC_IRQChannelPreemptionPriority = 0x0F;
	ACOKIRQ_NVIC_Init.NVIC_IRQChannelSubPriority = 0x00;
	ACOKIRQ_NVIC_Init.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&ACOKIRQ_NVIC_Init);

	//initialise CHGOK pin

	GPIO_InitTypeDef CHGOKIRQ_InitTypedef;
	CHGOKIRQ_InitTypedef.GPIO_Mode = GPIO_Mode_IN;
	CHGOKIRQ_InitTypedef.GPIO_PuPd = GPIO_PuPd_UP;
	CHGOKIRQ_InitTypedef.GPIO_Speed = GPIO_Speed_50MHz;
	CHGOKIRQ_InitTypedef.GPIO_Pin = CHGOKn_Pin;

	RCC_AHB1PeriphClockCmd(CHGOKn_GPIO_Clock, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(CHGOKn_EXTI_Port_Source, CHGOKn_EXTI_Pin);
	GPIO_Init(CHGOKn_EXTI_Port, &CHGOKIRQ_InitTypedef);

	//Now enable EXT1 on particular GPIO
	EXTI_InitTypeDef CHGOKIRQ_Exti_Init;
	CHGOKIRQ_Exti_Init.EXTI_Line = CHGOKn_EXTI_Line;
	CHGOKIRQ_Exti_Init.EXTI_Trigger = EXTI_Trigger_Falling;
	CHGOKIRQ_Exti_Init.EXTI_Mode = EXTI_Mode_Interrupt;
	CHGOKIRQ_Exti_Init.EXTI_LineCmd = ENABLE;

	EXTI_Init(&CHGOKIRQ_Exti_Init);

	//Now enable NVIC for EXTI8
	NVIC_InitTypeDef CHGOKIRQ_NVIC_Init;
	CHGOKIRQ_NVIC_Init.NVIC_IRQChannel = CHGOKn_EXTI_IRQn;
	CHGOKIRQ_NVIC_Init.NVIC_IRQChannelPreemptionPriority = 0x0F;
	CHGOKIRQ_NVIC_Init.NVIC_IRQChannelSubPriority = 0x00;
	CHGOKIRQ_NVIC_Init.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&CHGOKIRQ_NVIC_Init);
}

uint8_t GetACOKIRQLineState(void)
{
	return (GPIO_ReadInputDataBit(ACOKn_EXTI_Port, ACOKn_Pin));
}

uint8_t GetCHGOKIRQLineState(void)
{
	return (GPIO_ReadInputDataBit(CHGOKn_EXTI_Port, CHGOKn_Pin));
}

void EnableACOKIRQ(void)
{
	NVIC_EnableIRQ(ACOKn_EXTI_IRQn);
}

void EnableCHGOKIRQ(void)
{
	NVIC_EnableIRQ(CHGOKn_EXTI_IRQn);
}

void DisableACOKIRQ(void)
{
	NVIC_DisableIRQ(ACOKn_EXTI_IRQn);
}

void DisableCHGOKIRQ(void)
{
	NVIC_DisableIRQ(CHGOKn_EXTI_IRQn);
}

uint32_t GetACOKIRQ_Status(void)
{
	return NVIC_GetPendingIRQ(ACOKn_EXTI_IRQn);
}

uint32_t GetCHGOKIRQ_Status(void)
{
	return NVIC_GetPendingIRQ(CHGOKn_EXTI_IRQn);
}
*/
