#include "enable_and_reset.h"

void InitializeUWBReset(void)
{
	GPIO_InitTypeDef uwb_gpio_res_init;
	RCC_AHB1PeriphClockCmd(DW1000_UWB_Reset_GPIO_RCC, ENABLE);
	uwb_gpio_res_init.GPIO_Pin = DW1000_UWB_Reset_Pin;
	uwb_gpio_res_init.GPIO_Mode = GPIO_Mode_OUT;
	uwb_gpio_res_init.GPIO_OType = GPIO_OType_PP;
	uwb_gpio_res_init.GPIO_PuPd = GPIO_PuPd_UP;
	uwb_gpio_res_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DW1000_UWB_Reset_Port, &uwb_gpio_res_init);
	GPIO_SetBits(DW1000_UWB_Reset_Port, DW1000_UWB_Reset_Pin);
}
