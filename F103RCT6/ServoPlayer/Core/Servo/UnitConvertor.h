#pragma once

#include <cstdint>

namespace UnitConvertor {
	// 将 -90° ~ 90° 的角度映射到 25 ~ 125 的定时器比较值
	uint32_t AngleToCompare(int16_t angle);

	// 将角度转换为弧度
	float DegreeToRadian(float degree);

	// 将弧度转换为角度
	float RadianToDegree(float radian);
}