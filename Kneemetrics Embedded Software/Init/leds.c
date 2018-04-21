#include "leds.h"
#include "stm32f4xx.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"
#include "ff.h"
#include "diskio.h"
#include "compiler.h"

FRESULT res;
FIL fil;

//#define LED1
#define LEDR_GPIO_Pin 		GPIO_Pin_14
#define LEDR_GPIO_Port 		GPIOE
#define LEDR_GPIO_Clock 	RCC_AHB1Periph_GPIOE

//#define LED2
#define LEDG_GPIO_Pin 		GPIO_Pin_12
#define LEDG_GPIO_Port 		GPIOD
#define LEDG_GPIO_Clock 	RCC_AHB1Periph_GPIOD

//#define LED3
#define LEDY_GPIO_Pin 		GPIO_Pin_10
#define LEDY_GPIO_Port 		GPIOH
#define LEDY_GPIO_Clock 	RCC_AHB1Periph_GPIOH

//define SD Enable
#define SDEN_GPIO_Pin 		GPIO_Pin_10
#define SDEN_GPIO_Port 		GPIOB
#define SDEN_GPIO_Clock 	RCC_AHB1Periph_GPIOB

//define BME Enable pin
#define BME_GPIO_Pin 		GPIO_Pin_3
#define BME_GPIO_Port 		GPIOA
#define BME_GPIO_Clock 	RCC_AHB1Periph_GPIOA

//define BLE Enable pin
#define BLE_GPIO_Pin 		GPIO_Pin_0
#define BLE_GPIO_Port 		GPIOE
#define BLE_GPIO_Clock  	RCC_AHB1Periph_GPIOE

static LED_State ledr_state;
static LED_State ledg_state;
static LED_State ledy_state;


void InitializeLEDS(void)
{
	GPIO_InitTypeDef LEDR_InitTypedef;

	LEDR_InitTypedef.GPIO_Mode = GPIO_Mode_OUT;
	LEDR_InitTypedef.GPIO_OType = GPIO_OType_PP;
	LEDR_InitTypedef.GPIO_PuPd = GPIO_PuPd_DOWN;
	LEDR_InitTypedef.GPIO_Speed = GPIO_Speed_50MHz;
	LEDR_InitTypedef.GPIO_Pin = LEDR_GPIO_Pin;

	RCC_AHB1PeriphClockCmd(LEDR_GPIO_Clock, ENABLE);
	
	GPIO_InitTypeDef LEDG_InitTypedef;

		LEDG_InitTypedef.GPIO_Mode = GPIO_Mode_OUT;
		LEDG_InitTypedef.GPIO_OType = GPIO_OType_PP;
		LEDG_InitTypedef.GPIO_PuPd = GPIO_PuPd_DOWN;
		LEDG_InitTypedef.GPIO_Speed = GPIO_Speed_50MHz;
		LEDG_InitTypedef.GPIO_Pin = LEDG_GPIO_Pin;

	RCC_AHB1PeriphClockCmd(LEDG_GPIO_Clock, ENABLE);
	
	GPIO_InitTypeDef LEDY_InitTypedef;

		LEDY_InitTypedef.GPIO_Mode = GPIO_Mode_OUT;
		LEDY_InitTypedef.GPIO_OType = GPIO_OType_PP;
		LEDY_InitTypedef.GPIO_PuPd = GPIO_PuPd_DOWN;
		LEDY_InitTypedef.GPIO_Speed = GPIO_Speed_50MHz;
		LEDY_InitTypedef.GPIO_Pin = LEDY_GPIO_Pin;

	RCC_AHB1PeriphClockCmd(LEDY_GPIO_Clock, ENABLE);
	
	GPIO_InitTypeDef SDEN_InitTypedef;

		SDEN_InitTypedef.GPIO_Mode = GPIO_Mode_OUT;
		SDEN_InitTypedef.GPIO_OType = GPIO_OType_PP;
		SDEN_InitTypedef.GPIO_PuPd = GPIO_PuPd_DOWN;
		SDEN_InitTypedef.GPIO_Speed = GPIO_Speed_50MHz;
		SDEN_InitTypedef.GPIO_Pin = SDEN_GPIO_Pin;

		RCC_AHB1PeriphClockCmd(SDEN_GPIO_Clock, ENABLE);

		GPIO_InitTypeDef BME_InitTypedef;

						BME_InitTypedef.GPIO_Mode = GPIO_Mode_OUT;
						BME_InitTypedef.GPIO_OType = GPIO_OType_PP;
						BME_InitTypedef.GPIO_PuPd = GPIO_PuPd_NOPULL;
						BME_InitTypedef.GPIO_Speed = GPIO_Speed_50MHz;
						BME_InitTypedef.GPIO_Pin = BME_GPIO_Pin;

						RCC_AHB1PeriphClockCmd(BME_GPIO_Clock, ENABLE);
						GPIO_InitTypeDef BLEreset_InitTypedef;

													BLEreset_InitTypedef.GPIO_Mode = GPIO_Mode_OUT;
													BLEreset_InitTypedef.GPIO_OType = GPIO_OType_PP;
													BLEreset_InitTypedef.GPIO_PuPd = GPIO_PuPd_NOPULL;
													BLEreset_InitTypedef.GPIO_Speed = GPIO_Speed_50MHz;
													BLEreset_InitTypedef.GPIO_Pin = GPIO_Pin_4;

													RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

													GPIO_InitTypeDef BLEen_InitTypedef;

															BLEen_InitTypedef.GPIO_Mode = GPIO_Mode_OUT;
															BLEen_InitTypedef.GPIO_OType = GPIO_OType_PP;
															BLEen_InitTypedef.GPIO_PuPd = GPIO_PuPd_DOWN;
															BLEen_InitTypedef.GPIO_Speed = GPIO_Speed_50MHz;
															BLEen_InitTypedef.GPIO_Pin = GPIO_Pin_0;

															RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);





	GPIO_Init(LEDR_GPIO_Port, &LEDR_InitTypedef);
	GPIO_Init(LEDG_GPIO_Port, &LEDG_InitTypedef);
	GPIO_Init(LEDY_GPIO_Port, &LEDY_InitTypedef);
	GPIO_Init(SDEN_GPIO_Port, &SDEN_InitTypedef);
	GPIO_Init(BME_GPIO_Port, &BME_InitTypedef);
	GPIO_Init(GPIOE, &BLEreset_InitTypedef);
	GPIO_Init(GPIOE, &BLEen_InitTypedef);

}

void BLE_enable()
{
	GPIO_WriteBit(BLE_GPIO_Port, BLE_GPIO_Pin, Bit_SET);
}

void BLE_disable()
{
	GPIO_WriteBit(BLE_GPIO_Port, BLE_GPIO_Pin, Bit_RESET);
}

void SD_enable()
{
	GPIO_WriteBit(SDEN_GPIO_Port, SDEN_GPIO_Pin, Bit_SET);
}

void SD_disable()
{
	GPIO_WriteBit(SDEN_GPIO_Port, SDEN_GPIO_Pin, Bit_RESET);
}

void BME_enable()
{
	GPIO_WriteBit(BME_GPIO_Port, BME_GPIO_Pin, Bit_SET);
}
void BME_disable()
{
	GPIO_WriteBit(BME_GPIO_Port, BME_GPIO_Pin, Bit_RESET);
}

void SwitchOnLED(LED_Type led)
{
	switch(led)
	{
	case LED_RED:
		GPIO_WriteBit(LEDR_GPIO_Port, LEDR_GPIO_Pin, Bit_SET);
		ledr_state = LED_ON;
		break;
	case LED_GRE:
		GPIO_WriteBit(LEDG_GPIO_Port, LEDG_GPIO_Pin, Bit_SET);
		ledg_state = LED_ON;
		break;
	case LED_YEL:
		GPIO_WriteBit(LEDY_GPIO_Port, LEDY_GPIO_Pin, Bit_SET);
		ledy_state = LED_ON;
		break;

	case LED_ALL:
		GPIO_WriteBit(LEDR_GPIO_Port, LEDR_GPIO_Pin, Bit_SET);
		GPIO_WriteBit(LEDG_GPIO_Port, LEDG_GPIO_Pin, Bit_SET);
		GPIO_WriteBit(LEDY_GPIO_Port, LEDY_GPIO_Pin, Bit_SET);
		ledr_state = LED_ON;
		ledg_state = LED_ON;
		ledy_state = LED_ON;
		
	default:
		break;	
	}
}

void SwitchOffLED(LED_Type led)
{	
	switch(led)
	{
	case LED_RED:
			GPIO_WriteBit(LEDR_GPIO_Port, LEDR_GPIO_Pin, Bit_RESET);
			ledr_state = LED_OFF;
			break;
		case LED_GRE:
			GPIO_WriteBit(LEDG_GPIO_Port, LEDG_GPIO_Pin, Bit_RESET);
			ledg_state = LED_OFF;
			break;
		case LED_YEL:
			GPIO_WriteBit(LEDY_GPIO_Port, LEDY_GPIO_Pin, Bit_RESET);
			ledy_state = LED_OFF;
			break;

		case LED_ALL:
			GPIO_WriteBit(LEDR_GPIO_Port, LEDR_GPIO_Pin, Bit_RESET);
			GPIO_WriteBit(LEDG_GPIO_Port, LEDG_GPIO_Pin, Bit_RESET);
			GPIO_WriteBit(LEDY_GPIO_Port, LEDY_GPIO_Pin, Bit_RESET);
			ledr_state = LED_OFF;
			ledg_state = LED_OFF;
			ledy_state = LED_OFF;

		default:
			break;
	}
}

LED_State GetLedState(LED_Type led)
{
	switch(led)
	{
	case LED_RED:
		return ledr_state;

	case LED_GRE:
		return ledg_state;

	case LED_YEL:
		return ledy_state;

	default:
		return LED_NA;
	}
}
void Initial(void)
{

GPIO_InitTypeDef LED_InitTypedef;

	LED_InitTypedef.GPIO_Mode = GPIO_Mode_OUT;
	LED_InitTypedef.GPIO_OType = GPIO_OType_PP;
	LED_InitTypedef.GPIO_PuPd = GPIO_PuPd_DOWN;
	LED_InitTypedef.GPIO_Speed = GPIO_Speed_50MHz;
	LED_InitTypedef.GPIO_Pin = GPIO_Pin_6;

	RCC_AHB1PeriphClockCmd(GPIOB, ENABLE);
	GPIO_Init(GPIOB, &LED_InitTypedef);

	GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
}




void Button_Inter_init(void) {
    /* Set variables used */
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* Enable clock for GPIOB */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Set pin as input */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* Tell system that you will use PB12 for EXTI_Line12 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource15);

    /* PB12 is connected to EXTI_Line12 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line15;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);

    /* Add IRQ vector to NVIC */
    /* PB12 is connected to EXTI_Line12, which has EXTI15_10_IRQn vector */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
    /* Set priority */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);
}

/* Handle PB12 interrupt */
void EXTI15_10_IRQHandler(void) {
	DWORD size;
		           UINT BytesWritten;
		           char stri[80];
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line15) != RESET) {

        /* Do your stuff when PB12 is changed */
    	if (GetLedState(LED_YEL) == LED_OFF) {
    					SwitchOnLED(LED_YEL);


    					EXTI_ClearITPendingBit(EXTI_Line15);

    	}


    				else {
    					SwitchOffLED(LED_YEL);

        /* Clear interrupt flag */
    					EXTI_ClearITPendingBit(EXTI_Line15);
    				}

    }
}

void FuelG_alert_init(void) {
    /* Set variables used */
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* Enable clock for GPIOB */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Set pin as input */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* Tell system that you will use PG3 for EXTI_Line3 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource3);

    /* PB12 is connected to EXTI_Line12 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line3;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);

    /* Add IRQ vector to NVIC */
    /* PB12 is connected to EXTI_Line12, which has EXTI15_10_IRQn vector */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
    /* Set priority */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);
}

void EXTI3_IRQHandler(void) {

	//int count=1000;
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
    	SwitchOffLED(LED_RED);
    	//SwitchOffLED(LED_GRE);
        /* Do your stuff when PB12 is changed */

    					SwitchOnLED(LED_YEL);





    }
    EXTI_ClearITPendingBit(EXTI_Line3);
}
