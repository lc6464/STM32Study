#pragma once

#include "AHRS.h"


/**
 * @brief 基于 Mahony 算法的 AHRS 解算更新
 * @param[in,out] quat 四元数
 * @param[in] sample_time 采样时间 (s)
 * @param[in] accel 加速度 (m/s^2)
 * @param[in] gyro 角加速度 (rad/s)
 * @param[in] mag 磁场大小 (uT)
 */
void Mahony_AHRS_Update(float quat[4], float sample_time, Accel_Data_t *accel,
	Gyro_Data_t *gyro, Mag_Data_t *mag);