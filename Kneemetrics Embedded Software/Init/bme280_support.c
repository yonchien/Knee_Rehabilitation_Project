/*
****************************************************************************
* Copyright (C) 2014 - 2015 Bosch Sensortec GmbH
*
* bme280_support.c
* Date: 2015/03/27
* Revision: 1.0.5 $
*
* Usage: Sensor Driver support file for BME280 sensor
*
****************************************************************************
* License:
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
*   Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the distribution.
*
*   Neither the name of the copyright holder nor the names of the
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
* OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
*
* The information provided is believed to be accurate and reliable.
* The copyright holder assumes no responsibility
* for the consequences of use
* of such information nor for any infringement of patents or
* other rights of third parties which may result from its use.
* No license is granted by implication or otherwise under any patent or
* patent rights of the copyright holder.
**************************************************************************/
/*---------------------------------------------------------------------------*/
/* Includes*/
/*---------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*
*  The following functions are used for reading and writing of
*	sensor data using I2C or SPI communication
*----------------------------------------------------------------------------*/

#include "stm32f4xx.h"
#include "bme280.h"
#include <inttypes.h>
#include "compiler.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_sdio.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_sdio_sd.h"


#include "ff.h"
#include "diskio.h"

char tmp_data[20];
bme280_calibration_param cal_param;
 //int32_t  adc_p, adc_h;
 s32 t_fine;
//int32_t adc_t;




FRESULT res;
FIL fil;
/********************End of I2C/SPI function declarations***********************/
/*	Brief : The delay routine
 *	\param : delay in ms
*/
char BME280_GetCtrlMeasurement(void);
char BME280_GetStatus(void);
char BME280_GetConfig(void);
static long BME280_Compensate_T(void);
static unsigned long BME280_Compensate_P(void);

void read_coefficients(void){
	DWORD size;
	           UINT BytesWritten;

char stri[80];


//char lsb, msb;

 // msb = read_8bit_serial_data(0x89);
 // cal_param.dig_T1 = (unsigned int) msb;
  //lsb = read_8bit_serial_data(0x89)
  //cal_param.dig_T1 = (read_8bit_serial_data(0x89)<<8) | read_8bit_serial_data(0x88);
  cal_param.dig_T1= (u16)((((u16)((u8)read_8bit_serial_data(0x89)))<< 8)| read_8bit_serial_data(0x88));
  //dd= ~(cal_param.dig_T1)+1;
 // cal_param.dig_T1= (cal_param.dig_T1>>8)|(cal_param.dig_T1<<8);
 // msb = read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T2_MSB_REG);
  //  cal_param.dig_T2 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T2_LSB_REG);
    //cal_param.dig_T2 = (read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T2_MSB_REG) << 8) | read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T2_LSB_REG);
  cal_param.dig_T2= (s16)((((s16)((s8)read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T2_MSB_REG)))<< 8)| read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T2_LSB_REG));

  //  cal_param.dig_T2= (cal_param.dig_T2>>8)|(cal_param.dig_T2<<8);

//    msb = read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T3_MSB_REG);
 //   cal_param.dig_T3 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T3_LSB_REG);
   // cal_param.dig_T3 = (read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T3_MSB_REG) << 8) | read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T3_LSB_REG);
  cal_param.dig_T3= (s16)((((s16)((s8)read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T3_MSB_REG)))<< 8)| read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T3_LSB_REG));
  //  cal_param.dig_T3= (cal_param.dig_T3>>8)|(cal_param.dig_T3<<8);

  //  msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P1_MSB_REG);
  //  cal_param.dig_P1 = (unsigned int) msb;
  //  lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P1_LSB_REG);
   // cal_param.dig_P1 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P1_MSB_REG))| read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P1_LSB_REG);
    cal_param.dig_P1= (u16)((((u16)((u8)read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P1_MSB_REG)))<< 8)| read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P1_LSB_REG));


 //   msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P2_MSB_REG);
 //   cal_param.dig_P2 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P2_LSB_REG);
    cal_param.dig_P2= (s16)((((s16)((s8)read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P2_MSB_REG)))<< 8)| read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P2_LSB_REG));
    //cal_param.dig_P2 = (int16)(read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P2_MSB_REG)<< 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P2_LSB_REG);


  //  msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P3_MSB_REG);
  //  cal_param.dig_P3 = (int) msb;
  //  lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P3_LSB_REG);
    //cal_param.dig_P3 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P3_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P3_LSB_REG);
    cal_param.dig_P3= (s16)((((s16)((s8)read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P3_MSB_REG)))<< 8)| read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P3_LSB_REG));


    //msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P4_MSB_REG);
  //  cal_param.dig_P4 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P4_LSB_REG);
   // cal_param.dig_P4 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P4_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P4_LSB_REG);
    cal_param.dig_P4= (s16)((((s16)((s8)read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P4_MSB_REG)))<< 8)| read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P4_LSB_REG));


 //   msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P5_MSB_REG);
 //   cal_param.dig_P5 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P5_LSB_REG);
   // cal_param.dig_P5 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P5_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P5_LSB_REG);
    cal_param.dig_P5= (s16)((((s16)((s8)read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P5_MSB_REG)))<< 8)| read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P5_LSB_REG));


 //   msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P6_MSB_REG);
  //  cal_param.dig_P6 = (int) msb;
//    lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P6_LSB_REG);
  //  cal_param.dig_P6 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P6_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P6_LSB_REG);
    cal_param.dig_P6= (s16)((((s16)((s8)read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P6_MSB_REG)))<< 8)| read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P6_LSB_REG));


   // msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P7_MSB_REG);
 //   cal_param.dig_P7 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P7_LSB_REG);
  //  cal_param.dig_P7 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P7_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P7_LSB_REG);
    cal_param.dig_P7= (s16)((((s16)((s8)read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P7_MSB_REG)))<< 8)| read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P7_LSB_REG));


 //   msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P8_MSB_REG);
 //   cal_param.dig_P8 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P8_LSB_REG);
  //  cal_param.dig_P8 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P8_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P8_LSB_REG);
    cal_param.dig_P8= (s16)((((s16)((s8)read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P8_MSB_REG)))<< 8)| read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P8_LSB_REG));


 //   msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P9_MSB_REG);
  //  cal_param.dig_P9 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P9_LSB_REG);
  //  cal_param.dig_P9 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P9_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P9_LSB_REG);
    cal_param.dig_P9= (s16)((((s16)((s8)read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P9_MSB_REG)))<< 8)| read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P9_LSB_REG));



   // lsb = read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H1_REG);
    cal_param.dig_H1 = read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H1_REG);


  //  msb = read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H2_MSB_REG);
 //   cal_param.dig_H2 = (int) msb;
  //  lsb = read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H2_LSB_REG);
    cal_param.dig_H2 = (read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H2_MSB_REG) << 8) | read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H2_LSB_REG);

   // lsb = read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H3_REG);
    cal_param.dig_H3 = read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H3_REG);


//    msb = read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H4_MSB_REG);
  //  cal_param.dig_H4 = (int) msb;
  //  lsb = read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H4_LSB_REG);
    cal_param.dig_H4 = (read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H4_MSB_REG) << 4) | (read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H4_LSB_REG) & 0xF);

  //  msb = read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H5_MSB_REG);
 //   cal_param.dig_H5 = (int) msb;
    cal_param.dig_H5 = (read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H5_MSB_REG) << 4) | (read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H4_LSB_REG) >> 4);

   // lsb = read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H6_REG);
    cal_param.dig_H6 = read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H6_REG);


	    /*bme280_calib.dig_T1 = (unsigned int) read_16bit_serial_data(BME280_REGISTER_DIG_T1);
	    bme280_calib.dig_T2 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_T2);
	    bme280_calib.dig_T3 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_T3);

	    bme280_calib.dig_P1 = read_16bit_serial_data(BME280_REGISTER_DIG_P1);
	    bme280_calib.dig_P2 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P2);
	    bme280_calib.dig_P3 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P3);
	    bme280_calib.dig_P4 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P4);
	    bme280_calib.dig_P5 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P5);
	    bme280_calib.dig_P6 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P6);
	    bme280_calib.dig_P7 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P7);
	    bme280_calib.dig_P8 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P8);
	    bme280_calib.dig_P9 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P9);

	    bme280_calib.dig_H1 = read_8bit_serial_data(BME280_REGISTER_DIG_H1);
	    bme280_calib.dig_H2 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_H2);
	    bme280_calib.dig_H3 = read_8bit_serial_data(BME280_REGISTER_DIG_H3);
	    bme280_calib.dig_H4 = (read_8bit_serial_data(BME280_REGISTER_DIG_H4) << 4) | (read_8bit_serial_data(BME280_REGISTER_DIG_H4+1) & 0xF);
	    bme280_calib.dig_H5 = (read_8bit_serial_data(BME280_REGISTER_DIG_H5+1) << 4) | (read_8bit_serial_data(BME280_REGISTER_DIG_H5) >> 4);
	    bme280_calib.dig_H6 = (int8_t)read_8bit_serial_data(BME280_REGISTER_DIG_H6);*/



    sprintf(stri, "%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n", cal_param.dig_T1, cal_param.dig_T2,cal_param.dig_T3,cal_param.dig_P1,cal_param.dig_P2,cal_param.dig_P3,cal_param.dig_P4,cal_param.dig_P5,cal_param.dig_P6,cal_param.dig_P7,cal_param.dig_P8,cal_param.dig_P9);

    res = f_open(&fil,"calpar.txt",FA_WRITE | FA_OPEN_ALWAYS);

                     size = (&fil)->fsize;

                       res = f_lseek(&fil,size);

              	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

                      f_close(&fil);

                      sprintf(stri, "%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n",cal_param.dig_H1,cal_param.dig_H2,cal_param.dig_H3,cal_param.dig_H4,cal_param.dig_H5,cal_param.dig_H6);

                      res = f_open(&fil,"calpar.txt",FA_WRITE | FA_OPEN_ALWAYS);

                                           size = (&fil)->fsize;

                                             res = f_lseek(&fil,size);

                                    	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

                                            f_close(&fil);
}

void read_coefficients2(void){
	DWORD size;
	           UINT BytesWritten;

char stri[80];
//char lsb, msb;

 // msb = read_8bit_serial_data(0x89);
 // cal_param.dig_T1 = (unsigned int) msb;
  //lsb = read_8bit_serial_data(0x89)
  cal_param.dig_T1 = (read_8bit_serial_data(0x89)<<8) |read_8bit_serial_data(0x88);
 // cal_param.dig_T1= (cal_param.dig_T1>>8)|(cal_param.dig_T1<<8);
 // msb = read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T2_MSB_REG);
  //  cal_param.dig_T2 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T2_LSB_REG);
    cal_param.dig_T2 = (read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T2_MSB_REG) << 8) | read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T2_LSB_REG);
  //  cal_param.dig_T2= (cal_param.dig_T2>>8)|(cal_param.dig_T2<<8);

//    msb = read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T3_MSB_REG);
 //   cal_param.dig_T3 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T3_LSB_REG);
    cal_param.dig_T3 = (read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T3_MSB_REG) << 8) | read_8bit_serial_data(BME280_TEMPERATURE_CALIB_DIG_T3_LSB_REG);
  //  cal_param.dig_T3= (cal_param.dig_T3>>8)|(cal_param.dig_T3<<8);

  //  msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P1_MSB_REG);
  //  cal_param.dig_P1 = (unsigned int) msb;
  //  lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P1_LSB_REG);
    cal_param.dig_P1 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P1_MSB_REG))| read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P1_LSB_REG);


 //   msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P2_MSB_REG);
 //   cal_param.dig_P2 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P2_LSB_REG);
    cal_param.dig_P2 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P2_MSB_REG)<< 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P2_LSB_REG);


  //  msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P3_MSB_REG);
  //  cal_param.dig_P3 = (int) msb;
  //  lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P3_LSB_REG);
    cal_param.dig_P3 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P3_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P3_LSB_REG);


    //msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P4_MSB_REG);
  //  cal_param.dig_P4 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P4_LSB_REG);
    cal_param.dig_P4 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P4_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P4_LSB_REG);


 //   msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P5_MSB_REG);
 //   cal_param.dig_P5 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P5_LSB_REG);
    cal_param.dig_P5 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P5_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P5_LSB_REG);


 //   msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P6_MSB_REG);
  //  cal_param.dig_P6 = (int) msb;
//    lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P6_LSB_REG);
    cal_param.dig_P6 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P6_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P6_LSB_REG);


   // msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P7_MSB_REG);
 //   cal_param.dig_P7 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P7_LSB_REG);
    cal_param.dig_P7 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P7_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P7_LSB_REG);


 //   msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P8_MSB_REG);
 //   cal_param.dig_P8 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P8_LSB_REG);
    cal_param.dig_P8 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P8_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P8_LSB_REG);


 //   msb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P9_MSB_REG);
  //  cal_param.dig_P9 = (int) msb;
 //   lsb = read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P9_LSB_REG);
    cal_param.dig_P9 = (read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P9_MSB_REG) << 8) | read_8bit_serial_data(BME280_PRESSURE_CALIB_DIG_P9_LSB_REG);

   // lsb = read_8bit_serial_data(BME280_HUMIDITY_CALIB_DIG_H1_REG);



	    /*bme280_calib.dig_T1 = (unsigned int) read_16bit_serial_data(BME280_REGISTER_DIG_T1);
	    bme280_calib.dig_T2 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_T2);
	    bme280_calib.dig_T3 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_T3);

	    bme280_calib.dig_P1 = read_16bit_serial_data(BME280_REGISTER_DIG_P1);
	    bme280_calib.dig_P2 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P2);
	    bme280_calib.dig_P3 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P3);
	    bme280_calib.dig_P4 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P4);
	    bme280_calib.dig_P5 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P5);
	    bme280_calib.dig_P6 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P6);
	    bme280_calib.dig_P7 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P7);
	    bme280_calib.dig_P8 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P8);
	    bme280_calib.dig_P9 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_P9);

	    bme280_calib.dig_H1 = read_8bit_serial_data(BME280_REGISTER_DIG_H1);
	    bme280_calib.dig_H2 = (int16_t)read_16bit_serial_data(BME280_REGISTER_DIG_H2);
	    bme280_calib.dig_H3 = read_8bit_serial_data(BME280_REGISTER_DIG_H3);
	    bme280_calib.dig_H4 = (read_8bit_serial_data(BME280_REGISTER_DIG_H4) << 4) | (read_8bit_serial_data(BME280_REGISTER_DIG_H4+1) & 0xF);
	    bme280_calib.dig_H5 = (read_8bit_serial_data(BME280_REGISTER_DIG_H5+1) << 4) | (read_8bit_serial_data(BME280_REGISTER_DIG_H5) >> 4);
	    bme280_calib.dig_H6 = (int8_t)read_8bit_serial_data(BME280_REGISTER_DIG_H6);*/

    sprintf(stri, "%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n",cal_param.dig_T1, cal_param.dig_T2,cal_param.dig_T3,cal_param.dig_P1,cal_param.dig_P2,cal_param.dig_P3,cal_param.dig_P4,cal_param.dig_P5,cal_param.dig_P6,cal_param.dig_P7,cal_param.dig_P8,cal_param.dig_P9);

    res = f_open(&fil,"calpar.txt",FA_WRITE | FA_OPEN_ALWAYS);

                     size = (&fil)->fsize;

                       res = f_lseek(&fil,size);

              	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

                      f_close(&fil);


}

void BME280_INIT() {
      BME280_SetStandbyTime(BME280_STANDBY_TIME_1_MS);                              // Standby time 1ms
  BME280_SetFilterCoefficient(BME280_FILTER_COEFF_16);                          // IIR Filter coefficient 16
  BME280_SetOversamplingPressure(BME280_OVERSAMP_16X);                          // Pressure x16 oversampling
  BME280_SetOversamplingTemperature(BME280_OVERSAMP_16X);                        // Temperature x2 oversampling
  BME280_SetOversamplingHumidity(BME280_OVERSAMP_1X);                           // Humidity x1 oversampling
  BME280_SetOversamplingMode(BME280_NORMAL_MODE);
}

void BME280_SetStandbyTime(char Value) {
  char cfgv;
  cfgv = BME280_GetConfig();
  cfgv &= ~BME280_CONFIG_REG_TSB__MSK;
  cfgv |= Value << BME280_CONFIG_REG_TSB__POS;
  GPIO_ResetBits(GPIOA, GPIO_Pin_4); //A4
    send_8bit_serial_data(BME280_CONFIG_REG);
    send_8bit_serial_data(cfgv);
    //mySPI_SendData(BME280_CTRL_MEAS_REG,ctrlm);
    GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
    GPIO_SetBits(GPIOA, GPIO_Pin_4); //A4
}

void BME280_SetFilterCoefficient(char Value) {
  char cfgv;
  cfgv = BME280_GetConfig();
  cfgv &= ~BME280_CONFIG_REG_FILTER__MSK;
  cfgv |= Value << BME280_CONFIG_REG_FILTER__POS;
  GPIO_ResetBits(GPIOA, GPIO_Pin_4); //A4
      send_8bit_serial_data(BME280_CONFIG_REG);
      send_8bit_serial_data(cfgv);
      //mySPI_SendData(BME280_CTRL_MEAS_REG,ctrlm);
      GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
      GPIO_SetBits(GPIOA, GPIO_Pin_4); //A4
}






char BME280_GetConfig() {
  return read_8bit_serial_data(BME280_CONFIG_REG);
}

void BME280_SetOversamplingPressure(char Value) {
  char ctrlm;
  char stri[50];
  char ff;
  ctrlm = BME280_GetCtrlMeasurement();

//write 0x04 on 0xF4 (normal mode)
  ctrlm &= ~BME280_CTRL_MEAS_REG_OVERSAMP_PRESSURE__MSK;

  ctrlm |= Value << BME280_CTRL_MEAS_REG_OVERSAMP_PRESSURE__POS;

  GPIO_ResetBits(GPIOA, GPIO_Pin_4); //A4
  send_8bit_serial_data(BME280_CTRL_MEAS_REG);
  send_8bit_serial_data(ctrlm);
  //mySPI_SendData(BME280_CTRL_MEAS_REG,ctrlm);
  GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
  GPIO_SetBits(GPIOA, GPIO_Pin_4); //A4
  ff= BME280_GetCtrlMeasurement();
  sprintf(stri, "read= %x",ff);//prepare string to be sent through uart
  		 						USART_puts(USART6,stri);

}

void BME280_SetOversamplingTemperature(char Value) {
  char ctrlm;
  char stri[50];
    char ff;

  ctrlm = BME280_GetCtrlMeasurement();
  ctrlm &= ~BME280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE__MSK;
  ctrlm |= Value << BME280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE__POS;
  GPIO_ResetBits(GPIOA, GPIO_Pin_4); //A4
  send_8bit_serial_data(BME280_CTRL_MEAS_REG);
  send_8bit_serial_data(ctrlm);
  GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
    GPIO_SetBits(GPIOA, GPIO_Pin_4); //A4
    ff= BME280_GetCtrlMeasurement();
    sprintf(stri, "read= %x",ff);//prepare string to be sent through uart
    		 						USART_puts(USART6,stri);
}

void BME280_SetOversamplingHumidity(char Value) {

	GPIO_ResetBits(GPIOA, GPIO_Pin_4); //A4
	send_8bit_serial_data(BME280_CTRL_HUMIDITY_REG);
	send_8bit_serial_data(Value);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
	    GPIO_SetBits(GPIOA, GPIO_Pin_4); //A4
}

void BME280_SetOversamplingMode(char Value) {
  char ctrlm;
  ctrlm = BME280_GetCtrlMeasurement();
  ctrlm |= Value;
  GPIO_ResetBits(GPIOA, GPIO_Pin_4); //A4
  	send_8bit_serial_data(BME280_CTRL_MEAS_REG);
  	send_8bit_serial_data(ctrlm);
  	GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
  	    GPIO_SetBits(GPIOA, GPIO_Pin_4); //A4

}

char BME280_GetCtrlMeasurement() {
  return read_8bit_serial_data(BME280_CTRL_MEAS_REG);
	//return mySPI_GetData(BME280_CTRL_MEAS_REG);
}

char BME280_IsMeasuring() {
  char output;
  output = BME280_GetStatus();
  return (output & BME280_STAT_REG_MEASURING__MSK);
}

char BME280_GetStatus() {
  return read_8bit_serial_data(BME280_STAT_REG);
}

/*
void BME280_ReadMeasurements() {

  //tmp_data[0] = BME280_PRESSURE_MSB_REG;
 char stri[50];



  adc_h = read_8bit_serial_data(BME280_HUMIDITY_LSB_REG) << 8;
  adc_h |= (unsigned long)read_8bit_serial_data(BME280_HUMIDITY_MSB_REG) << 8;

  adc_t  = (unsigned long)read_8bit_serial_data(BME280_TEMPERATURE_MSB_REG) << 12;
  adc_t |= (unsigned long)read_8bit_serial_data(BME280_TEMPERATURE_LSB_REG) << 4;
  adc_t |= (unsigned long)read_8bit_serial_data(BME280_TEMPERATURE_XLSB_REG) >> 4;


  adc_p  = (unsigned long)read_8bit_serial_data(BME280_PRESSURE_MSB_REG) << 12;
  adc_p |= (unsigned long)read_8bit_serial_data(BME280_PRESSURE_LSB_REG) << 4;
  adc_p |= (unsigned long)read_8bit_serial_data(BME280_PRESSURE_XLSB_REG) >> 4;


  sprintf(stri, "h= %lu t= %lu p= %lu",adc_h,adc_t,adc_p);//prepare string to be sent through uart
     		 						USART_puts(USART6,stri);
}
*/

long BME280_GetTemperature() {
  return BME280_Compensate_T();
}

long BME280_GetPressure() {
  return  BME280_Compensate_P();
}

/*static unsigned long BME280_Compensate_P() {
  long press1, press2;
  unsigned long P;
  char stri[50];

  press1 = ((long)t_fine) - (long)128000;
  press2 = (((press1>>2) * (press1>>2)) >> 11 ) * ((long)cal_param.dig_P6);
  press2 = press2 + ((press1*((long)cal_param.dig_P5))<<1);
  press2 = (press2>>2)+(((long)cal_param.dig_P4)<<16);
  press1 = (((cal_param.dig_P3 * (((press1>>2) * (press1>>2)) >> 13 )) >> 3) + ((((long)cal_param.dig_P2) * press1)>>1))>>18;
  press1 =((((32768+press1))*((long)cal_param.dig_P1))>>15);
  if (press1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  P = (((unsigned long)(((long)1048576)-adc_p)-(press2>>12)))*3125;
  if (P < 0x80000000) {
    P = (P << 1) / ((unsigned long)press1);
  } else {
    P = (P / (unsigned long)press1) * 2;
  }
  press1 = (((long)cal_param.dig_P9) * ((long)(((P>>3) * (P>>3))>>13)))>>12;
  press2 = (((long)(P>>2)) * ((long)cal_param.dig_P8))>>13;
  P = (unsigned long)((long)P + ((press1 + press2 + cal_param.dig_P7) >> 4));
  sprintf(stri, "pressure: %d Pa\n",P);//prepare string to be sent through uart
   		 	   					USART_puts(USART6,stri);
  return P;
  long var1, var2, p;
  char stri[50];



    var1 = ((long)t_fine) - 128000;
    var2 = var1 * var1 * (long)cal_param.dig_P6;
    var2 = var2 + ((var1*(long)cal_param.dig_P5)<<17);
    var2 = var2 + (((long)cal_param.dig_P4)<<35);
    var1 = ((var1 * var1 * (long)cal_param.dig_P3)>>8) +
      ((var1 * (long)cal_param.dig_P2)<<12);
    var1 = (((((long)1)<<47)+var1))*((long)cal_param.dig_P1)>>33;

    if (var1 == 0) {
      return 0;  // avoid exception caused by division by zero
    }
    p = 1048576 - adc_p;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = (((long)cal_param.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((long)cal_param.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((long)cal_param.dig_P7)<<4);

    sprintf(stri, "pressure: %d Pa\n",p);//prepare string to be sent through uart
       		 	   					USART_puts(USART6,stri);
    return p;




}*/

/*static long BME280_Compensate_T() {
  long temp1, temp2, T;

  temp1 = ((((adc_t>>3) -((long)cal_param.dig_T1<<1))) * ((long)cal_param.dig_T2)) >> 11;
  temp2 = (((((adc_t>>4) - ((long)cal_param.dig_T1)) * ((adc_t>>4) - ((long)cal_param.dig_T1))) >> 12) * ((long)cal_param.dig_T3)) >> 14;
  t_fine = temp1 + temp2;
  T = (t_fine * 5 + 128) >> 8;
  return T;
}*/

int32_t readTemperature(void)
{
//   long var1, var2;
   long T;
  // uint8_t adc_t1, adc_t2,adc_t3;
//  int32_t adc_t;
   char stri[50];
   DWORD size;
           UINT BytesWritten;

 // int32_t adc_T = read24(BME280_REGISTER_TEMPDATA);
  /*adc_t  = (uint32_t)(read_8bit_serial_data(BME280_TEMPERATURE_MSB_REG)<<12);

    adc_t |= (uint32_t)(read_8bit_serial_data(BME280_TEMPERATURE_LSB_REG)<<4);

    adc_t |= (uint32_t)(read_8bit_serial_data(BME280_TEMPERATURE_XLSB_REG)<<4);*/
   adc_t1  = read_8bit_serial_data(BME280_TEMPERATURE_MSB_REG);
  // adc_t<<8;
   adc_t2  = read_8bit_serial_data(BME280_TEMPERATURE_LSB_REG);
    //  adc_t<<8;
      adc_t3 = read_8bit_serial_data(BME280_TEMPERATURE_XLSB_REG);
     /* sprintf(stri, "%x %x %x\r\n",adc_t1,adc_t2,adc_t3);
      res = f_open(&fil,"temp.txt",FA_WRITE | FA_OPEN_ALWAYS);

                 size = (&fil)->fsize;

                   res = f_lseek(&fil,size);

          	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

                  f_close(&fil);*/

    adc_t=((uint32_t)adc_t1<<12) | ((uint32_t)adc_t2<<4) | ((adc_t3>>4) & 0x0F);
  //sprintf(stri, "temperature: %d %d %d ;; %d\n",adc_t1, adc_t2,adc_t3, adc_t);//prepare string to be sent through uart
  		 	  // 					USART_puts(USART6,stri);

  /* int32_t var1, var2;
  var1  = ((((adc_t>>3) -((int32_t)cal_param.dig_T1<<1))) * ((int32_t)cal_param.dig_T2)) >> 11;

  var2  = (((((adc_t>>4) - ((int32_t)cal_param.dig_T1)) * ((adc_t>>4) - ((int32_t)cal_param.dig_T1))) >> 12) * ((int32_t)cal_param.dig_T3)) >> 14;

  t_fine = var1 + var2;

  bartemp  = (t_fine * 5 + 128)>>8;*/

  /*sprintf(stri, "%li\r\n",bartemp);
        res = f_open(&fil,"temp.txt",FA_WRITE | FA_OPEN_ALWAYS);

                   size = (&fil)->fsize;

                     res = f_lseek(&fil,size);

            	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

                    f_close(&fil);*/
//  return T;
                  return 0;

}

int32_t readPressure(void) {

char stri[50];
int v_x1_u32, v_x2_u32;
unsigned int v_pressure_u32;
//uint8_t adc_p1, adc_p2, adc_p3;
DWORD size;
           UINT BytesWritten;

           readTemperature();
  //readTemperature(); // must be done first to get t_fine

  adc_p1  = read_8bit_serial_data(BME280_PRESSURE_MSB_REG);

      adc_p2 = read_8bit_serial_data(BME280_PRESSURE_LSB_REG);

      adc_p3 = read_8bit_serial_data(BME280_PRESSURE_XLSB_REG);

      adc_p=((uint32_t)adc_p1<<12) | ((uint32_t)adc_p2<<4) | ((adc_p3>>4) & 0x0F);

      /*int64_t var1, var2;
      	var1 = ((int64_t)t_fine) - 128000;
      	var2 = var1 * var1 * (int64_t)cal_param.dig_P6;
      	var2 = var2 + ((var1 * (int64_t)cal_param.dig_P5)<<17);
      	var2 = var2 + (((int64_t)cal_param.dig_P4)<<35);
      	var1 = ((var1 * var1 * (int64_t)cal_param.dig_P3)>>8) + ((var1 * (int64_t)cal_param.dig_P2)<<12);
      	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)cal_param.dig_P1)>>33;
      	if (var1 == 0)
      	{
      		return 0; // avoid exception caused by division by zero
      	}
      	barpress = 1048576 - adc_p;
      	barpress = (((barpress<<31) - var2)*3125)/var1;
      	var1 = (((int64_t)cal_param.dig_P9) * (barpress>>13) * (barpress>>13)) >> 25;
      	var2 = (((int64_t)cal_param.dig_P8) * barpress) >> 19;
      	barpress = ((barpress + var1 + var2) >> 8) + (((int64_t)cal_param.dig_P7)<<4);

      	barpress = barpress >> 8; // /256*/


      	/*int32_t var1, var2;
      	      	var1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
      	      	var2 = (((var1>>2) * (var1>>2))>>11) * ((int32_t)cal_param.dig_P6);
      	      	var2 = var2 + ((var1 * ((int32_t)cal_param.dig_P5))<<1);
      	      	var2 = (var2>>2) + (((int32_t)cal_param.dig_P4)<<16);
      	      	var1 = (((cal_param.dig_P3 * (((var1>>2) * (var1>>2))>>13))>>3)+ ((((int32_t)cal_param.dig_P2)*var1)>>1))>>18;
      	      	var1 = ((((32768+var1))*((int32_t)cal_param.dig_P1))>>15);
      	      	if (var1 == 0)
      	      	{
      	      		return 0; // avoid exception caused by division by zero
      	      	}
      	      	barpress = (((uint32_t)(((int32_t)1048576)-adc_p)-(var2>>12)))*3125;
      	      			if (barpress<0x80000000)
      	      			{
      	      				barpress= (barpress<<1) / ((uint32_t)var1);
      	      			}
      	      			else
      	      			{
      	      			barpress= (barpress / (uint32_t)var1) *2;
      	      			}

      	      	var1 = (((int32_t)cal_param.dig_P9) * ((int32_t)(((barpress>>3)*(barpress>>3))>>13)))>>12;
      	      	var2 = (((int32_t)(barpress>>2)) * ((int32_t)cal_param.dig_P8))>>13;
      	      	barpress = (uint32_t)((int32_t)barpress + ((var1 + var2 + cal_param.dig_P7)>>4));

      	      	//barpress = barpress >> 8; // /256

      //int32_t adc_p=((uint32_t)adc_t1<<12) + ((uint32_t)adc_t2<<4) + ((adc_t3>>4) & 0x0F);
      sprintf(stri, "%d\r\n",barpress);
            res = f_open(&fil,"press.txt",FA_WRITE | FA_OPEN_ALWAYS);

                       size = (&fil)->fsize;

                         res = f_lseek(&fil,size);

                	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

                        f_close(&fil);
*/

  return 0;
}

int32_t readHumidity(void) {

char stri[50];
int v_x1_u32, v_x2_u32;
unsigned int v_pressure_u32;
//uint8_t adc_p1, adc_p2, adc_p3;
DWORD size;
           UINT BytesWritten;
  //readTemperature(); // must be done first to get t_fine


 // adc_h1  = read_8bit_serial_data(0xFD);

    //  adc_h2 = read_8bit_serial_data(0xFE);

           s32 var1;

      adc_h=(s32)((((u32)(read_8bit_serial_data(0xFD)))<< 8)|((u32)(read_8bit_serial_data(0xFE))));

      var1 = (t_fine - ((s32)76800));
      	/* calculate var1*/
      var1 = (((((adc_h<< 14) -(((s32)cal_param.dig_H4)<< 20) -(((s32)cal_param.dig_H5) * var1)) +((s32)16384)) >> 15)* (((((((var1 *((s32)cal_param.dig_H6))>> 10) *(((var1 * ((s32)cal_param.dig_H3))>> 11) + ((s32)32768)))>> 10) + ((s32)2097152)) *((s32)cal_param.dig_H2) + 8192) >> 14));

      var1 = (var1 - (((((var1>> 15) *(var1 >> 15))>> 7) *((s32)cal_param.dig_H1))>> 4));

      var1 = (var1 < 0 ? 0 : var1);

      var1 = (var1 > 419430400 ? 419430400 : var1);

      barhum = (u32)(var1 >> 12); //divide it by 1024 to have %RH

     /* sprintf(stri, "%d\r\n",barhum);
              res = f_open(&fil,"hum.txt",FA_WRITE | FA_OPEN_ALWAYS);

                         size = (&fil)->fsize;

                           res = f_lseek(&fil,size);

                  	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

                          f_close(&fil);*/
      return 0;

}


int32_t readT(void)
{
//   long var1, var2;
   long T;
  // uint8_t adc_t1, adc_t2,adc_t3;
//  int32_t adc_t;
   char stri[50];
   DWORD size;
           UINT BytesWritten;

 // int32_t adc_T = read24(BME280_REGISTER_TEMPDATA);
  /*adc_t  = (uint32_t)(read_8bit_serial_data(BME280_TEMPERATURE_MSB_REG)<<12);

    adc_t |= (uint32_t)(read_8bit_serial_data(BME280_TEMPERATURE_LSB_REG)<<4);

    adc_t |= (uint32_t)(read_8bit_serial_data(BME280_TEMPERATURE_XLSB_REG)<<4);*/
  // adc_t1  = read_8bit_serial_data(BME280_TEMPERATURE_MSB_REG);
  // adc_t<<8;
  // adc_t2  = read_8bit_serial_data(BME280_TEMPERATURE_LSB_REG);
    //  adc_t<<8;
  //    adc_t3 = read_8bit_serial_data(BME280_TEMPERATURE_XLSB_REG);
     /* sprintf(stri, "%x %x %x\r\n",adc_t1,adc_t2,adc_t3);
      res = f_open(&fil,"temp.txt",FA_WRITE | FA_OPEN_ALWAYS);

                 size = (&fil)->fsize;

                   res = f_lseek(&fil,size);

          	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

                  f_close(&fil);*/

   // adc_t=((uint32_t)adc_t1<<12) + ((uint32_t)adc_t2<<4) + ((adc_t3>>4) & 0x0F);


    adc_t = (s32)((((u32) (read_8bit_serial_data(BME280_TEMPERATURE_MSB_REG)))<< 12) |(((u32)(read_8bit_serial_data(BME280_TEMPERATURE_LSB_REG)))<< 4)| ((u32)read_8bit_serial_data(BME280_TEMPERATURE_XLSB_REG)>> 4));
  //sprintf(stri, "temperature: %d %d %d ;; %d\n",adc_t1, adc_t2,adc_t3, adc_t);//prepare string to be sent through uart
  		 	  // 					USART_puts(USART6,stri);

   s32 var1, var2;
  var1  = ((((adc_t>>3) -((s32)cal_param.dig_T1<<1))) * ((s32)cal_param.dig_T2)) >> 11;

  var2  = (((((adc_t>>4) - ((s32)cal_param.dig_T1)) * ((adc_t>>4) - ((s32)cal_param.dig_T1))) >> 12) * ((s32)cal_param.dig_T3)) >> 14;

  t_fine = var1 + var2;

  bartemp  = (t_fine * 5 + 128)>>8;

  /*sprintf(stri, "%d\r\n",bartemp);
        res = f_open(&fil,"temp.txt",FA_WRITE | FA_OPEN_ALWAYS);

                   size = (&fil)->fsize;

                     res = f_lseek(&fil,size);

            	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

                    f_close(&fil);*/
//  return T;
                  return 0;

}

int32_t readP(void) {

char stri[50];
int v_x1_u32, v_x2_u32;
unsigned int v_pressure_u32;
//uint8_t adc_p1, adc_p2, adc_p3;
DWORD size;
           UINT BytesWritten;

           readT();
  //readTemperature(); // must be done first to get t_fine

 // adc_p1  = read_8bit_serial_data(BME280_PRESSURE_MSB_REG);

     // adc_p2 = read_8bit_serial_data(BME280_PRESSURE_LSB_REG);

   //   adc_p3 = read_8bit_serial_data(BME280_PRESSURE_XLSB_REG);

   //   adc_p=((uint32_t)adc_p1<<12) + ((uint32_t)adc_p2<<4) + ((adc_p3>>4) & 0x0F);

     adc_p = (s32)((((u32)(read_8bit_serial_data(BME280_PRESSURE_MSB_REG)))<< 12) |(((u32)(read_8bit_serial_data(BME280_PRESSURE_LSB_REG)))<< 4) |((u32)read_8bit_serial_data(BME280_PRESSURE_XLSB_REG) >>4));

     /* int64_t var1, var2;
      	var1 = ((int64_t)t_fine) - 128000;
      	var2 = var1 * var1 * (int64_t)cal_param.dig_P6;
      	var2 = var2 + ((var1 * (int64_t)cal_param.dig_P5)<<17);
      	var2 = var2 + (((int64_t)cal_param.dig_P4)<<35);
      	var1 = ((var1 * var1 * (int64_t)cal_param.dig_P3)>>8) + ((var1 * (int64_t)cal_param.dig_P2)<<12);
      	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)cal_param.dig_P1)>>33;
      	if (var1 == 0)
      	{
      		return 0; // avoid exception caused by division by zero
      	}
      	barpress = 1048576 - adc_p;
      	barpress = (((barpress<<31) - var2)*3125)/var1;
      	var1 = (((int64_t)cal_param.dig_P9) * (barpress>>13) * (barpress>>13)) >> 25;
      	var2 = (((int64_t)cal_param.dig_P8) * barpress) >> 19;
      	barpress = ((barpress + var1 + var2) >> 8) + (((int64_t)cal_param.dig_P7)<<4);

      	barpress = barpress >> 8; // /256*/


      	s32 var1, var2;
      	      	var1 = (((s32)t_fine)>>1) - (s32)64000;
      	      	var2 = (((var1>>2) * (var1>>2))>>11) * ((s32)cal_param.dig_P6);
      	      	var2 = var2 + ((var1 * ((s32)cal_param.dig_P5))<<1);
      	      	var2 = (var2>>2) + (((s32)cal_param.dig_P4)<<16);
      	      	var1 = (((cal_param.dig_P3 * (((var1>>2) * (var1>>2))>>13))>>3)+ ((((s32)cal_param.dig_P2)*var1)>>1))>>18;
      	      	var1 = ((((32768+var1))*((s32)cal_param.dig_P1))>>15);
      	      	/*if (var1 == 0)
      	      	{
      	      		return 0; // avoid exception caused by division by zero
      	      	}*/
      	      	barpress = (((u32)(((s32)1048576)-adc_p)-(var2>>12)))*3125;
      	      			if (barpress<0x80000000)
      	      			{
      	      				if (var1 != 0)
      	      				barpress =(barpress << 1)/((u32)var1);
      	      					else
      	      						return 0;
      	      			}
      	      			else
      	      			{
      	      			if (var1 != 0)
      	      			barpress = (barpress/(u32)var1) * 2;
      	      					else
      	      			return 0;
      	      			}

      	      	var1 = (((s32)cal_param.dig_P9) * ((s32)(((barpress>>3)*(barpress>>3))>>13)))>>12;
      	      	var2 = (((s32)(barpress>>2)) * ((s32)cal_param.dig_P8))>>13;
      	      	barpress = (u32)((s32)barpress + ((var1 + var2 + cal_param.dig_P7)>>4));

      	      //	barpress = barpress >> 8; // /256

      //int32_t adc_p=((uint32_t)adc_t1<<12) + ((uint32_t)adc_t2<<4) + ((adc_t3>>4) & 0x0F);
     /* sprintf(stri, "%d\r\n",barpress); //this has to be divided by 100
            res = f_open(&fil,"press.txt",FA_WRITE | FA_OPEN_ALWAYS);

                       size = (&fil)->fsize;

                         res = f_lseek(&fil,size);

                	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

                        f_close(&fil);*/


  return 0;
}
