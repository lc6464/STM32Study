#include "Motor_Shared.h"

// 两个电机
Motor leftMotor(&htim8, TIM_CHANNEL_2, TIM_CHANNEL_1);
Motor rightMotor(&htim1, TIM_CHANNEL_3, TIM_CHANNEL_4);