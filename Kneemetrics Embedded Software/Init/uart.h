#include "stm32f4xx.h"
#include "global.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_usart.h"

void InitExtUART(void);
void TX_ExtUART (char* string);
void USART_puts(USART_TypeDef* USARTx, volatile char *s);
