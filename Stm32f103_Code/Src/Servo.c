#include "main.h"
#include "tim.h"

//舵机初始化
void Servo_Init()
{
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 50);
}

// 舵机控制
void ControlServo()
{
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 175);
    HAL_Delay(3000);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 50);
}
