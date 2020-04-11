/**
  ******************************************************************************
  * @file    bsp_motor_control.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   电机控制接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include ".\motor_control\bsp_motor_control.h"
#include "./usart/bsp_debug_usart.h"
#include "./pid/bsp_pid.h"
#include "./tim/bsp_basic_tim.h"
#include <math.h>
#include <stdlib.h>

static motor_dir_t direction  = MOTOR_FWD;     // 记录方向
static uint16_t    dutyfactor = 0;             // 记录占空比
static uint8_t    is_motor_en = 0;             // 电机使能

/**
  * @brief  设置电机速度
  * @param  v: 速度（占空比）
  * @retval 无
  */
void set_motor_speed(uint16_t v)
{
  v = (v > PWM_PERIOD_COUNT) ? PWM_PERIOD_COUNT : v;     // 上限处理
  
  dutyfactor = v;
  
  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // 设置速度
  }
  else
  {
    SET_REV_COMPAER(dutyfactor);     // 设置速度
  }
}

/**
  * @brief  设置电机方向
  * @param  无
  * @retval 无
  */
void set_motor_direction(motor_dir_t dir)
{
  direction = dir;
  
//  SET_FWD_COMPAER(0);     // 设置速度为 0
//  SET_REV_COMPAER(0);     // 设置速度为 0

//  HAL_Delay(200);         // 延时一会
  
  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // 设置速度
    SET_REV_COMPAER(0);              // 设置速度
  }
  else
  {
    SET_FWD_COMPAER(0);              // 设置速度
    SET_REV_COMPAER(dutyfactor);     // 设置速度
  }
  
}

/**
  * @brief  使能电机
  * @param  无
  * @retval 无
  */
void set_motor_enable(void)
{
  is_motor_en = 1;
  MOTOR_FWD_ENABLE();
  MOTOR_REV_ENABLE();
}

/**
  * @brief  禁用电机
  * @param  无
  * @retval 无
  */
void set_motor_disable(void)
{
  is_motor_en = 0;
  MOTOR_FWD_DISABLE();
  MOTOR_REV_DISABLE();
}

/**
  * @brief  电机位置式 PID 控制实现(定时调用)
  * @param  无
  * @retval 无
  */
void motor_pid_control(void)
{
  if (is_motor_en == 1)     // 电机在使能状态下才进行控制处理
  {
    float cont_val = 0;           // 当前控制值
    int32_t Capture_Count = 0;    // 当前时刻总计数值
    
    /* 当前时刻总计数值 = 计数器值 + 计数溢出次数 * ENCODER_TIM_PERIOD  */
    Capture_Count = __HAL_TIM_GET_COUNTER(&TIM_EncoderHandle) + (Encoder_Overflow_Count * ENCODER_TIM_PERIOD);
    
    cont_val = PID_realize(Capture_Count);    // 进行 PID 计算
    
    if (cont_val > 0)    // 判断电机方向
    {
      set_motor_direction(MOTOR_FWD);
    }
    else
    {
      cont_val = -cont_val;
      set_motor_direction(MOTOR_REV);
    }
    
    cont_val = (cont_val > PWM_PERIOD_COUNT*0.48) ? PWM_PERIOD_COUNT*0.48 : cont_val;    // 速度上限处理
    set_motor_speed(0);                                                         // 设置 PWM 占空比
    
  #if defined(PID_ASSISTANT_EN)
    set_computer_value(SEED_FACT_CMD, CURVES_CH1, &Capture_Count, 1);                // 给通道 1 发送实际值
  #else
    printf("实际值：%d. 目标值：%.0f\n", actual_speed, get_pid_actual());      // 打印实际值和目标值
  #endif
  }
}

///**
//  * @brief  定时器每100ms产生一次中断回调函数
//  * @param  htim：定时器句柄
//  * @retval 无
//  */
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//  if(htim==(&TIM_TimeBaseStructure))
//  {
//    motor_pid_control();
//  }
//}

/*********************************************END OF FILE**********************/

