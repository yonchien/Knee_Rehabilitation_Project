#ifndef _SYS_CLOCK_H_
#define _SYS_CLOCK_H_


#ifdef __cplusplus
extern "C" {
#endif

#include<stdint.h>

void SysTick_Init(void);

unsigned long GetTickCount(void);

void Update_SysTickFlags(void);

uint8_t GetOneMilliSecTick(void);

uint8_t GetFiveMilliSecTick(void);

uint8_t GetTenMilliSecTick(void);

uint8_t GetOneSecondTick(void);

void DelayMilliSec(uint16_t milliSec);

void ResetOneSecondFlag();

void Delay(uint16_t sec);


#ifdef __cplusplus
}
#endif

#endif
