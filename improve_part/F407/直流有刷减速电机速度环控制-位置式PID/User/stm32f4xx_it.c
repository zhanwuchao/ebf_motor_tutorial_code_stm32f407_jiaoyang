/**
  ******************************************************************************
  * @file    GPIO/GPIO_EXTI/Src/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
#include "./led/bsp_led.h"
#include "./tim/bsp_motor_tim.h"
#include "./usart/bsp_debug_usart.h"
#include "./encoder/bsp_encoder.h"
#include "./tim/bsp_basic_tim.h"
#include ".\motor_control\bsp_motor_control.h"

//接收数组指针
extern unsigned char UART_RxPtr;
extern TIM_HandleTypeDef TIM_EncoderHandle;

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M7 Processor Exceptions Handlers                         */
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
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

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
  HAL_IncTick();
  
}
  

/******************************************************************************/
/*                 STM32F7xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f7xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

// 串口中断服务函数

void DEBUG_USART_IRQHandler(void)
{
  __IO uint8_t data;
  
  if(__HAL_UART_GET_IT_SOURCE(&UartHandle, UART_IT_RXNE) != RESET)
	{
    data = UartHandle.Instance->DR;
    
    //如果为退格键
    if(data == '\b')
    {
      //如果指针不在数组的开始位置
      if(UART_RxPtr)
      {
        Usart_SendByte('\b');
        Usart_SendByte(' ');
        Usart_SendByte('\b');
        UART_RxPtr--;
        UART_RxBuffer[UART_RxPtr]=0x00;
      }
    }
    //如果不是退格键
    else
    {
      //将数据填入数组UART_RxBuffer
      //并且将后面的一个元素清零如果数组满了则写入最后一个元素为止
      if(UART_RxPtr < (UART_RX_BUFFER_SIZE - 1))
      {
        UART_RxBuffer[UART_RxPtr] = data;
        UART_RxBuffer[UART_RxPtr + 1]=0x00;
        UART_RxPtr++;
      }
      else
      {
        UART_RxBuffer[UART_RxPtr - 1] = data;
        Usart_SendByte('\b');
      }
      //如果为回车键，则开始处理串口数据
      if(data == 13 || data == 10)
      {
        receive_cmd = 1;
      }
      else
      {
        Usart_SendByte(data);
      }
    }
    
    __HAL_UART_CLEAR_FLAG(&UartHandle, UART_IT_RXNE);
  }
  
//  HAL_UART_Receive_IT(&UartHandle, &data, sizeof(data));
  
  HAL_UART_IRQHandler(&UartHandle);
}

/**
  * @brief 编码器接口中断服务函数
  */
void ENCODER_TIM_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TIM_EncoderHandle);
}

/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */	
void BASIC_TIM_IRQHandler (void)
{
	HAL_TIM_IRQHandler(&TIM_TimeBaseStructure);	 	
}

/**
  * @brief  定时器更新事件回调函数
  * @param  无
  * @retval 无
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim==(&TIM_EncoderHandle))
  {
    /* 判断当前计数器计数方向 */
    if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&TIM_EncoderHandle))
      /* 下溢 */
      Encoder_Overflow_Count--;
    else
      /* 上溢 */
      Encoder_Overflow_Count++;
  }
  else if(htim==(&TIM_TimeBaseStructure))
  {
    motor_pid_control();
  }
}


/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
