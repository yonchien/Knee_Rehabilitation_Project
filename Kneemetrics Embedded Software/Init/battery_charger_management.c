#include "battery_charger_management.h"

//#define M0
#define M1_GPIO_Pin 		GPIO_Pin_6
#define M1_GPIO_Port 		GPIOG
#define M1_GPIO_Clock		RCC_AHB1Periph_GPIOG

//#define M1
#define M0_GPIO_Pin 		GPIO_Pin_6
#define M0_GPIO_Port 		GPIOC
#define M0_GPIO_Clock 		RCC_AHB1Periph_GPIOC

static Mx_State M0_state;
static Mx_State M1_state;

void InitializeM0M1(void){

GPIO_InitTypeDef M0_InitTypedef;

	M0_InitTypedef.GPIO_Mode = GPIO_Mode_OUT;
	M0_InitTypedef.GPIO_OType = GPIO_OType_PP;
	M0_InitTypedef.GPIO_PuPd = GPIO_PuPd_UP;
	M0_InitTypedef.GPIO_Speed = GPIO_Speed_50MHz;
	M0_InitTypedef.GPIO_Pin = M0_GPIO_Pin;

	RCC_AHB1PeriphClockCmd(M0_GPIO_Clock, ENABLE);

	M0_state= Mx_High;

	GPIO_InitTypeDef M1_InitTypedef;

		M1_InitTypedef.GPIO_Mode = GPIO_Mode_OUT;
		M1_InitTypedef.GPIO_OType = GPIO_OType_PP;
		M1_InitTypedef.GPIO_Speed = GPIO_Speed_50MHz;
		M1_InitTypedef.GPIO_Pin = M1_GPIO_Pin;

	RCC_AHB1PeriphClockCmd(M1_GPIO_Clock, ENABLE);

	M1_state= Mx_Low;

	GPIO_Init(M0_GPIO_Port, &M0_InitTypedef);
	GPIO_Init(M1_GPIO_Port, &M1_InitTypedef);
}

void SetMxHigh(Mx_Type m)
{
	switch(m)
	{
	case M0:
		GPIO_WriteBit(M0_GPIO_Port, M0_GPIO_Pin, Bit_SET);
		M0_state = Mx_High;
		break;

	case M1:
		GPIO_WriteBit(M1_GPIO_Port, M1_GPIO_Pin, Bit_SET);
		M1_state = Mx_High;
		break;

	case BOTH_M:
		GPIO_WriteBit(M0_GPIO_Port, M0_GPIO_Pin, Bit_SET);
		GPIO_WriteBit(M1_GPIO_Port, M1_GPIO_Pin, Bit_SET);
		M0_state = Mx_High;
		M1_state = Mx_High;
		break;

	default:
		break;
	}
}

void SetMxLow(Mx_Type m)
{
	switch(m)
	{
	case M0:
		GPIO_WriteBit(M0_GPIO_Port, M0_GPIO_Pin, Bit_RESET);
		M0_state = Mx_Low;
		break;

	case M1:
		GPIO_WriteBit(M1_GPIO_Port, M1_GPIO_Pin, Bit_RESET);
		M1_state = Mx_Low;
		break;

	case BOTH_M:
		GPIO_WriteBit(M0_GPIO_Port, M0_GPIO_Pin, Bit_RESET);
		GPIO_WriteBit(M1_GPIO_Port, M1_GPIO_Pin, Bit_RESET);
		M0_state = Mx_Low;
		M1_state = Mx_Low;
		break;

	default:
		break;
	}
}

Mx_State GetMxState(Mx_Type m)
{
		switch(m)
		{
		case M0:
			return M0_state;

		case M1:
			return M1_state;

		default:
			return Mx_NA;
		}
}
