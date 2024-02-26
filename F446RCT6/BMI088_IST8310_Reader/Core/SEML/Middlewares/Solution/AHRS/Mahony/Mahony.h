#ifndef __MAHONY_H
#define __MAHONT_H
#include "../../../math/math_common.h"
#include "../AHRS.h"
#include "SEML_common.h"
/**
 * @addtogroup 解算库
 * @{
 */
/**
 * @addtogroup 航姿参考系统解算模块
 * @{
 */

/**
 * @brief AHRS解算更新(基于Mahony算法)
 * @param[in,out] quat 四元数
 * @param[in] sample_time 采样时间(S)
 * @param[in] accel 加速度(m/s^2)
 * @param[in] gyro 角加速度(rad/s)
 * @param[in] mag 磁场大小(uT)
 */
void Mahony_AHRS_Update(float quat[4], float sample_time, Accel_Data_t *accel,
                        Gyro_Data_t *gyro, Mag_Data_t *mag);

/**
 * @} 航姿参考系统解算模块
 */
/**
 * @} 解算库
 */
#endif
