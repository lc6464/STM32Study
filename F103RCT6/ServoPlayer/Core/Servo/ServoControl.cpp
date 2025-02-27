#include <algorithm>
#include <cstdint>
#include <cmath>

#include "tim.h"

#include "data.h"
#include "PID_Shared.h"
#include "Status.h"
#include "UnitConvertor.h"

// 向舵机发送角度
static inline void SetServoAngle(int16_t angle) {
	Status::servoAngle = angle;
	uint16_t compareValue = UnitConvertor::AngleToCompare(angle);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, compareValue);
}

// 从 BMI088 读取数据并计算角度
static inline float CalculateRealTimeAngle() {
	// bmi088_data_average.x = bmi088_data_accumulator.x / 1.0f;
	// bmi088_data_average.y = bmi088_data_accumulator.y / 1.0f;
	// bmi088_data_average.z = bmi088_data_accumulator.z / 1.0f;
	// bmi088_data_accumulator.x = 0;
	// bmi088_data_accumulator.y = 0;
	// bmi088_data_accumulator.z = 0;

	// float radian = std::atan2f(-bmi088_data_average.y, bmi088_data_average.x);
	float radian = std::atan2f(-bmi088_data.y, bmi088_data.x);
	auto degree = UnitConvertor::RadianToDegree(radian);

	Status::bmi088_angle = degree;

	return degree;
}

// PID 控制
void ServoControl() {
	float angle = CalculateRealTimeAngle();
	// float output = pid.Update(Status::targetAngle, angle);
	// SetServoAngle(static_cast<int16_t>(output));
	SetServoAngle(static_cast<int16_t>(angle));
}