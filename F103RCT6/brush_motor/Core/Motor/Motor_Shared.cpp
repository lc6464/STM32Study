#include "Motor_Shared.h"

// 两个电机
Motor leftMotor(&htim8, TIM_CHANNEL_1, TIM_CHANNEL_2);
Motor rightMotor(&htim1, TIM_CHANNEL_4, TIM_CHANNEL_3);