#ifndef __BSP_MOTOR_CONTROL_H
#define	__BSP_MOTOR_CONTROL_H

#include "stm32f4xx.h"
#include "./tim/bsp_motor_tim.h"
#include "main.h"

/* 电机方向控制枚举 */
typedef enum
{
  MOTOR_FWD = 0,
  MOTOR_REV,
}motor_dir_t;

/* 设置速度（占空比） */
#define SET_FWD_COMPAER(ChannelPulse)     TIM1_SetPWM_pulse(PWM_CHANNEL_1,ChannelPulse)    // 设置比较寄存器的值
#define SET_REV_COMPAER(ChannelPulse)     TIM1_SetPWM_pulse(PWM_CHANNEL_2,ChannelPulse)    // 设置比较寄存器的值

/* 使能输出 */
#define MOTOR_FWD_ENABLE()      HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,PWM_CHANNEL_1);    // 使能 PWM 通道 1
#define MOTOR_REV_ENABLE()      HAL_TIM_PWM_Start(&TIM_TimeBaseStructure,PWM_CHANNEL_2);    // 使能 PWM 通道 2

/* 禁用输出 */
#define MOTOR_FWD_DISABLE()     HAL_TIM_PWM_Stop(&TIM_TimeBaseStructure,PWM_CHANNEL_1);     // 禁用 PWM 通道 1
#define MOTOR_REV_DISABLE()     HAL_TIM_PWM_Stop(&TIM_TimeBaseStructure,PWM_CHANNEL_2);     // 禁用 PWM 通道 2

void set_motor_speed(uint16_t v);
void set_motor_direction(motor_dir_t dir);

#endif /* __LED_H */

