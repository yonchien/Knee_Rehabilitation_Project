#ifndef _SPI_H_
#define _SPI_H_

#ifdef __cplusplus
extern "C" {
#endif


#include<stdint.h>

#define DUMMY_BYTE                 ((uint8_t)0x00)

/*================================DW1000-SPI2============================================*/
#define DW1000_SPI_Clock_Pin 		GPIO_Pin_13
#define DW1000_SPI_MISO_Pin 		GPIO_Pin_14
#define DW1000_SPI_MOSI_Pin			GPIO_Pin_15

#define DW1000_SPI_Port				GPIOB
#define DW1000_SPI_CLOCK			8000000

#define DW1000_SPI					SPI2
#define DW1000_SPI_GPIO_RCC			RCC_AHB1Periph_GPIOB
#define DW1000_SPI_RCC				RCC_APB1Periph_SPI2

#define DW1000_GPIO_AF				GPIO_AF_SPI2
#define DW1000_PinSource_CLK		GPIO_PinSource13
#define DW1000_PinSource_MISO		GPIO_PinSource14
#define DW1000_PinSource_MOS1		GPIO_PinSource15

#define DW1000_Chip_Sel_Pin			GPIO_Pin_12
#define DW1000_Chip_Sel_Port		GPIOB
#define DW1000_Chip_Sel_GPIO_RCC	RCC_AHB1Periph_GPIOB

void SPIUWBInit(uint16_t scalingfactor);

void InitializeDW1000ChipSel(void);

void SetDW1000ChipSelLow(void);
void SetDW1000ChipSelHigh(void);

void SPI_ConfigFastRate(uint16_t scalingfactor);

void SendMultiByteToDW1000(const uint8_t *inBuffer, uint32_t len);
void ReadMultiByteFromDW1000(uint8_t *outBuffer, uint32_t len);

#ifdef __cplusplus
}
#endif


#endif


