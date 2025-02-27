#include "UnitConvertor.h"

#include <algorithm>
#include <numbers>

// 将 -90° ~ 90° 的角度映射到 25 ~ 125 的定时器比较值
uint32_t UnitConvertor::AngleToCompare(int16_t angle) {
	angle = std::clamp(angle, static_cast<int16_t>(-90), static_cast<int16_t>(90));

	return static_cast<uint32_t>((angle * 5 / 9) + 75);
}

// 将角度转换为弧度
float UnitConvertor::DegreeToRadian(float degree) {
	return degree * std::numbers::pi_v<float> / 180.0f;
}

// 将弧度转换为角度
float UnitConvertor::RadianToDegree(float radian) {
	return radian * 180.0f / std::numbers::pi_v<float>;
}