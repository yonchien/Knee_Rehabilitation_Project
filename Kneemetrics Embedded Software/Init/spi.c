#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rtc.h"
#include "spi.h"
#include "compiler.h"
#include "leds.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_tim.h"
#include "stdio.h"


void InitializeDW1000ChipSel(void)
{
	GPIO_InitTypeDef dw1000_gpio_css_init;
	RCC_AHB1PeriphClockCmd(DW1000_Chip_Sel_GPIO_RCC, ENABLE);
	dw1000_gpio_css_init.GPIO_Pin = DW1000_Chip_Sel_Pin;
	dw1000_gpio_css_init.GPIO_Mode = GPIO_Mode_OUT;
	dw1000_gpio_css_init.GPIO_OType = GPIO_OType_PP;
	dw1000_gpio_css_init.GPIO_PuPd = GPIO_PuPd_UP;
	dw1000_gpio_css_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DW1000_Chip_Sel_Port, &dw1000_gpio_css_init);
	SetDW1000ChipSelHigh();
}

void InitializeBMEChipSel(void)
{
	GPIO_InitTypeDef bme_gpio_css_init;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	bme_gpio_css_init.GPIO_Pin = GPIO_Pin_4;
	bme_gpio_css_init.GPIO_Mode = GPIO_Mode_OUT;
	bme_gpio_css_init.GPIO_OType = GPIO_OType_PP;
	bme_gpio_css_init.GPIO_PuPd = GPIO_PuPd_UP;
	bme_gpio_css_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &bme_gpio_css_init);
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
}

void InitializeEXTChipSel(void)
{
	GPIO_InitTypeDef bme_gpio_css_init;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	bme_gpio_css_init.GPIO_Pin = GPIO_Pin_15;
	bme_gpio_css_init.GPIO_Mode = GPIO_Mode_OUT;
	bme_gpio_css_init.GPIO_OType = GPIO_OType_PP;
	bme_gpio_css_init.GPIO_PuPd = GPIO_PuPd_UP;
	bme_gpio_css_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &bme_gpio_css_init);
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET);
}

void SetDW1000ChipSelLow(void)
{
	GPIO_WriteBit(DW1000_Chip_Sel_Port, DW1000_Chip_Sel_Pin, Bit_RESET);
}

void SetDW1000ChipSelHigh(void)
{
	GPIO_WriteBit(DW1000_Chip_Sel_Port, DW1000_Chip_Sel_Pin, Bit_SET);
}

void SPIUWBInit(uint16_t scalingfactor)
{
	GPIO_InitTypeDef dw1000_gpio_spi_init;
	SPI_InitTypeDef dw1000_spi_init;
	/* GPIOA clock enable */

	SPI_I2S_DeInit(DW1000_SPI);

	RCC_AHB1PeriphClockCmd(DW1000_SPI_GPIO_RCC, ENABLE);

	dw1000_gpio_spi_init.GPIO_Pin =  DW1000_SPI_Clock_Pin | DW1000_SPI_MISO_Pin | DW1000_SPI_MOSI_Pin;
	dw1000_gpio_spi_init.GPIO_Mode = GPIO_Mode_AF;
	dw1000_gpio_spi_init.GPIO_OType = GPIO_OType_PP;
	dw1000_gpio_spi_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	dw1000_gpio_spi_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DW1000_SPI_Port, &dw1000_gpio_spi_init);

	//GPIO_PinAFConfig(GPIOB, GPIO_PinSource12);
	GPIO_PinAFConfig(DW1000_SPI_Port, DW1000_PinSource_CLK, DW1000_GPIO_AF);
	GPIO_PinAFConfig(DW1000_SPI_Port, DW1000_PinSource_MISO, DW1000_GPIO_AF);
	GPIO_PinAFConfig(DW1000_SPI_Port, DW1000_PinSource_MOS1, DW1000_GPIO_AF);

	RCC_APB1PeriphClockCmd(DW1000_SPI_RCC, ENABLE);

	dw1000_spi_init.SPI_BaudRatePrescaler = scalingfactor;
	dw1000_spi_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	dw1000_spi_init.SPI_CPHA =  SPI_CPHA_1Edge; //SPI_CPHA_2Edge;
	dw1000_spi_init.SPI_CPOL = SPI_CPOL_Low;
	dw1000_spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
	dw1000_spi_init.SPI_NSS = SPI_NSS_Soft;
	dw1000_spi_init.SPI_Mode = SPI_Mode_Master;
	dw1000_spi_init.SPI_DataSize = SPI_DataSize_8b;
	dw1000_spi_init.SPI_CRCPolynomial = 7;

	InitializeDW1000ChipSel();

	SPI_Init(DW1000_SPI, &dw1000_spi_init);
	//SPI_SSOutputCmd(DW1000_SPI, DISABLE);
	SPI_Cmd(DW1000_SPI, ENABLE);

}

uint8_t SendByteToDW1000(uint8_t byte)
{

	//uint16_t count = 5000;
	SPI_I2S_SendData(DW1000_SPI, byte);
	//while(SPI_I2S_GetFlagStatus(DW1000_SPI, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(DW1000_SPI, SPI_I2S_FLAG_RXNE) == RESET){}
	return SPI_I2S_ReceiveData(DW1000_SPI);

}

uint8_t ReadByteFromDW1000(void)
{
	return SendByteToDW1000(DUMMY_BYTE);
}

void SendMultiByteToDW1000(const uint8_t *inBuffer, uint32_t len)
{
	while(len-- > 0)
	{
		SendByteToDW1000(*inBuffer++);
	}
}


void ReadMultiByteFromDW1000(uint8_t *outBuffer, uint32_t len)
{
	while(len-- > 0)
	{
		*outBuffer++ = ReadByteFromDW1000();
	}
}

void SPI_ConfigFastRate(uint16_t scalingfactor)
{
	SPIUWBInit(scalingfactor);
}





void init_SPI1(void){


	/*GPIO_InitTypeDef bme_gpio_css_init;
	GPIO_InitTypeDef bme_gpio_spi_init;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		bme_gpio_css_init.GPIO_Pin = BME_Chip_Sel_Pin;
		bme_gpio_css_init.GPIO_Mode = GPIO_Mode_OUT;
		bme_gpio_css_init.GPIO_OType = GPIO_OType_PP;
		bme_gpio_css_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
		bme_gpio_css_init.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(BME_Chip_Sel_Port, &bme_gpio_css_init);
		//SetBMEChipSelHigh();
		GPIO_SetBits(GPIOA, GPIO_Pin_15);*/

		GPIO_InitTypeDef GPIO_InitDef;
		GPIO_InitTypeDef GPIO_InitDef2;

		    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		    GPIO_InitDef.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
		    GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
		    GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
		    GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
		    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
		    //Initialize pins
		    GPIO_Init(GPIOA, &GPIO_InitDef);
	//	    GPIO_SetBits(GPIOB, GPIO_Pin_3);

		  //  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		   		   GPIO_InitDef2.GPIO_Pin = GPIO_Pin_7;
		   		    GPIO_InitDef2.GPIO_Mode = GPIO_Mode_IN;
		   		    GPIO_InitDef2.GPIO_OType = GPIO_OType_PP;
		   		    GPIO_InitDef2.GPIO_PuPd = GPIO_PuPd_NOPULL;
		   		    GPIO_InitDef2.GPIO_Speed = GPIO_Speed_50MHz;
		   		    //Initialize pins
		   		    GPIO_Init(GPIOA, &GPIO_InitDef2);


		   		 InitializeBMEChipSel();

}

void init_SPIEXT(void){


	/*GPIO_InitTypeDef bme_gpio_css_init;
	GPIO_InitTypeDef bme_gpio_spi_init;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		bme_gpio_css_init.GPIO_Pin = BME_Chip_Sel_Pin;
		bme_gpio_css_init.GPIO_Mode = GPIO_Mode_OUT;
		bme_gpio_css_init.GPIO_OType = GPIO_OType_PP;
		bme_gpio_css_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
		bme_gpio_css_init.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(BME_Chip_Sel_Port, &bme_gpio_css_init);
		//SetBMEChipSelHigh();
		GPIO_SetBits(GPIOA, GPIO_Pin_15);*/

		GPIO_InitTypeDef GPIO_InitDef;
		GPIO_InitTypeDef GPIO_InitDef2;

		    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

		    GPIO_InitDef.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
		    GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
		    GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
		    GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
		    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
		    //Initialize pins
		    GPIO_Init(GPIOB, &GPIO_InitDef);
	//	    GPIO_SetBits(GPIOB, GPIO_Pin_3);

		  //  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		   		   GPIO_InitDef2.GPIO_Pin = GPIO_Pin_4;
		   		    GPIO_InitDef2.GPIO_Mode = GPIO_Mode_IN;
		   		    GPIO_InitDef2.GPIO_OType = GPIO_OType_PP;
		   		    GPIO_InitDef2.GPIO_PuPd = GPIO_PuPd_NOPULL;
		   		    GPIO_InitDef2.GPIO_Speed = GPIO_Speed_50MHz;
		   		    //Initialize pins
		   		    GPIO_Init(GPIOB, &GPIO_InitDef2);


		   		 InitializeEXTChipSel();

}

void send_8bit_serial_reg(uint8_t data)
{
   int i;
   int a=0;
 //  GPIO_ResetBits(GPIOA, GPIO_Pin_15); //A4
   data = data & ~0x80;

      // send bits 7..0
   for (i = 0; i < 8; i++)
     {
         // consider leftmost bit
         // set line high if bit is 1, low if bit is 0
         if (data & 0x80)
      	   GPIO_SetBits(GPIOA, GPIO_Pin_6); //A6
         else
      	   GPIO_ResetBits(GPIOA, GPIO_Pin_6); //A6

         // pulse clock to indicate that bit value should be read
         GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
       a++;
       a++;
       a++;
       a++;
       a++;
       a++;
       a++;
            a++;
            a++;

         GPIO_SetBits(GPIOA, GPIO_Pin_5); //A5

         // shift byte left so next bit will be leftmost
         data <<= 1;
     }




   // select device
  // output_high(SD_CS);
/*   GPIO_ResetBits(GPIOA, GPIO_Pin_15);
 Sleep(1);
   // send bits 7..0
   int spi_shift_count=8;
          while(spi_shift_count--)
   {
        	  GPIO_ResetBits(GPIOB, BME_SPI_Clock_Pin);
       // consider leftmost bit
       // set line high if bit is 1, low if bit is 0
        	  Sleep(1);
       if (data & 0x80)
    	   GPIO_SetBits(GPIOB, BME_SPI_MOSI_Pin);
       else
    	   GPIO_ResetBits(GPIOB, BME_SPI_MOSI_Pin);

       // pulse clock to indicate that bit value should be read

       Sleep(1);
       GPIO_SetBits(GPIOB, BME_SPI_Clock_Pin);

       // shift byte left so next bit will be leftmost
       data <<= 1;
   }*/

   // deselect device
  // output_low(SD_CS);
      //    Sleep(1);
               //GPIO_ResetBits(GPIOB, GPIO_Pin_3); //A5
 //  GPIO_SetBits(GPIOA, GPIO_Pin_15); //A4
}
void send_8bit_serial_data(uint8_t reg, uint8_t data)
{
   int i;
   int a=0;
 //  GPIO_ResetBits(GPIOA, GPIO_Pin_15); //A4
   reg = reg & ~0x80;

      // send bits 7..0
   for (i = 0; i < 8; i++)
     {
         // consider leftmost bit
         // set line high if bit is 1, low if bit is 0
         if (reg & 0x80)
      	   GPIO_SetBits(GPIOA, GPIO_Pin_6); //A6
         else
      	   GPIO_ResetBits(GPIOA, GPIO_Pin_6); //A6

         // pulse clock to indicate that bit value should be read
         GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
       a++;
       a++;
       a++;
       a++;
       a++;
       a++;
       a++;
            a++;
            a++;

         GPIO_SetBits(GPIOA, GPIO_Pin_5); //A5

         // shift byte left so next bit will be leftmost
         reg <<= 1;

     }
   a++;
          a++;
               a++;
               a++;
   for (i = 0; i < 8; i++)
        {
            // consider leftmost bit
            // set line high if bit is 1, low if bit is 0
            if (data & 0x80)
         	   GPIO_SetBits(GPIOA, GPIO_Pin_6); //A6
            else
         	   GPIO_ResetBits(GPIOA, GPIO_Pin_6); //A6

            // pulse clock to indicate that bit value should be read
            GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
          a++;
          a++;
          a++;
          a++;
          a++;
          a++;
          a++;
               a++;
               a++;

            GPIO_SetBits(GPIOA, GPIO_Pin_5); //A5

            // shift byte left so next bit will be leftmost
            data <<= 1;

        }




   // select device
  // output_high(SD_CS);
/*   GPIO_ResetBits(GPIOA, GPIO_Pin_15);
 Sleep(1);
   // send bits 7..0
   int spi_shift_count=8;
          while(spi_shift_count--)
   {
        	  GPIO_ResetBits(GPIOB, BME_SPI_Clock_Pin);
       // consider leftmost bit
       // set line high if bit is 1, low if bit is 0
        	  Sleep(1);
       if (data & 0x80)
    	   GPIO_SetBits(GPIOB, BME_SPI_MOSI_Pin);
       else
    	   GPIO_ResetBits(GPIOB, BME_SPI_MOSI_Pin);

       // pulse clock to indicate that bit value should be read

       Sleep(1);
       GPIO_SetBits(GPIOB, BME_SPI_Clock_Pin);

       // shift byte left so next bit will be leftmost
       data <<= 1;
   }*/

   // deselect device
  // output_low(SD_CS);
      //    Sleep(1);
               //GPIO_ResetBits(GPIOB, GPIO_Pin_3); //A5
 //  GPIO_SetBits(GPIOA, GPIO_Pin_15); //A4
}



uint8_t read_8bit_serial_data(uint8_t data)
{
   int i;
   int a=0;
   uint8_t spi_shift_in;
   int spi_shift_count=8;
   data = 0x80 | data;
   // select device
  // output_high(SD_CS);
   GPIO_ResetBits(GPIOA, GPIO_Pin_4); //A4

   // send bits 7..0

   for (i = 0; i < 8; i++)
   {
       // consider leftmost bit
       // set line high if bit is 1, low if bit is 0
       if (data & 0x80)
    	   GPIO_SetBits(GPIOA, GPIO_Pin_6); //A6
       else
    	   GPIO_ResetBits(GPIOA, GPIO_Pin_6);  //A6

       // pulse clock to indicate that bit value should be read
       GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
     a++;
     a++;
     a++;
     a++;
     a++;
     a++;
     a++;
          a++;
          a++;

       GPIO_SetBits(GPIOA, GPIO_Pin_5); //A5

       // shift byte left so next bit will be leftmost
       data <<= 1;
   }


   a++;
             a++;
             a++;
             GPIO_ResetBits(GPIOA, GPIO_Pin_6);  //A6
   for (i = 0; i < 8; i++)
    {

	   GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5



	                spi_shift_in<<=1;
        // consider leftmost bit
        // set line high if bit is 1, low if bit is 0
	  if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)){ //A7

	           	   spi_shift_in|=1;


	   }
        // pulse clock to indicate that bit value should be read








        GPIO_SetBits(GPIOA, GPIO_Pin_5); //A5
        a++;
        	                    a++;
        	                    a++;
        	                    a++;
        	                    a++;
        	                    a++;
        	                            	                    a++;


        // shift byte left so next bit will be leftmost
        //data <<= 1;
    }


   GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
   GPIO_SetBits(GPIOA, GPIO_Pin_4); //A4

   return spi_shift_in;
}


uint16_t read_24bit_serial_data(uint8_t data)
{
   int i;
   int a=0;
   uint16_t spi_shift_in;
   int spi_shift_count=8;
   data = 0x80 | data;
   // select device
  // output_high(SD_CS);
   GPIO_ResetBits(GPIOA, GPIO_Pin_4);

   // send bits 7..0

   for (i = 0; i < 8; i++)
   {
       // consider leftmost bit
       // set line high if bit is 1, low if bit is 0
       if (data & 0x80)
    	   GPIO_SetBits(GPIOA, GPIO_Pin_6); //A6
       else
    	   GPIO_ResetBits(GPIOA, GPIO_Pin_6);  //A6

       // pulse clock to indicate that bit value should be read
       GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
     a++;
     a++;
     a++;
     a++;
     a++;
     a++;
     a++;
          a++;
          a++;

       GPIO_SetBits(GPIOA, GPIO_Pin_5); //A5

       // shift byte left so next bit will be leftmost
       data <<= 1;
   }


   a++;
             a++;
             a++;
             GPIO_ResetBits(GPIOA, GPIO_Pin_6);  //A6
   for (i = 0; i < 23; i++)
    {

	   GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5



	                spi_shift_in<<=1;
        // consider leftmost bit
        // set line high if bit is 1, low if bit is 0
	  if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)){ //A7

	           	   spi_shift_in|=1;


	   }
        // pulse clock to indicate that bit value should be read








        GPIO_SetBits(GPIOA, GPIO_Pin_5); //A5
        a++;
        	                    a++;
        	                    a++;
        	                    a++;
        	                    a++;
        	                    a++;
        	                            	                    a++;


        // shift byte left so next bit will be leftmost
        //data <<= 1;
    }


   GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
   GPIO_SetBits(GPIOA, GPIO_Pin_4); //A4

   return spi_shift_in;
}


void SPIBMEInit(void)
{
	SPI_I2S_DeInit(SPI1);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	SPI_InitTypeDef SPI_InitTypeDefStruct;


	SPI_InitTypeDefStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitTypeDefStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitTypeDefStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitTypeDefStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitTypeDefStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitTypeDefStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitTypeDefStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitTypeDefStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitTypeDefStruct.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitTypeDefStruct);


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitTypeDefStruct;

	GPIO_InitTypeDefStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitTypeDefStruct);

	//init chip select
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDefStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitTypeDefStruct);


	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

	GPIO_SetBits(GPIOA, GPIO_Pin_15);

//	SPI_Init(SPI1, &SPI_InitTypeDefStruct);
	SPI_Cmd(SPI1, ENABLE);

}



uint8_t read_8bit_serial_data3(uint8_t adress){

	GPIO_ResetBits(GPIOA, GPIO_Pin_15);

	adress = 0x80 | adress;

	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1, adress);
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	SPI_I2S_ReceiveData(SPI1); //Clear RXNE bit

	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1, 0x00); //Dummy byte to generate clock
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));

	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI1, 0x00); //Dummy byte to generate clock
		while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));

		while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
			SPI_I2S_SendData(SPI1, 0x00); //Dummy byte to generate clock
			while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));


uSleep(10);
	GPIO_SetBits(GPIOA, GPIO_Pin_15);

	return  SPI_I2S_ReceiveData(SPI1);
}

void mySPI_SendData(uint8_t adress, uint8_t data)
{
//adress = adress & ~0x80; //WRITE command. For readability
GPIO_ResetBits(GPIOA, GPIO_Pin_15);

/*
SPI_I2S_SendData(SPI1, adress);
	//while(SPI_I2S_GetFlagStatus(DW1000_SPI, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){}
	SPI_I2S_SendData(SPI1, data);
		//while(SPI_I2S_GetFlagStatus(DW1000_SPI, SPI_I2S_FLAG_TXE) == RESET);
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){}
	return (uint8_t) SPI_I2S_ReceiveData(SPI1);*/


while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); //transmit buffer empty?
SPI_I2S_SendData(SPI1, adress);
while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)); //data received?
SPI_I2S_ReceiveData(SPI1);

while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)); //transmit buffer empty?
SPI_I2S_SendData(SPI1, data);
while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)); //data received?

SPI_I2S_ReceiveData(SPI1);

GPIO_SetBits(GPIOA, GPIO_Pin_15);
}

uint8_t mySPI_GetData(uint8_t adress){

	GPIO_ResetBits(GPIOA, GPIO_Pin_15);

	adress = 0x80 | adress;

	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1, adress);
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	SPI_I2S_ReceiveData(SPI1); //Clear RXNE bit

	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1, 0x00); //Dummy byte to generate clock
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));

	GPIO_SetBits(GPIOA, GPIO_Pin_15);

	return  SPI_I2S_ReceiveData(SPI1);
}
