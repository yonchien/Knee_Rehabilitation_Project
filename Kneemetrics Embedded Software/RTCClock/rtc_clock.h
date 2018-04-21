#ifndef _RTC_CLOCK_H_
#define _RTC_CLOCK_H_


#ifdef __cplusplus
extern "C" {
#endif


#include<stdint.h>
#include "stm32f4xx_rtc.h"

#ifndef RTC_SYNC_PREDIV
#define RTC_SYNC_PREDIV					0x3FF
#endif
/* Async pre division for clock */
#ifndef RTC_ASYNC_PREDIV
#define RTC_ASYNC_PREDIV				0x1F
#endif
/* NVIC global Priority set */
#ifndef RTC_PRIORITY
#define RTC_PRIORITY					0x04
#endif
/* Sub priority for wakeup trigger */
#ifndef RTC_WAKEUP_SUBPRIORITY
#define RTC_WAKEUP_SUBPRIORITY			0x00
#endif
/* Sub priority for alarm trigger */
#ifndef RTC_ALARM_SUBPRIORITY
#define RTC_ALARM_SUBPRIORITY			0x01
#endif

typedef struct RTC_AppTimeTypedef
{
	uint8_t tm_sec;
	uint8_t tm_min;
	uint8_t tm_hour;
	uint8_t tm_mday;
	uint8_t tm_mon;
	uint8_t tm_year;
	uint8_t tm_wday;
	uint16_t tm_yday;
	uint8_t tm_isdst;
}RTC_AppTimeTypedef;

typedef struct RTC_UnixTimeValTypedef
{
	uint32_t tv_sec;
	uint32_t tv_usec;
}RTC_UnixTimeValTypedef;

typedef enum {
	TM_RTC_Result_Ok,   /*!< Everything OK */
	TM_RTC_Result_Error /*!< An error occurred */
} TM_RTC_Result_t;

typedef enum {
	TM_RTC_Int_Disable = 0x00, /*!< Disable RTC wakeup interrupts */
	TM_RTC_Int_60s,            /*!< RTC Wakeup interrupt every 60 seconds */
	TM_RTC_Int_30s,            /*!< RTC Wakeup interrupt every 30 seconds */
	TM_RTC_Int_15s,            /*!< RTC Wakeup interrupt every 15 seconds */
	TM_RTC_Int_10s,            /*!< RTC Wakeup interrupt every 10 seconds */
	TM_RTC_Int_5s,             /*!< RTC Wakeup interrupt every 5 seconds */
	TM_RTC_Int_2s,             /*!< RTC Wakeup interrupt every 2 seconds */
	TM_RTC_Int_1s,             /*!< RTC Wakeup interrupt every 1 seconds */
	TM_RTC_Int_500ms,          /*!< RTC Wakeup interrupt every 500 milliseconds */
	TM_RTC_Int_250ms,          /*!< RTC Wakeup interrupt every 250 milliseconds */
	TM_RTC_Int_125ms           /*!< RTC Wakeup interrupt every 125 milliseconds */
} TM_RTC_Int_t;

typedef enum {
	TM_RTC_ClockSource_Internal = 0x00, /*!< Use internal clock source for RTC (LSI oscillator) */
	TM_RTC_ClockSource_External         /*!< Use external clock source for RTC (LSE oscillator) */
} TM_RTC_ClockSource_t;

void RTCInit(void);
void RTCGetTimeOfDay(RTC_TimeTypeDef *);
void RTCGetDate(RTC_DateTypeDef *);
void RTCSetTimeOfDay(RTC_TimeTypeDef*);
void RTCSetDate(RTC_DateTypeDef*);
void RTCGetTimeOfDayAppTime(RTC_AppTimeTypedef*);
void RTCGetTimeOfDayUnit(RTC_UnixTimeValTypedef*);
void RTC_Interrupts(uint32_t int_val);


#ifdef __cplusplus
}
#endif


#endif
