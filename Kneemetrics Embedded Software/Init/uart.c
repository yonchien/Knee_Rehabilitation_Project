#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "uart.h"
#include "delay.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
#include "leds.h"


//#define UART6_Tx_Pin 				GPIO_Pin_14
//#define UART6_Tx_Port 				GPIOG
//#define UART6_Tx_GPIO_Port_Clock  	RCC_AHB1Periph_GPIOG
//#define UART6_Tx_Periph_Clock 		RCC_APB2Periph_USART6
//#define UART6_Tx_PinSource			GPIO_PinSource14
//#define UART6_Tx_AF					GPIO_AF_USART6

//#define UART6_Rx_Pin 				GPIO_Pin_9
//#define UART6_Rx_Port 				GPIOG
//#define UART6_GPIO_Port_Clock  		RCC_AHB1Periph_GPIOG
//#define UART6_Rx_Port_Clock 		RCC_APB2Periph_USART6
//#define UART6_Rx_PinSource			GPIO_PinSource9
//#define UART6_Rx_AF					GPIO_AF_USART6

void InitExtUART()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	 NVIC_InitTypeDef NVIC_InitStructure;


	 /* enable peripheral clock for USART2 */
	 	//RCC_APB2PeriphClockCmd(UART6_Tx_Periph_Clock, ENABLE);
	 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	/* GPIOA clock enable */
//	RCC_AHB1PeriphClockCmd(UART6_Tx_GPIO_Port_Clock, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);





	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_9; // PC.6 USART6_TX, PC.7 USART6_RX
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //NOPULL
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	  /* Configure PC6 as alternate function  */
	  // GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	 //  GPIO_InitStructure.GPIO_Pin   = UART6_Tx_Pin;
	   GPIO_Init(GPIOG, &GPIO_InitStructure);

	   /* Configure PC7 as alternate function  */
	//   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	//   GPIO_InitStructure.GPIO_Pin   = UART6_Rx_Pin;
	//   GPIO_Init(GPIOG, &GPIO_InitStructure);
	  //GPIO_Init(GPIOG, &GPIO_InitStructure);

	  /* Connect USART pins to AF */
	  GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);
	  GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6);

	/* GPIOA Configuration:  USART2 TX on PD5 */
//	GPIO_InitStructure.GPIO_Pin = UART6_Tx_Pin | UART6_Rx_Pin;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
//	GPIO_Init(UART6_Tx_Port, &GPIO_InitStructure);

	/* Connect USART6 pins to AF2 */
	// TX = PG14  RX = PG9
//	GPIO_PinAFConfig(UART6_Tx_Port, UART6_Tx_PinSource, UART6_Tx_AF);
//	GPIO_PinAFConfig(UART6_Rx_Port, UART6_Rx_PinSource, UART6_Rx_AF);


	  USART_InitStructure.USART_BaudRate = 115200;
	    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	    USART_InitStructure.USART_StopBits = USART_StopBits_1;
	    USART_InitStructure.USART_Parity = USART_Parity_No;
	    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

	    USART_Init(USART6, &USART_InitStructure);

	//    USART_Cmd(USART6, ENABLE);

	//USART_InitStructure.USART_BaudRate = 115200;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//USART_InitStructure.USART_Parity = USART_Parity_No;
	//USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	//USART_Init(USART6, &USART_InitStructure);


	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

	//Now enable NVIC for UART6
	NVIC_InitTypeDef UART6_NVIC_Init;
	UART6_NVIC_Init.NVIC_IRQChannel = USART6_IRQn;
	UART6_NVIC_Init.NVIC_IRQChannelPreemptionPriority = 0;
	UART6_NVIC_Init.NVIC_IRQChannelSubPriority = 0;
	UART6_NVIC_Init.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&UART6_NVIC_Init);


	USART_Cmd(USART6, ENABLE); // enable USART6
}

void InitBLEUART()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure2;
	USART_InitTypeDef USART_InitStructure;
	 NVIC_InitTypeDef NVIC_InitStructure;


	 /* enable peripheral clock for USART2 */
	 	//RCC_APB2PeriphClockCmd(UART6_Tx_Periph_Clock, ENABLE);
	 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* GPIOB clock enable */
//	RCC_AHB1PeriphClockCmd(UART6_Tx_GPIO_Port_Clock, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);





	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	  GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_7 ;
	 	  GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_AF;
	 	  GPIO_InitStructure2.GPIO_OType = GPIO_OType_PP;
	 	  GPIO_InitStructure2.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 	  GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_50MHz;

	  /* Configure PC6 as alternate function  */
	  // GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	 //  GPIO_InitStructure.GPIO_Pin   = UART6_Tx_Pin;
	   GPIO_Init(GPIOB, &GPIO_InitStructure);
	   GPIO_Init(GPIOB, &GPIO_InitStructure2);

	   /* Configure PC7 as alternate function  */
	//   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	//   GPIO_InitStructure.GPIO_Pin   = UART6_Rx_Pin;
	//   GPIO_Init(GPIOG, &GPIO_InitStructure);
	  //GPIO_Init(GPIOG, &GPIO_InitStructure);

	  /* Connect USART pins to AF */
	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

	/* GPIOA Configuration:  USART2 TX on PD5 */
//	GPIO_InitStructure.GPIO_Pin = UART6_Tx_Pin | UART6_Rx_Pin;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
//	GPIO_Init(UART6_Tx_Port, &GPIO_InitStructure);

	/* Connect USART6 pins to AF2 */
	// TX = PG14  RX = PG9
//	GPIO_PinAFConfig(UART6_Tx_Port, UART6_Tx_PinSource, UART6_Tx_AF);
//	GPIO_PinAFConfig(UART6_Rx_Port, UART6_Rx_PinSource, UART6_Rx_AF);


	  USART_InitStructure.USART_BaudRate = 115200;
	    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	    USART_InitStructure.USART_StopBits = USART_StopBits_1;
	    USART_InitStructure.USART_Parity = USART_Parity_No;
	    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

	    USART_Init(USART1, &USART_InitStructure);

	//    USART_Cmd(USART6, ENABLE);

	//USART_InitStructure.USART_BaudRate = 115200;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//USART_InitStructure.USART_Parity = USART_Parity_No;
	//USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	//USART_Init(USART6, &USART_InitStructure);


	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	//Now enable NVIC for UART6
	NVIC_InitTypeDef UART1_NVIC_Init;
	UART1_NVIC_Init.NVIC_IRQChannel = USART1_IRQn;
	UART1_NVIC_Init.NVIC_IRQChannelPreemptionPriority = 0;
	UART1_NVIC_Init.NVIC_IRQChannelSubPriority = 0;
	UART1_NVIC_Init.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&UART1_NVIC_Init);


	USART_Cmd(USART1, ENABLE); // enable USART6
}


void InitBLEUART_PROG()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure2;
	USART_InitTypeDef USART_InitStructure;
	 NVIC_InitTypeDef NVIC_InitStructure;


	 /* enable peripheral clock for USART2 */
	 	//RCC_APB2PeriphClockCmd(UART6_Tx_Periph_Clock, ENABLE);
	 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* GPIOB clock enable */
//	RCC_AHB1PeriphClockCmd(UART6_Tx_GPIO_Port_Clock, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);





	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	  GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_7 ;
	 	  GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_AF;
	 	  GPIO_InitStructure2.GPIO_OType = GPIO_OType_PP;
	 	  GPIO_InitStructure2.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 	  GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_50MHz;

	  /* Configure PC6 as alternate function  */
	  // GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	 //  GPIO_InitStructure.GPIO_Pin   = UART6_Tx_Pin;
	   GPIO_Init(GPIOB, &GPIO_InitStructure);
	   GPIO_Init(GPIOB, &GPIO_InitStructure2);

	   /* Configure PC7 as alternate function  */
	//   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	//   GPIO_InitStructure.GPIO_Pin   = UART6_Rx_Pin;
	//   GPIO_Init(GPIOG, &GPIO_InitStructure);
	  //GPIO_Init(GPIOG, &GPIO_InitStructure);

	  /* Connect USART pins to AF */
	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

	/* GPIOA Configuration:  USART2 TX on PD5 */
//	GPIO_InitStructure.GPIO_Pin = UART6_Tx_Pin | UART6_Rx_Pin;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
//	GPIO_Init(UART6_Tx_Port, &GPIO_InitStructure);

	/* Connect USART6 pins to AF2 */
	// TX = PG14  RX = PG9
//	GPIO_PinAFConfig(UART6_Tx_Port, UART6_Tx_PinSource, UART6_Tx_AF);
//	GPIO_PinAFConfig(UART6_Rx_Port, UART6_Rx_PinSource, UART6_Rx_AF);


	  USART_InitStructure.USART_BaudRate = 115200;
	    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	    USART_InitStructure.USART_StopBits = USART_StopBits_1;
	    USART_InitStructure.USART_Parity = USART_Parity_No;
	    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

	    USART_Init(USART1, &USART_InitStructure);

	//    USART_Cmd(USART6, ENABLE);

	//USART_InitStructure.USART_BaudRate = 115200;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//USART_InitStructure.USART_StopBits = USART_StopBits_1;
	//USART_InitStructure.USART_Parity = USART_Parity_No;
	//USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	//USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	//USART_Init(USART6, &USART_InitStructure);


	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	//Now enable NVIC for UART6
	NVIC_InitTypeDef UART1_NVIC_Init;
	UART1_NVIC_Init.NVIC_IRQChannel = USART1_IRQn;
	UART1_NVIC_Init.NVIC_IRQChannelPreemptionPriority = 0;
	UART1_NVIC_Init.NVIC_IRQChannelSubPriority = 0;
	UART1_NVIC_Init.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&UART1_NVIC_Init);


	USART_Cmd(USART1, ENABLE); // enable USART6
}


void TX_ExtUART (char* string){
	int  i;
	int len = strlen(string);
	//char stringa="d";

	//int len = strlen(stringa);
	RxCount = 0;//Needed to start string on the first character

//	printf("hello\n");
	//while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
	for(i=0; i<len; i++)
	  {

		while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
		USART_SendData(USART6, string[i]);
		//Loop until the end of transmission

	//	SwitchOnLED(LED_YEL);

	  }
		//SwitchOffLED(LED_GRE);
}


void USART_puts(USART_TypeDef* USARTx, volatile char *s){

        while(*s){
                // wait until data register is empty
                while( !(USARTx->SR & 0x00000040) );
                USART_SendData(USARTx, *s);
                *s++;
        }
}

void USART1_IRQHandler(void){

	//SwitchOnLED(LED_YEL);
	// check if the USART1 receive interrupt flag was set
	/*if( USART_GetITStatus(USART1, USART_IT_RXNE) ){

		static uint8_t cnt = 0; // this counter is used to determine the string length
		char t = USART1->DR; // the character from the USART1 data register is saved in t
		char received_string[501];
		// check if the received character is not the LF character (used to determine end of string)
	//	 * or the if the maximum string length has been been reached


		if( (t != '\n') && (cnt < 500) ){
			received_string[cnt] = t;
			cnt++;
		}
		else{ // otherwise reset the character counter and print the received string
			cnt = 0;
			USART_puts(USART6, received_string);
		}
	}
*/
	//uint8_t read;

	//		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE)){
			//read = USART_ReceiveData(USART1);
			//text_test[RXCount_test++] = read;
			USART_SendData(USART6, USART_ReceiveData(USART1));

			//USART_puts(USART6, read);
		//	}
	//		USART_ClearFlag(USART1, USART_FLAG_RXNE);

}
void USART6_IRQHandler(void){
	//SwitchOnLED(LED_GRE);

	// check if the USART1 receive interrupt flag was set
/*	if( USART_GetITStatus(USART6, USART_IT_RXNE) ){

		static uint8_t cnt = 0; // this counter is used to determine the string length
		char t = USART1->DR; // the character from the USART1 data register is saved in t
		char received_string[301];
	//	 check if the received character is not the LF character (used to determine end of string)
	//	 * or the if the maximum string length has been been reached


		if( (t != '\n') && (cnt < 300) ){
			received_string[cnt] = t;
			cnt++;
		}
		else{ // otherwise reset the character counter and print the received string
			cnt = 0;
			USART_puts(USART1, received_string);
		}
	}*/


	//uint8_t read;

//		while (USART_GetFlagStatus(USART6, USART_FLAG_RXNE)){
		//read = USART_ReceiveData(USART6);
		//text_test[RXCount_test++] = read;
		//USART_puts(USART6, read);
		USART_SendData(USART1, USART_ReceiveData(USART6));

	//	}
	//	USART_ClearFlag(USART6, USART_FLAG_RXNE);

}

