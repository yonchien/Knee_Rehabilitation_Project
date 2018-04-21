#include "sys_clock.h"
#include "misc.h"
#include "stm32f4xx_rcc.h"
#include "compiler.h"

#define FIVE_MILLISEC_COUNT 	5
#define TEN_MILLISEC_COUNT 		10
#define ONE_MILLISEC_COUNT 		1
#define ONE_SECOND_COUNT 		1000

extern __IO unsigned long time32_incr;
static uint8_t sysTickFlag = 0;
static uint8_t oneMilliSecFlag = 0;
static uint8_t fiveMilliSecFlag = 0;
static uint8_t tenMilliSecFlag = 0;
static uint8_t oneSecondFlag = 0;

#if 0
static uint8_t fiveMilliSecCount = 0;
static uint8_t tenMilliSecCount = 0;
static uint16_t oneMilliSecCount = 0;
#endif

static uint16_t oneSecondCount = 0;

void SysTick_Init(void)
{
	RCC_ClocksTypeDef rccTypedef;
	RCC_GetClocksFreq(&rccTypedef);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(rccTypedef.HCLK_Frequency/1000);

	NVIC_SetPriority (SysTick_IRQn, 5);
}

void Update_SysTickFlags(void)
{
	sysTickFlag = 1;
	oneSecondCount++;
	if(oneSecondCount == ONE_SECOND_COUNT)
	{
		oneSecondCount = 0;
		oneSecondFlag = 1;
	}
}

uint8_t GetOneMilliSecTick(void)
{
	return oneMilliSecFlag;
}

uint8_t GetFiveMilliSecTick(void)
{
	return fiveMilliSecFlag;
}


uint8_t GetTenMilliSecTick(void)
{
	return tenMilliSecFlag;
}

uint8_t GetOneSecondTick(void)
{
	return oneSecondFlag;
}

void ResetOneSecondFlag(void)
{
	oneSecondFlag = 0;
}

//Here Sleep is define in complier.h is millisec sleep

void DelayMilliSec(uint16_t milliSec)
{
#if 0
	while(milliSec > 0)
	{
		if(oneMilliSecFlag)
		{
			milliSec--;
		}
	}
#endif
	Sleep(milliSec);
}

void Delay(uint16_t sec)
{
#if 0
	while(sec > 0)
	{
		if(oneSecondFlag)
		{
			sec--;
			oneSecondFlag = 0;
		}
	}
#endif
	Sleep(sec*1000);
}

unsigned long GetTickCount(void)
{
	return time32_incr;
}
