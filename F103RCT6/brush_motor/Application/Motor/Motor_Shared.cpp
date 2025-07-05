#include "Motor_Shared.h"

// 四个电机
Motor leftFrontMotor(&htim8, TIM_CHANNEL_1, TIM_CHANNEL_2);
Motor rightFrontMotor(&htim1, TIM_CHANNEL_4, TIM_CHANNEL_3);
Motor leftBackMotor(&htim8, TIM_CHANNEL_4, TIM_CHANNEL_3);
Motor rightBackMotor(&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2);