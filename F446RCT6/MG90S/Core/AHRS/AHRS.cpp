#include "AHRS.h"

/**
 * @brief AHRS 构造函数
 * @param sample_time 采样时间
 * @param ahrs_calculate_func AHRS解算算法
 * @param get_imu_func 获取IMU数据(加速度、角速度)函数
 * @param imu_handle IMU句柄
 * @param get_mag_func 获取磁场数据函数
 * @param mag_handle 磁力计句柄
 */
AHRS::AHRS(float sample_time, AHRSCalculateFunc ahrs_calculate_func, GetIMUFunc get_imu_func, void *imu_handle, GetMagFunc get_mag_func, void *mag_handle)
	: _sample_time(sample_time),
	_ahrs_calculate_func(std::move(ahrs_calculate_func)),
	_get_imu_func(std::move(get_imu_func)),
	_imu_handle(imu_handle),
	_get_mag_func(std::move(get_mag_func)),
	_mag_handle(mag_handle),
	_gravity_norm(GRAVITY_NORM),
	_count(0) {
	_gyro_offset = { Gx_OFFSET, Gy_OFFSET, Gz_OFFSET };
}

/**
 * @brief 处理IMU校准错误
 * @return HAL_StatusTypeDef 错误状态
 */
HAL_StatusTypeDef AHRS::HandleIMUCalibrationError() {
	_gyro_offset = { Gx_OFFSET, Gy_OFFSET, Gz_OFFSET };
	_gravity_norm = GRAVITY_NORM;
	return HAL_ERROR;
}

/**
 * @brief 校准IMU偏移
 * @return HAL_StatusTypeDef 校准状态
 */
HAL_StatusTypeDef AHRS::CalibrateIMU() {
	constexpr uint16_t CalibrationTimes = 1000;
	const uint32_t start_time = HAL_GetTick();
	uint16_t error_count = 0, restart = 0;
	float gravity_temp, k;

	while (true) {
		if (HAL_GetTick() - start_time > 60000) {
			_gyro_offset = { Gx_OFFSET, Gy_OFFSET, Gz_OFFSET };
			_gravity_norm = GRAVITY_NORM;
			return HAL_TIMEOUT;
		}

		_gravity_norm = 0.0f;
		_gyro_offset = { 0.0f, 0.0f, 0.0f };
		k = 0.0f;
		error_count = 0;
		restart = 0;

		for (uint16_t i = 0; i < CalibrationTimes; ++i) {
			const uint32_t last_time = HAL_GetTick();
			_count = static_cast<uint8_t>(100 * (i + 1) / CalibrationTimes);
			_get_imu_func(_imu_handle, _accel, _gyro);

			if (IsNanOrInf(_accel.x) || IsNanOrInf(_accel.y) || IsNanOrInf(_accel.z) ||
				IsNanOrInf(_gyro.x) || IsNanOrInf(_gyro.y) || IsNanOrInf(_gyro.z)) {
				return HandleIMUCalibrationError();
			}

			gravity_temp = std::sqrt(_accel.x * _accel.x + _accel.y * _accel.y + _accel.z * _accel.z);

			if (i != 0 && (std::abs(_gravity_norm - gravity_temp) > 0.5f ||
				std::abs(_gyro_offset.x - _gyro.x) > 0.05f ||
				std::abs(_gyro_offset.y - _gyro.y) > 0.05f ||
				std::abs(_gyro_offset.z - _gyro.z) > 0.05f)) {
				--i;
				++error_count;
				if (error_count > 100) {
					restart = 1;
					break;
				}
				HAL_Delay(20);
				continue;
			}

			_gravity_norm = gravity_temp * (1 - k) + _gravity_norm * k;
			_gyro_offset.x = _gyro.x * (1 - k) + _gyro_offset.x * k;
			_gyro_offset.y = _gyro.y * (1 - k) + _gyro_offset.y * k;
			_gyro_offset.z = _gyro.z * (1 - k) + _gyro_offset.z * k;
			k = (i + 1.0f) / (i + 2.0f);

			if (HAL_GetTick() - last_time == 0) {
				HAL_Delay(1);
			}
		}

		if (_gravity_norm == 0.0f) {
			return HandleIMUCalibrationError();
		}

		if (!restart) {
			break;
		}
	}

	return HAL_OK;
}

/**
 * @brief 更新AHRS数据
 */
void AHRS::Update() {
	if (_get_imu_func) {
		_get_imu_func(_imu_handle, _accel, _gyro);
	}

	if (_get_mag_func) {
		_get_mag_func(_mag_handle, _mag);
	} else {
		_mag = { 0.0f, 0.0f, 0.0f };
	}

	_gyro.x -= _gyro_offset.x;
	_gyro.y -= _gyro_offset.y;
	_gyro.z -= _gyro_offset.z;

	_ahrs_calculate_func(_quat, _sample_time, _accel, _gyro, _mag);

	CalculateEulerAngles();

	AccelData real_accel;
	ConvertToEarthFrame(_accel, real_accel);
	real_accel.z -= _gravity_norm;
	_real_accel.x = LowPassFilter(real_accel.x, _real_accel.x, 0.6f);
	_real_accel.y = LowPassFilter(real_accel.y, _real_accel.y, 0.6f);
	_real_accel.z = LowPassFilter(real_accel.z, _real_accel.z, 0.6f);
}

/**
 * @brief 计算四元数对应的欧拉角
 */
void AHRS::CalculateEulerAngles() {
	_euler_angle.yaw = std::atan2(2.0f * (_quat[0] * _quat[3] + _quat[1] * _quat[2]), 2.0f * (_quat[0] * _quat[0] + _quat[1] * _quat[1]) - 1.0f);
	_euler_angle.pitch = std::atan2(2.0f * (_quat[0] * _quat[1] + _quat[2] * _quat[3]), 2.0f * (_quat[0] * _quat[0] + _quat[3] * _quat[3]) - 1.0f);
	_euler_angle.roll = std::asin(-2.0f * (_quat[1] * _quat[3] - _quat[0] * _quat[2]));
}

/**
 * @brief 将地球坐标系向量转换为机体坐标系向量
 * @param vecEF 地球坐标系向量
 * @param vecBF 机体坐标系向量
 */
void AHRS::ConvertToBodyFrame(const RectangularCoordinate &vecEF, RectangularCoordinate &vecBF) {
	vecBF.x = 2.0f * ((0.5f - _quat[2] * _quat[2] - _quat[3] * _quat[3]) * vecEF.x + (_quat[1] * _quat[2] + _quat[0] * _quat[3]) * vecEF.y + (_quat[1] * _quat[3] - _quat[0] * _quat[2]) * vecEF.z);
	vecBF.y = 2.0f * ((_quat[1] * _quat[2] - _quat[0] * _quat[3]) * vecEF.x + (0.5f - _quat[1] * _quat[1] - _quat[3] * _quat[3]) * vecEF.y + (_quat[2] * _quat[3] + _quat[0] * _quat[1]) * vecEF.z);
	vecBF.z = 2.0f * ((_quat[1] * _quat[3] + _quat[0] * _quat[2]) * vecEF.x + (_quat[2] * _quat[3] - _quat[0] * _quat[1]) * vecEF.y + (0.5f - _quat[1] * _quat[1] - _quat[2] * _quat[2]) * vecEF.z);
}

/**
 * @brief 将机体坐标系向量转换为地球坐标系向量
 * @param vecBF 机体坐标系向量
 * @param vecEF 地球坐标系向量
 */
void AHRS::ConvertToEarthFrame(const RectangularCoordinate &vecBF, RectangularCoordinate &vecEF) {
	vecEF.x = 2.0f * ((0.5f - _quat[2] * _quat[2] - _quat[3] * _quat[3]) * vecBF.x + (_quat[1] * _quat[2] - _quat[0] * _quat[3]) * vecBF.y + (_quat[1] * _quat[3] + _quat[0] * _quat[2]) * vecBF.z);
	vecEF.y = 2.0f * ((_quat[1] * _quat[2] + _quat[0] * _quat[3]) * vecBF.x + (0.5f - _quat[1] * _quat[1] - _quat[3] * _quat[3]) * vecBF.y + (_quat[2] * _quat[3] - _quat[0] * _quat[1]) * vecBF.z);
	vecEF.z = 2.0f * ((_quat[1] * _quat[3] - _quat[0] * _quat[2]) * vecBF.x + (_quat[2] * _quat[3] + _quat[0] * _quat[1]) * vecBF.y + (0.5f - _quat[1] * _quat[1] - _quat[2] * _quat[2]) * vecBF.z);
}