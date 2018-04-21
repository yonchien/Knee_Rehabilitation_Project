#ifndef _DECA_PORT_H_
#define _DECA_PORT_H_


#ifdef __cplusplus
extern "C"{
#endif


#include "spi.h"
#include "sys_clock.h"
#include "leds.h"
#include "deca_irq.h"
#include "deca_instance_init.h"

#define port_SPIx_clear_chip_select(x)		SetDW1000ChipSelLow(x)
#define port_SPIx_set_chip_select(x)		SetDW1000ChipSelHigh(x)

#define portGetTickCount()           		GetTickCount()

#define LED_PC6 							LED_RED
#define LED_PC7 							LED_GRE
#define LED_PC8 							LED_YEL
#define LED_PC9 							(0)  //LED4

#define led_on(x) 							SwitchOnLED(x)
#define led_off(x) 							SwitchOffLED(x)

#define TA_SW1_1							(1)
#define TA_SW1_2							(0)
#define TA_SW1_3                    		(1)
#define TA_SW1_4                    		(0) //0=tag, 1=anchor
#define TA_SW1_5                    		(0)
#define TA_SW1_6                    		(1)
#define TA_SW1_7                    		(0)
#define TA_SW1_8                    		(0)
#define TA_SW1_GPIO                 		(0)
#define TR_SW2_5							(1)
#define TR_SW2_6							(1)
#define TR_SW2_7	                  		(1)
#define TR_SW2_8	                  		(1)


#define is_switch_on(x)						Sim_SW1_Switch(x)
#define is_fastrng_on()						Sim_SW1_Switch(TA_SW1_2)

#define S1_SWITCH_ON  						(1)
#define S1_SWITCH_OFF 						(0)

#define port_IS_TAG_pressed()       		is_switch_on(TA_SW1_4)


#define port_DisableEXT_IRQ()               DisableDECAIRQ()
#define port_EnableEXT_IRQ()               	EnableDECAIRQ()
#define port_CheckEXT_IRQ()					GetDECAIRQLineState()
#define port_GetEXT_IRQStatus()				GetDECAIRQ_Status()

#define port_Disable_USB_IRQ()				NVIC_DisableIRQ(OTG_FS_IRQn);
#define port_Enable_USB_IRQ()				NVIC_EnableIRQ(OTG_FS_IRQn);


#define ENABLE_PRINTF						(0)


#ifdef __cplusplus
}
#endif


#endif
