#include "rtc_clock.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rtc.h"
#include <math.h>
#include <stdio.h>
#include "misc.h"
#include "irq.h"

void RTCInit(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	/*RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);
*/
	/* Enable the LSE OSC */
	//RCC_LSEConfig(RCC_LSE_ON);

	/* Enable the LSI OSC */
	//RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */
	//while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	/* Wait till LSE is ready */
	//while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

	/* Select the RTC Clock Source */
	//RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

	/* Select the RTC Clock Source */
	//RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div8);
	RCC_RTCCLKCmd(ENABLE);

	RTC_WaitForSynchro();

	RCC_RTCCLKCmd(ENABLE);

	RTC_WriteProtectionCmd(ENABLE);

	// Refer application notes DM00025071.pdf (ST Micro. RTC App notes page 9/45)
	RTC_InitTypeDef rtcInit;
	rtcInit.RTC_HourFormat = RTC_HourFormat_24;
	rtcInit.RTC_SynchPrediv = 7999;
	rtcInit.RTC_AsynchPrediv = 124;
	RTC_Init(&rtcInit);

	RTC_TimeTypeDef currTime;
	currTime.RTC_Hours = 12;
	currTime.RTC_Minutes = 35;
	currTime.RTC_Seconds = 35;
	currTime.RTC_H12 = RTC_H12_PM;
	RTC_SetTime(RTC_Format_BIN, &currTime);

	RTC_DateTypeDef currDate;
	currDate.RTC_WeekDay = RTC_Weekday_Thursday;
	currDate.RTC_Date = 5;
	currDate.RTC_Month = RTC_Month_November;
	currDate.RTC_Year = 15;
	RTC_SetDate(RTC_Format_BIN, &currDate);

	RTC_ExitInitMode();
	RTC_WriteProtectionCmd(DISABLE);
}

void RTCGetTimeOfDay(RTC_TimeTypeDef* currTime)
{
	 RTC_GetTime(RTC_Format_BIN, currTime);
}

void RTCGetDate(RTC_DateTypeDef* currDate)
{
	 RTC_GetDate(RTC_Format_BIN, currDate);
}

void RTCSetTimeOfDay(RTC_TimeTypeDef* setTime)
{
	RTC_WriteProtectionCmd(ENABLE);
	RTC_EnterInitMode();
	RTC_SetTime(RTC_Format_BIN, setTime);
	RTC_ExitInitMode();
	RTC_WriteProtectionCmd(DISABLE);
}


void RTCSetDate(RTC_DateTypeDef* setDate)
{
	RTC_WriteProtectionCmd(ENABLE);
	RTC_EnterInitMode();
	RTC_SetDate(RTC_Format_BIN, setDate);
	RTC_ExitInitMode();
	RTC_WriteProtectionCmd(DISABLE);
}

void RTCGetTimeOfDayAppTime(RTC_AppTimeTypedef* appTime)
{

}

void RTCGetTimeOfDayUnit(RTC_UnixTimeValTypedef* appUnixTime)
{

}

void RTC_Interrupts(uint32_t int_val) {
	if(int_val==0)
		return;
	double set;
	uint32_t set_floor;

	/* Clear pending bit */
	EXTI->PR = 0x00400000;

	/* Disable wakeup interrupt */
	RTC_WakeUpCmd(DISABLE);

	/* Disable RTC interrupt flag */
	RTC_ITConfig(RTC_IT_WUT, DISABLE);

	/* NVIC init for RTC */
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = RTC_PRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = RTC_WAKEUP_SUBPRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStruct);

	/* RTC connected to EXTI_Line22 */
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line22;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStruct);

	/* Enable NVIC */
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	/* Enable EXT1 interrupt */
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	/* First disable wake up command */
	RTC_WakeUpCmd(DISABLE);

	/* Clock divided by 8, 32768 / 2 = 16384 */
	/* 16384 ticks for 1second interrupt */
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);

	set= int_val*16.384;
	set_floor= (int) round(set);

	/* Set RTC wakeup counter */
	RTC_SetWakeUpCounter(set_floor);
	/* Enable wakeup interrupt */
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	/* Enable wakeup command */
	RTC_WakeUpCmd(ENABLE);

}
