#include "AHRS.h"

#define GxOFFSET 0.00055712f
#define GyOFFSET -0.0056608f
#define GzOFFSET -0.0009765f
#define gNORM 9.82609f

float g_norm = gNORM;

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
	_g_norm(gNORM),
	_count(0) {
	_accel = { 0.0f, 0.0f, 0.0f };
	_gyro = { 0.0f, 0.0f, 0.0f };
	_mag = { 0.0f, 0.0f, 0.0f };
	_gyro_offset = { GxOFFSET, GyOFFSET, GzOFFSET };
	_euler_angle = { 0.0f, 0.0f, 0.0f };
	_quat[0] = 1.0f;
	_quat[1] = 0.0f;
	_quat[2] = 0.0f;
	_quat[3] = 0.0f;
}

/**
 * @brief 矫正IMU偏移错误处理
 * @return HAL_StatusTypeDef 错误状态
 */
HAL_StatusTypeDef AHRS::CalibrateIMUOffsetErrorHandle() {
	_gyro_offset = { GxOFFSET, GyOFFSET, GzOFFSET };
	_g_norm = gNORM;
	return HAL_ERROR;
}

/**
 * @brief 矫正IMU偏移
 * @return HAL_StatusTypeDef 矫正状态
 */
HAL_StatusTypeDef AHRS::CalibrateIMUOffset() {
	constexpr uint16_t CaliTimes = 1000;
	const uint32_t start_time = HAL_GetTick();
	uint16_t error_count = 0, restart = 0;
	float gNormTemp, k;

	while (true) {
		if (HAL_GetTick() - start_time > 60000) {
			_gyro_offset = { GxOFFSET, GyOFFSET, GzOFFSET };
			_g_norm = gNORM;
			return HAL_TIMEOUT;
		}

		_g_norm = 0.0f;
		_gyro_offset = { 0.0f, 0.0f, 0.0f };
		k = 0.0f;
		error_count = 0;
		restart = 0;

		for (uint16_t i = 0; i < CaliTimes; ++i) {
			const uint32_t last_time = HAL_GetTick();
			_count = static_cast<uint8_t>(100 * (i + 1) / CaliTimes);
			_get_imu_func(_imu_handle, &_accel, &_gyro);

			if (IsNanOrInf(_accel.x) || IsNanOrInf(_accel.y) || IsNanOrInf(_accel.z) ||
				IsNanOrInf(_gyro.x) || IsNanOrInf(_gyro.y) || IsNanOrInf(_gyro.z)) {
				return CalibrateIMUOffsetErrorHandle();
			}

			gNormTemp = std::sqrt(_accel.x * _accel.x + _accel.y * _accel.y + _accel.z * _accel.z);

			if (i != 0 && (std::abs(_g_norm - gNormTemp) > 0.5f ||
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

			_g_norm = gNormTemp * (1 - k) + _g_norm * k;
			_gyro_offset.x = _gyro.x * (1 - k) + _gyro_offset.x * k;
			_gyro_offset.y = _gyro.y * (1 - k) + _gyro_offset.y * k;
			_gyro_offset.z = _gyro.z * (1 - k) + _gyro_offset.z * k;
			k = (i + 1.0f) / (i + 2.0f);

			if (HAL_GetTick() - last_time == 0) {
				HAL_Delay(1);
			}
		}

		if (_g_norm == 0.0f) {
			return CalibrateIMUOffsetErrorHandle();
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
		_get_imu_func(_imu_handle, &_accel, &_gyro);
	}

	if (_get_mag_func) {
		_get_mag_func(_mag_handle, &_mag);
	} else {
		_mag = { 0.0f, 0.0f, 0.0f };
	}

	_gyro.x -= _gyro_offset.x;
	_gyro.y -= _gyro_offset.y;
	_gyro.z -= _gyro_offset.z;

	g_norm = _g_norm;
	_ahrs_calculate_func(_quat, _sample_time, &_accel, &_gyro, &_mag);

	GetAngle(_quat, &_euler_angle);

	static AccelData real_accel;
	BodyFrameToEarthFrame(_accel, &real_accel, _quat);
	real_accel.z -= _g_norm;
	_real_accel.x = LowPassFilter(real_accel.x, _real_accel.x, 0.6f);
	_real_accel.y = LowPassFilter(real_accel.y, _real_accel.y, 0.6f);
	_real_accel.z = LowPassFilter(real_accel.z, _real_accel.z, 0.6f);
}

/**
 * @brief 计算四元数对应的欧拉角
 * @param quat 四元数
 * @param euler_angle 欧拉角结构体
 */
void AHRS::GetAngle(const float quat[4], EulerData *euler_angle) {
	euler_angle->yaw = std::atan2(2.0f * (quat[0] * quat[3] + quat[1] * quat[2]), 2.0f * (quat[0] * quat[0] + quat[1] * quat[1]) - 1.0f);
	euler_angle->pitch = std::atan2(2.0f * (quat[0] * quat[1] + quat[2] * quat[3]), 2.0f * (quat[0] * quat[0] + quat[3] * quat[3]) - 1.0f);
	euler_angle->roll = std::asin(-2.0f * (quat[1] * quat[3] - quat[0] * quat[2]));
}

/**
 * @brief 将地球坐标系向量转换为机体坐标系向量
 * @param vecEF 地球坐标系向量
 * @param vecBF 机体坐标系向量
 * @param q 四元数
 */
void AHRS::EarthFrameToBodyFrame(const RectangularCoordinate &vecEF, RectangularCoordinate *vecBF, const float q[4]) {
	vecBF->x = 2.0f * ((0.5f - q[2] * q[2] - q[3] * q[3]) * vecEF.x + (q[1] * q[2] + q[0] * q[3]) * vecEF.y + (q[1] * q[3] - q[0] * q[2]) * vecEF.z);
	vecBF->y = 2.0f * ((q[1] * q[2] - q[0] * q[3]) * vecEF.x + (0.5f - q[1] * q[1] - q[3] * q[3]) * vecEF.y + (q[2] * q[3] + q[0] * q[1]) * vecEF.z);
	vecBF->z = 2.0f * ((q[1] * q[3] + q[0] * q[2]) * vecEF.x + (q[2] * q[3] - q[0] * q[1]) * vecEF.y + (0.5f - q[1] * q[1] - q[2] * q[2]) * vecEF.z);
}

/**
 * @brief 将机体坐标系向量转换为地球坐标系向量
 * @param vecBF 机体坐标系向量
 * @param vecEF 地球坐标系向量
 * @param q 四元数
 */
void AHRS::BodyFrameToEarthFrame(const RectangularCoordinate &vecBF, RectangularCoordinate *vecEF, const float q[4]) {
	vecEF->x = 2.0f * ((0.5f - q[2] * q[2] - q[3] * q[3]) * vecBF.x + (q[1] * q[2] - q[0] * q[3]) * vecBF.y + (q[1] * q[3] + q[0] * q[2]) * vecBF.z);
	vecEF->y = 2.0f * ((q[1] * q[2] + q[0] * q[3]) * vecBF.x + (0.5f - q[1] * q[1] - q[3] * q[3]) * vecBF.y + (q[2] * q[3] - q[0] * q[1]) * vecBF.z);
	vecEF->z = 2.0f * ((q[1] * q[3] - q[0] * q[2]) * vecBF.x + (q[2] * q[3] + q[0] * q[1]) * vecBF.y + (0.5f - q[1] * q[1] - q[2] * q[2]) * vecBF.z);
}