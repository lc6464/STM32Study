#pragma once

#include "main.h"
#include <cmath>
#include <type_traits>
#include <functional>

/**
 * @brief 检查浮点数是否为 NaN（非数字）或无穷大
 * @param x 浮点数
 * @return true 如果 x 是 NaN 或无穷大
 * @return false 如果 x 不是 NaN 也不是无穷大
 */
inline bool IsNanOrInf(float x) {
	return std::isnan(x) || std::isinf(x);
}

/**
 * @brief 计算浮点数的倒数平方根
 * @param x 浮点数
 * @return float 倒数平方根
 */
inline float InvSqrt(float x) {
	return 1.0f / std::sqrt(x);
}

/**
 * @brief 低通滤波器
 * @tparam T 数据类型
 * @param current_value 当前值
 * @param previous_average 前一个平均值
 * @param smoothing_factor 平滑因子
 * @return T 新的平均值
 */
template<typename T>
constexpr auto LowPassFilter(T current_value, T previous_average, float smoothing_factor) noexcept
-> std::enable_if_t<std::is_arithmetic_v<T>, T> {
	return current_value * smoothing_factor + previous_average * (1.0f - smoothing_factor);
}

/**
 * @brief 直角坐标系结构体
 */
struct RectangularCoordinate {
	float x;
	float y;
	float z;
};

/**
 * @brief 加速度数据
 */
using AccelData = RectangularCoordinate;

/**
 * @brief 角速度数据
 */
using GyroData = RectangularCoordinate;

/**
 * @brief 磁场数据
 */
using MagData = RectangularCoordinate;

/**
 * @brief 欧拉角数据
 */
struct EulerData {
	float pitch;
	float roll;
	float yaw;
};

/**
 * @brief AHRS 类
 */
class AHRS {
public:
	using AHRSCalculateFunc = std::function<void(float quat[4], float sample_time, AccelData *, GyroData *, MagData *)>;
	using GetIMUFunc = std::function<void(void *, AccelData *, GyroData *)>;
	using GetMagFunc = std::function<void(void *, MagData *)>;

	/**
	 * @brief 构造函数
	 * @param sample_time 采样时间
	 * @param ahrs_calculate_func AHRS解算算法
	 * @param get_imu_func 获取IMU数据(加速度、角速度)函数
	 * @param imu_handle IMU句柄
	 * @param get_mag_func 获取磁场数据函数
	 * @param mag_handle 磁力计句柄
	 */
	explicit AHRS(float sample_time, AHRSCalculateFunc ahrs_calculate_func, GetIMUFunc get_imu_func, void *imu_handle, GetMagFunc get_mag_func, void *mag_handle);

	/**
	 * @brief 矫正角速度零飘
	 * @return 函数执行状态
	 *  - HAL_OK 矫正成功
	 *  - HAL_TIMEOUT 矫正超时
	 *  - HAL_ERROR 矫正失败(IMU数据异常)
	 */
	HAL_StatusTypeDef CalibrateIMUOffset();

	/**
	 * @brief 更新AHRS数据
	 */
	void Update();

	/**
	 * @brief 获取欧拉角
	 * @return 欧拉角数据结构体
	 */
	EulerData GetEuler() const {
		return _euler_angle;
	}

	/**
	 * @brief 获取加速度数据
	 * @return 加速度数据结构体
	 */
	AccelData GetAccel() const {
		return _accel;
	}

	/**
	 * @brief 获取角速度数据
	 * @return 角速度数据结构体
	 */
	GyroData GetGyro() const {
		return _gyro;
	}

	/**
	 * @brief 获取磁场数据
	 * @return 磁场数据结构体
	 */
	MagData GetMag() const {
		return _mag;
	}

	/**
	 * @brief 获取四元数数据
	 * @return 四元数数据数组
	 */
	const float *GetQuat() const {
		return _quat;
	}

	/**
	 * @brief 获取标定时的重力加速度
	 * @return 重力加速度
	 */
	float GetGNorm() const {
		return _g_norm;
	}

private:
	EulerData _euler_angle;
	AccelData _accel;
	AccelData _real_accel;
	GyroData _gyro;
	MagData _mag;
	float _quat[4];
	float _g_norm;
	float _sample_time;
	GyroData _gyro_offset;
	AHRSCalculateFunc _ahrs_calculate_func;
	void *_imu_handle;
	GetIMUFunc _get_imu_func;
	void *_mag_handle;
	GetMagFunc _get_mag_func;
	uint8_t _count;

	HAL_StatusTypeDef CalibrateIMUOffsetErrorHandle();
	void GetAngle(const float quat[4], EulerData *euler_angle);
	void EarthFrameToBodyFrame(const RectangularCoordinate &vecEF, RectangularCoordinate *vecBF, const float q[4]);
	void BodyFrameToEarthFrame(const RectangularCoordinate &vecBF, RectangularCoordinate *vecEF, const float q[4]);
};