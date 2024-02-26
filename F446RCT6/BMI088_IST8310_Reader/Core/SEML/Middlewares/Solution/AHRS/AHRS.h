/**
 ******************************************************************************
 * @copyright (c) 2023 - ~, Singularity
 * @file   : AHRS.h
 * @author : Chad SY7_yellow
 * @brief  : AHRS模块
 * @date   : 2023-9-26
 * @par Change Log：
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2023-03-07     <td>0.1         <td>Chad  			<td>创建初始版本
 * <tr><td>2023-09-26     <td>1.0         <td>7_yellow    <td>移植到SEML库
 * </table>
 * @bug     : 不能融合磁力计，需要进行矫正
 * @todo    : 加入里程计和估测速度
 * @details :
 * ============================================================================
 *                       How to use this driver
 * ============================================================================
 * 1. 初始化IMU,磁力计后调用AHRS_Init初始化AHRS解算模块,如果没有使用磁力计可传入NULL
 * 不融合磁力计.
 * 2. 调用Calibrate_IMU_Offset矫正角速度零漂,校准成功返回SEML_OK,传感器故障返回
 * SEML_ERROR.校准超时返回SEML_TIMEOUT,此时可以使用以往矫正数据,需注意零飘受到温度和
 * IMU个体差异影响,其校准数据不可复用到其他单元.
 * 3. 定期调用AHRS_Update更新AHRS数据.
 ******************************************************************************
 * @attention:
 *
 * 文件编码：UTF-8,出现乱码请勿上传! \n
 * 修改后测试没问题记得修改版本号,未经过测试的请使用加上后缀alpha,beta...并且请
 * 勿合并到master. \n
 * 防御性编程,对输入参数做有效性检查,并返回错误号. \n
 * 不要包含太多不相关的头文件. \n
 * 若发现bug请提交issue,详细描述现象(必须)和复现条件(选填),以便对应的负责人能
 * 够准确定位修复. \n
 * 若是存在多线程同时编辑的情况，请使用互斥锁防止某进程编辑时候被其他进程访问。
 * File encoding:UTF-8,Do not upload garbled code!\n
 * Please remember to change the version number. If you have not been tested,
 * please use the suffix alpha,beta... And do not merge to master. \n
 * Defensive programming, where input arguments are checked for validity and
 * an error number is returned. \n
 * Don't include too many irrelevant headers. \n
 * If you find a bug, file an issue with a detailed description of the
 * phenomenon (required) and conditions for reoccurrence (optional) so that
 * the appropriate owner can locate the correct fix. \n
 * In the case of simultaneous editing by multiple threads, use a mutex to
 * prevent one process from being accessed by other processes while it is
 * editing. \n
 ******************************************************************************
 */
#ifndef __AHRS_H_
#define __AHRS_H_
#include "SEML_common.h"
#include "../../math/math_common.h"
/**
 * @addtogroup 解算库
 * @{
 */
/**
 * @addtogroup 航姿参考系统解算模块
 * @{
 */
/**
 * @brief 直角坐标系结构体
 */
typedef struct
{
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
typedef struct
{
	float pitch;
	float roll;
	float yaw;
} Euler_Data_t;

/**
 * @brief AHRS解算算法回调函数
 */
typedef void (*AHRS_Calculate_fun_t)(float quat[4], float sample_time, Accel_Data_t *accel, Gyro_Data_t *gyro, Mag_Data_t *mag);

/**
 * @brief 获取IMU数据回调函数
 */
typedef void (*Get_IMU_fun_t)(void *config, Accel_Data_t *accel, Gyro_Data_t *gyro);

/**
 * @brief 获取磁力计数据回调函数
 */
typedef void (*Get_Mag_fun_t)(void *config, Mag_Data_t *mag);

/**
 * @brief AHRS句柄
 */
typedef struct
{
	Euler_Data_t euler_angle;								 /**< 欧拉角 */
	Accel_Data_t accel;											 /**< 加速度 */
	Accel_Data_t real_accel;								 /**< 惯性坐标系下的加速度 */
	Gyro_Data_t gyro;												 /**< 角速度 */
	Mag_Data_t mag;													 /**< 磁场 */
	float quat[4];													 /**< 姿态四元数 */
	float g_norm;														 /**< 标定时的重力加速度 */
	float sample_time;											 /**< 采样时间 */
	Gyro_Data_t gyro_offset;								 /**< 角速度零偏校正值 */
	AHRS_Calculate_fun_t AHRS_Calculate_fun; /**< AHRS解算算法 */
	void *IMU_handle;												 /**< IMU句柄*/
	Get_IMU_fun_t Get_IMU_fun;							 /**< 获取IMU数据函数 */
	void *mag_handle;												 /**< 磁力计句柄*/
	Get_Mag_fun_t Get_Mag_fun;							 /**< 获取磁力计数据函数 */
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
void AHRS_Init(AHRS_t *AHRS, float sample_time, AHRS_Calculate_fun_t AHRS_Calculate_fun, Get_IMU_fun_t Get_IMU_fun, void *IMU_handle, Get_Mag_fun_t Get_Mag_fun, void *mag_handle);

/**
 * @brief 矫正角速度零飘
 * @param[in,out] AHRS AHRS句柄
 * @return 函数执行状态
 *  - SEML_OK 矫正成功
 * 	- SEML_TIMEOUT 矫正超时
 *  - SEML_ERROR 矫正失败(IMU数据异常)
 */
SEML_StatusTypeDef Calibrate_IMU_Offset(AHRS_t *AHRS);

/**
 * @brief 更新AHRS数据
 * @param[in,out] AHRS AHRS句柄
 */
void AHRS_Update(AHRS_t *AHRS);

/**
 * @brief 获取AHRS欧拉角数据
 * @param[in,out] AHRS AHRS句柄
 * @return 欧拉角数据结构体
 */
#define Get_AHRS_Euler(AHRS) (AHRS->euler_angle)

/**
 * @brief 获取AHRS加速度数据
 * @param[in,out] AHRS AHRS句柄
 * @return 加速度数据结构体
 */
#define Get_AHRS_Accel(AHRS) (AHRS->accel)

/**
 * @brief 获取AHRS角速度数据
 * @param[in,out] AHRS AHRS句柄
 * @return 角速度数据结构体
 */
#define Get_AHRS_Gyro(AHRS) (AHRS->gyro)

/**
 * @brief 获取AHRS磁场数据
 * @param[in,out] AHRS AHRS句柄
 * @return 磁场数据结构体
 */
#define Get_AHRS_Mag(AHRS) (AHRS->mag)

/**
 * @brief 获取AHRS四元数数据
 * @param[in,out] AHRS AHRS句柄
 * @return 姿态四元数数据数组
 */
#define Get_AHRS_Quat(AHRS) (AHRS->quat)

/**
 * @brief 获取AHRS标定时的重力加速度
 * @param[in,out] AHRS AHRS句柄
 * @return 重力加速度
 */
#define Get_AHRS_g_norm(AHRS) (AHRS->gnorm)

/**
 * @brief 世界坐标系转换为机体坐标系
 * @param[in] vecEF 世界坐标系坐标
 * @param[out] vecBF 机体坐标系坐标
 * @param[in] q 四元数
 */
void EarthFrameToBodyFrame(const Rectangular_Coordinate_t vecEF, Rectangular_Coordinate_t *vecBF, float q[4]);

/**
 * @brief 机体坐标系转换为世界坐标系
 * @param[in] vecEF 机体坐标系坐标
 * @param[out] vecBF 世界坐标系坐标
 * @param[in] q 四元数
 */
void BodyFrameToEarthFrame(const Rectangular_Coordinate_t vecBF, Rectangular_Coordinate_t *vecEF, float q[4]);

/**
 * @} 航姿参考系统解算模块
 */
/**
 * @} 解算库
 */
#endif
