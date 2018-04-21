/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Template/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-January-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "sys_clock.h"
//#include "buttons.h"
#include "uart.h"
#include "irq.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "main.h"
#include "deca_irq.h"
#include "deca_port.h"
#include "deca_device_api.h"
#include "deca_spi.h"
#include "deca_instance_app.h"
#include "global.h"

#include "battery_charger_management.h"

/*
#include "usb_core.h"
#include "usbd_core.h"
#include "usbd_cdc_core.h"
*/
__IO uint16_t IC2Value = 0;
__IO uint16_t DutyCycle = 0;
__IO uint32_t Frequency = 0;
//#include "ble_irq.h"
//#include "ble_app.h"

/*extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
extern USB_OTG_CORE_HANDLE USB_OTG_dev;*/

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif

__IO unsigned long time32_incr;

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
/*void HardFault_Handler(void)
{
   Go to infinite loop when Hard Fault exception occurs
  while (1)
  {
  }
}*/

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
/*  TimingDelay_Decrement(); */
	//Update_SysTickFlags();
	time32_incr++;
}

void EXTI2_IRQHandler(void)
{
  /* Handle new gyro*/


  gyro_data_ready_cb();
  EXTI_ClearITPendingBit(EXTI_Line2);
}
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/*void EXTI0_IRQHandler(void)
{
	HandleButtonState();
}*/
void TIM4_IRQHandler(void)
{
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);

  /* Clear TIM4 Capture compare interrupt pending bit */
  TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);

  /* Get the Input Capture value */
  IC2Value = TIM_GetCapture2(TIM4);

  if (IC2Value != 0)
  {
    /* Duty cycle computation */
    DutyCycle = (TIM_GetCapture1(TIM4) * 100) / IC2Value;

    /* Frequency computation
       TIM4 counter clock = (RCC_Clocks.HCLK_Frequency)/2 */

    Frequency = (RCC_Clocks.HCLK_Frequency)/2 / IC2Value;
  }
  else
  {
    DutyCycle = 0;
    Frequency = 0;
  }
}

/*void USART6_IRQHandler(void)
{
	char read;

	while (USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == RESET){};
	read = USART_ReceiveData(USART6);
	text_test[RXCount_test++] = read;
	text_test[RXCount_test] = '\0';
}*/

//void EXTI1_IRQHandler(void)
//{
 //   process_deca_irq();
 //   /* Clear EXTI Line 8 Pending Bit */
//    EXTI_ClearFlag(DECAIRQ_EXTI_Line);
//    EXTI_ClearITPendingBit(DECAIRQ_EXTI_Line);

//}

/*void EXTI3_IRQHandler(void)
{
	SetMxLow(M0);
	SetMxHigh(M1);
	EXTI_ClearFlag(ACOKn_EXTI_Line);
	EXTI_ClearITPendingBit(ACOKn_EXTI_Line);
}*/

/*void EXTI9_5_IRQHandler(void)// Pradeep settings
{
    process_deca_irq();
    // Clear EXTI Line 8 Pending Bit
    EXTI_ClearFlag(DECAIRQ_EXTI_Line);
    EXTI_ClearITPendingBit(DECAIRQ_EXTI_Line);
}

void EXTI15_10_IRQHandler(void)
{
	process_ble_irq();
     //Clear EXTI Line 11 Pending Bit
 //   EXTI_ClearFlag(BLEIRQ_EXTI_Line);
    EXTI_ClearITPendingBit(BLEIRQ_EXTI_Line);
}*/
/*void EXTI15_10_IRQHandler(void)
{
    process_deca_irq();
    // Clear EXTI Line 8 Pending Bit
    EXTI_ClearFlag(DECAIRQ_EXTI_Line);
    EXTI_ClearITPendingBit(DECAIRQ_EXTI_Line);
}*/

/**
  * @}
  */ 

#ifdef USE_USB_OTG_FS1
void OTG_FS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearFlag(EXTI_Line18);
  EXTI_ClearITPendingBit(EXTI_Line18);

}
#endif

/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS
void OTG_HS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line20);
}
#endif

/**
  * @brief  This function handles OTG_HS Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS
void OTG_HS_IRQHandler(void)
#else
void OTG_FS_IRQHandler(void)
#endif
{
 // USBD_OTG_ISR_Handler (&USB_OTG_dev);
}

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
/**
  * @brief  This function handles EP1_IN Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_IN_IRQHandler(void)
{
  USBD_OTG_EP1IN_ISR_Handler (&USB_OTG_dev);
}

/**
  * @brief  This function handles EP1_OUT Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_OUT_IRQHandler(void)
{
  USBD_OTG_EP1OUT_ISR_Handler (&USB_OTG_dev);
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
