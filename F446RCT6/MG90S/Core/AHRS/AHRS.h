#pragma once

#include "main.h"

#include <math.h>
#include <stdlib.h>


#define IS_NAN_INF(x) (isnan(x) || isinf(x))


/**
 * @brief 直角坐标系结构体
 */
typedef struct {
	float x;
	float y;
	float z;
} Rectangular_Coordinate_t;

/**
 * @brief 加速度数据
 */
typedef Rectangular_Coordinate_t Accel_Data_t;

/**
 * @brief 角速度数据
 */
typedef Rectangular_Coordinate_t Gyro_Data_t;

/**
 * @brief 磁场数据
 */
typedef Rectangular_Coordinate_t Mag_Data_t;

/**
 * @brief 欧拉角数据
 */
typedef struct {
	float pitch;
	float roll;
	float yaw;
} Euler_Data_t;

/**
 * @brief AHRS 解算算法回调函数
 */
typedef void (*AHRS_Calculate_fun_t)(float quat[4], float sample_time,
	Accel_Data_t *accel, Gyro_Data_t *gyro,
	Mag_Data_t *mag);

/**
 * @brief 获取IMU数据回调函数
 */
typedef void (*Get_IMU_fun_t)(void *config, Accel_Data_t *accel,
	Gyro_Data_t *gyro);

/**
 * @brief 获取磁力计数据回调函数
 */
typedef void (*Get_Mag_fun_t)(void *config, Mag_Data_t *mag);

/**
 * @brief AHRS 句柄
 */
typedef struct {
	// 欧拉角
	Euler_Data_t euler_angle;

	// 加速度
	Accel_Data_t accel;

	// 惯性坐标系下的加速度
	Accel_Data_t real_accel;

	// 角速度
	Gyro_Data_t gyro;

	// 磁场
	Mag_Data_t mag;

	// 姿态四元数
	float quat[4];

	// 标定时的重力加速度
	float g_norm;

	// 采样时间
	float sample_time;

	// 角速度零偏校正值
	Gyro_Data_t gyro_offset;

	// AHRS 解算算法
	AHRS_Calculate_fun_t AHRS_Calculate_fun;

	// IMU句柄
	void *IMU_handle;

	// 获取IMU数据函数
	Get_IMU_fun_t Get_IMU_fun;

	// 磁力计句柄
	void *mag_handle;

	// 获取磁力计数据函数
	Get_Mag_fun_t Get_Mag_fun;

	uint8_t count;
} AHRS_t;

extern float g_norm;

/**
 * @brief 将四元数转换为欧拉角
 * @param[in] quat 四元数
 * @param[out] euler_angle 欧拉角
 */
void AHRS_Get_Angle(const float quat[4], Euler_Data_t *euler_angle);

/**
 * @brief AHRS初始化
 * @param[out] AHRS AHRS句柄
 * @param[in] sample_time 采样时间
 * @param[in] AHRS_Calculate_fun AHRS解算算法
 * @param[in] Get_IMU_fun 获取IMU数据(加速度、角速度)函数
 * @param[in] IMU_handle IMU句柄
 * @param[in] Get_Mag_fun 获取磁场数据函数
 * @param[in] mag_handle 磁力计句柄
 */
void AHRS_Init(AHRS_t *AHRS, float sample_time,
	AHRS_Calculate_fun_t AHRS_Calculate_fun,
	Get_IMU_fun_t Get_IMU_fun, void *IMU_handle,
	Get_Mag_fun_t Get_Mag_fun, void *mag_handle);

/**
 * @brief 矫正角速度零飘
 * @param[in,out] AHRS AHRS句柄
 * @return 函数执行状态
 *  - HAL_OK 矫正成功
 * 	- HAL_TIMEOUT 矫正超时
 *  - HAL_ERROR 矫正失败(IMU数据异常)
 */
HAL_StatusTypeDef Calibrate_IMU_Offset(AHRS_t *AHRS);

/**
 * @brief 更新AHRS数据
 * @param[in,out] AHRS AHRS句柄
 */
void AHRS_Update(AHRS_t *AHRS);

/**
 * @brief 获取AHRS欧拉角数据
 * @param[in] AHRS AHRS句柄
 * @return 欧拉角数据结构体
 */
static inline Euler_Data_t Get_AHRS_Euler(const AHRS_t *AHRS) {
	return AHRS->euler_angle;
}

/**
 * @brief 获取AHRS加速度数据
 * @param[in] AHRS AHRS句柄
 * @return 加速度数据结构体
 */
static inline Accel_Data_t Get_AHRS_Accel(const AHRS_t *AHRS) {
	return AHRS->accel;
}

/**
 * @brief 获取AHRS角速度数据
 * @param[in] AHRS AHRS句柄
 * @return 角速度数据结构体
 */
static inline Gyro_Data_t Get_AHRS_Gyro(const AHRS_t *AHRS) {
	return AHRS->gyro;
}

/**
 * @brief 获取AHRS磁场数据
 * @param[in] AHRS AHRS句柄
 * @return 磁场数据结构体
 */
static inline Mag_Data_t Get_AHRS_Mag(const AHRS_t *AHRS) {
	return AHRS->mag;
}

/**
 * @brief 获取 AHRS 四元数数据
 * @param[in] AHRS AHRS句柄
 * @return 姿态四元数数据数组
 */
static inline float *Get_AHRS_Quat(const AHRS_t *AHRS) {
	return AHRS->quat;
}

/**
 * @brief 获取AHRS标定时的重力加速度
 * @param[in] AHRS AHRS句柄
 * @return 重力加速度
 */
static inline float Get_AHRS_g_norm(const AHRS_t *AHRS) {
	return AHRS->g_norm;
}


/**
 * @brief 世界坐标系转换为机体坐标系
 * @param[in] vecEF 世界坐标系坐标
 * @param[out] vecBF 机体坐标系坐标
 * @param[in] q 四元数
 */
void EarthFrameToBodyFrame(const Rectangular_Coordinate_t vecEF,
	Rectangular_Coordinate_t *vecBF, float q[4]);

/**
 * @brief 机体坐标系转换为世界坐标系
 * @param[in] vecEF 机体坐标系坐标
 * @param[out] vecBF 世界坐标系坐标
 * @param[in] q 四元数
 */
void BodyFrameToEarthFrame(const Rectangular_Coordinate_t vecBF,
	Rectangular_Coordinate_t *vecEF, float q[4]);