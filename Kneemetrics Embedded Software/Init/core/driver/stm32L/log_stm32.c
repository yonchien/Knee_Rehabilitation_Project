/*
 $License:
    Copyright (C) 2010 InvenSense Corporation, All Rights Reserved.
 $
 */
/*******************************************************************************
 * $Id: $
 *******************************************************************************/

/**
 *  @defgroup MSP430_System_Layer MSP430 System Layer
 *  @brief  MSP430 System Layer APIs.
 *          To interface with any platform, eMPL needs access to various
 *          system layer functions.
 *
 *  @{
 *      @file   log_msp430.c
 *      @brief  Logging facility for the TI MSP430.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "uart.h"
#include "packet.h"
#include "log.h"
#include "stm32f4xx.h"
#include "uart.h"
#include "ff.h"
#include "diskio.h"
FRESULT res;
FILINFO fno;
FIL fil;
DIR dir;
FATFS fs32[1];
char* path;
char *fn;   /* This function is assuming non-Unicode cfg. */

#define BUF_SIZE        (256)
#define PACKET_LENGTH   (23)

#define PACKET_DEBUG    (1)
#define PACKET_QUAT     (2)
#define PACKET_DATA     (3)

/**
 *  @brief      Prints a variable argument log message.
 *  USB output will be formatted as follows:\n
 *  packet[0]       = $\n
 *  packet[1]       = packet type (1: debug, 2: quat, 3: data)\n
 *  packet[2]       = \n for debug packets: log priority\n
 *                    for quaternion packets: unused\n
 *                    for data packets: packet content (accel, gyro, etc)\n
 *  packet[3-20]    = data\n
 *  packet[21]      = \\r\n
 *  packet[22]      = \\n
 *  @param[in]  priority    Log priority (based on Android).
 *  @param[in]  tag         File specific string.
 *  @param[in]  fmt         String of text with optional format tags.
 *
 *  @return     0 if successful.
 */
int _MLPrintLog (int priority, const char* tag, const char* fmt, ...)
{
    va_list args;
    int length, ii, i;
    char buf[BUF_SIZE], out[PACKET_LENGTH], this_length;
    char stri[30];

    /* This can be modified to exit for unsupported priorities. */
    switch (priority) {
    case MPL_LOG_UNKNOWN:
    case MPL_LOG_DEFAULT:
    case MPL_LOG_VERBOSE:
    case MPL_LOG_DEBUG:
    case MPL_LOG_INFO:
    case MPL_LOG_WARN:
    case MPL_LOG_ERROR:
    case MPL_LOG_SILENT:
        break;
    default:
        return 0;
    }

    va_start(args, fmt);

    length = vsprintf(buf, fmt, args);
    if (length <= 0) {
        va_end(args);
        return length;
    }

    memset(out, 0, PACKET_LENGTH);
    out[0] = '$';
    out[1] = PACKET_DEBUG;
    out[2] = priority;
    out[21] = '\r';
    out[22] = '\n';
    for (ii = 0; ii < length; ii += (PACKET_LENGTH-5)) {
#define min(a,b) ((a < b) ? a : b)
        this_length = min(length-ii, PACKET_LENGTH-5);
        memset(out+3, 0, 18);
        memcpy(out+3, buf+ii, this_length);


   //     sprintf(stri, out);//prepare string to be sent through uart
   //      	USART_puts(USART6,stri);
    //    for (i=0; i<PACKET_LENGTH; i++) {
   //       fputc(out[i]);
   //     }
    }
    
            
    va_end(args);

    return 0;
}

void eMPL_send_quat(long *quat)
{
    char out[PACKET_LENGTH];
    int i;
    char stri[200];

	UINT BytesWritten;
           	char string[100];
           	char string2[100];
           	 DWORD size;

    if (!quat)
        return;
    memset(out, 0, PACKET_LENGTH);
    out[0] = '$';
    out[1] = PACKET_QUAT;
    out[3] = (quat[0] >> 24);
    out[4] = (quat[0] >> 16);
    out[5] = (quat[0] >> 8);
    out[6] = quat[0];
    out[7] = (quat[1] >> 24);
    out[8] = (quat[1] >> 16);
    out[9] = (quat[1] >> 8);
    out[10] = quat[1];
    out[11] = (quat[2] >> 24);
    out[12] = (quat[2] >> 16);
    out[13] = (quat[2] >> 8);
    out[14] = quat[2];
    out[15] = (quat[3] >> 24);
    out[16] = (quat[3] >> 16);
    out[17] = (quat[3] >> 8);
    out[18] = quat[3];
    out[21] = '\r';
    out[22] = '\n';

   sprintf(stri, "%ld; %ld; %ld; %ld\r\n",((quat[0] >> 24)+(quat[0] >> 16)+(quat[0] >> 8)+quat[0]),((quat[1] >> 24)+(quat[1] >> 16)+(quat[1] >> 8)+quat[1]),((quat[2] >> 24)+(quat[2] >> 16)+(quat[2] >> 8)+quat[2]),((quat[3] >> 24)+(quat[3] >> 16)+(quat[3] >> 8)+quat[3]));//prepare string to be sent through uart
  //  sprintf(stri, "%ld; %ld; %ld; %ld\r\n",quat[0],quat[1],quat[2],quat[3]);
    //  sprintf(stri,out);
  //  sprintf(stri, out);
           	//sleep(200);


                        	/*res = f_open(&fil,"quat.txt",FA_WRITE | FA_OPEN_ALWAYS);

                        	        size = (&fil)->fsize;

                        	        res = f_lseek(&fil,size);

                        	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

                        	        f_close(&fil);*/

                        	       // sprintf(string2, "gyro = %ld %ld %ld\r\n",gyro[0],gyro[1],gyro[2]);//prepare string to be sent through uart


 //   sprintf(stri, out);//prepare string to be sent through uart
 //     	USART_puts(USART6,stri);
 //   for (i=0; i<PACKET_LENGTH; i++) {
 //     fputc(out[i]);
 //   }
                        	  //      USART_puts(USART6,stri);
}

void eMPL_send_data(unsigned char type, long *data)
{
    char out[PACKET_LENGTH];
    int i;
  char stri[200];
  char stri2[200];
  long gyrodata[3];
  UINT BytesWritten;
             	char string[100];
             	char string2[100];
             	 DWORD size;


    if (!data)
        return;
    memset(out, 0, PACKET_LENGTH);
    out[0] = '$';
    out[1] = PACKET_DATA;
    out[2] = type;
    out[21] = '\r';
    out[22] = '\n';
    switch (type) {
    /* Two bytes per-element. */
    case PACKET_DATA_ROT:
        out[3] = (char)(data[0] >> 24);
        out[4] = (char)(data[0] >> 16);
        out[5] = (char)(data[1] >> 24);
        out[6] = (char)(data[1] >> 16);
        out[7] = (char)(data[2] >> 24);
        out[8] = (char)(data[2] >> 16);
        out[9] = (char)(data[3] >> 24);
        out[10] = (char)(data[3] >> 16);
        out[11] = (char)(data[4] >> 24);
        out[12] = (char)(data[4] >> 16);
        out[13] = (char)(data[5] >> 24);
        out[14] = (char)(data[5] >> 16);
        out[15] = (char)(data[6] >> 24);
        out[16] = (char)(data[6] >> 16);
        out[17] = (char)(data[7] >> 24);
        out[18] = (char)(data[7] >> 16);
        out[19] = (char)(data[8] >> 24);
        out[20] = (char)(data[8] >> 16);
        break;
    /* Four bytes per-element. */
    /* Four elements. */
    case PACKET_DATA_QUAT:
        out[15] = (char)(data[3] >> 24);
        out[16] = (char)(data[3] >> 16);
        out[17] = (char)(data[3] >> 8);
        out[18] = (char)data[3];
    /* Three elements. */
    case PACKET_DATA_ACCEL:
    	 sprintf(stri, "%hi  %hi  %hi  %hi  %li  %li  %li  %li  %li\r\n",data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8]);
    	        	    	    	    	  //  sprintf(stri, out);//prepare string to be sent through uart
    	        	    	    	    	      //   	USART_puts(USART6,out);
    	        	    	    	    	    res = f_open(&fil,"data.txt",FA_WRITE | FA_OPEN_ALWAYS);

    	        	    	    	    	                            	               	        size = (&fil)->fsize;

    	        	    	    	    	                            	               	        res = f_lseek(&fil,size);

    	        	    	    	    	                            	               	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

    	        	    	    	    	                            	               	        f_close(&fil);

    case PACKET_DATA_GYRO:
    	/* sprintf(stri2, "%li  %li  %li  %li  %li  %li  %li  %li  %li\r\n",data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8]);
    	    	        	    	    	    	  //  sprintf(stri, out);//prepare string to be sent through uart
    	    	        	    	    	    	      //   	USART_puts(USART6,out);
    	    	        	    	    	    	    res = f_open(&fil,"data2.txt",FA_WRITE | FA_OPEN_ALWAYS);

    	    	        	    	    	    	                            	               	        size = (&fil)->fsize;

    	    	        	    	    	    	                            	               	        res = f_lseek(&fil,size);

    	    	        	    	    	    	                            	               	        res = f_write(&fil,stri2, strlen(stri2),&BytesWritten);

    	    	        	    	    	    	                            	               	        f_close(&fil);*/


    case PACKET_DATA_COMPASS:
    case PACKET_DATA_EULER:
        out[3] = (char)(data[0] >> 24);
        out[4] = (char)(data[0] >> 16);
        out[5] = (char)(data[0] >> 8);
        out[6] = (char)data[0];
        out[7] = (char)(data[1] >> 24);
        out[8] = (char)(data[1] >> 16);
        out[9] = (char)(data[1] >> 8);
        out[10] = (char)data[1];
        out[11] = (char)(data[2] >> 24);
        out[12] = (char)(data[2] >> 16);
        out[13] = (char)(data[2] >> 8);
        out[14] = (char)data[2];
        break;
    case PACKET_DATA_HEADING:
        out[3] = (char)(data[0] >> 24);
        out[4] = (char)(data[0] >> 16);
        out[5] = (char)(data[0] >> 8);
        out[6] = (char)data[0];
        break;
    default:
        return;
    }




  //  gyrodata[0] = four_bytes(data[3],data[4],data[5],data[6]) * 1.0 / (1<<16);
  //  gyrodata[1] = four_bytes(data[7],data[8],data[9],data[10]) * 1.0 / (1<<16);
//	gyrodata[2] = four_bytes(data[11],data[12],data[13],data[14]) * 1.0 / (1<<16);

  //  for (i=0; i<PACKET_LENGTH; i++) {
   //   fputc(out[i]);
    //}
    //USART_puts(USART6,stri);
}
void eMPL_send_data2(unsigned char type, long *data, unsigned long *timestamp)
{
    char out[PACKET_LENGTH];
    int i;
  char stri[30];
  long gyrodata[3];
  UINT BytesWritten;
             	char string[100];
             	char string2[100];
             	 DWORD size;


    if (!data)
        return;
    memset(out, 0, PACKET_LENGTH);
    out[0] = '$';
    out[1] = PACKET_DATA;
    out[2] = type;
    out[21] = '\r';
    out[22] = '\n';
    switch (type) {
    /* Two bytes per-element. */
    case PACKET_DATA_ROT:
        out[3] = (char)(data[0] >> 24);
        out[4] = (char)(data[0] >> 16);
        out[5] = (char)(data[1] >> 24);
        out[6] = (char)(data[1] >> 16);
        out[7] = (char)(data[2] >> 24);
        out[8] = (char)(data[2] >> 16);
        out[9] = (char)(data[3] >> 24);
        out[10] = (char)(data[3] >> 16);
        out[11] = (char)(data[4] >> 24);
        out[12] = (char)(data[4] >> 16);
        out[13] = (char)(data[5] >> 24);
        out[14] = (char)(data[5] >> 16);
        out[15] = (char)(data[6] >> 24);
        out[16] = (char)(data[6] >> 16);
        out[17] = (char)(data[7] >> 24);
        out[18] = (char)(data[7] >> 16);
        out[19] = (char)(data[8] >> 24);
        out[20] = (char)(data[8] >> 16);
        break;
    /* Four bytes per-element. */
    /* Four elements. */
    case PACKET_DATA_QUAT:
        out[15] = (char)(data[3] >> 24);
        out[16] = (char)(data[3] >> 16);
        out[17] = (char)(data[3] >> 8);
        out[18] = (char)data[3];
    /* Three elements. */
    case PACKET_DATA_ACCEL:

    case PACKET_DATA_GYRO:
    	sprintf(stri, "%ld   %ld\r\n",data[3]);
    	    	    	    	  //  sprintf(stri, out);//prepare string to be sent through uart
    	    	    	    	      //   	USART_puts(USART6,out);
    	    	    	    	    res = f_open(&fil,"mag2.txt",FA_WRITE | FA_OPEN_ALWAYS);

    	    	    	    	                            	               	        size = (&fil)->fsize;

    	    	    	    	                            	               	        res = f_lseek(&fil,size);

    	    	    	    	                            	               	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

    	    	    	    	                            	               	        f_close(&fil);

    case PACKET_DATA_COMPASS:
    	sprintf(stri, "mag_x=%ld   %ld\r\n",(data[3]*(1<<24))+(data[4]*(1<<16))+(data[5]*(1<<8))+data[6],timestamp);
    	    	    	  //  sprintf(stri, out);//prepare string to be sent through uart
    	    	    	      //   	USART_puts(USART6,out);
    	    	    	    res = f_open(&fil,"mag2.txt",FA_WRITE | FA_OPEN_ALWAYS);

    	    	    	                            	               	        size = (&fil)->fsize;

    	    	    	                            	               	        res = f_lseek(&fil,size);

    	    	    	                            	               	        res = f_write(&fil,stri, strlen(stri),&BytesWritten);

    	    	    	                            	               	        f_close(&fil);
    case PACKET_DATA_EULER:
        out[3] = (char)(data[0] >> 24);
        out[4] = (char)(data[0] >> 16);
        out[5] = (char)(data[0] >> 8);
        out[6] = (char)data[0];
        out[7] = (char)(data[1] >> 24);
        out[8] = (char)(data[1] >> 16);
        out[9] = (char)(data[1] >> 8);
        out[10] = (char)data[1];
        out[11] = (char)(data[2] >> 24);
        out[12] = (char)(data[2] >> 16);
        out[13] = (char)(data[2] >> 8);
        out[14] = (char)data[2];
        break;
    case PACKET_DATA_HEADING:
        out[3] = (char)(data[0] >> 24);
        out[4] = (char)(data[0] >> 16);
        out[5] = (char)(data[0] >> 8);
        out[6] = (char)data[0];
        break;
    default:
        return;
    }

  //  gyrodata[0] = four_bytes(data[3],data[4],data[5],data[6]) * 1.0 / (1<<16);
  //  gyrodata[1] = four_bytes(data[7],data[8],data[9],data[10]) * 1.0 / (1<<16);
//	gyrodata[2] = four_bytes(data[11],data[12],data[13],data[14]) * 1.0 / (1<<16);

  //  for (i=0; i<PACKET_LENGTH; i++) {
   //   fputc(out[i]);
    //}
}
/**
 * @}
**/


