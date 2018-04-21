/*
 * unistd.c
 *
 *  Created on: Apr 14, 2011
 *      Author: Ekawahyu Susilo
 */

#include "compiler.h"
#include "sleep.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_pwr.h"
#include "deca_port.h"


#define _clock(x) clock(x)

unsigned sleep(unsigned seconds)
{
	clock_t t0 = _clock();
	clock_t dt = seconds * CLOCKS_PER_SEC;

	while (_clock() - t0  < dt);
	return 0;
}

int usleep(useconds_t useconds)
{
	clock_t t0 = _clock();
	clock_t dt = useconds / (1000000/CLOCKS_PER_SEC);

	while (_clock() - t0  < dt);
	return 0;
}
