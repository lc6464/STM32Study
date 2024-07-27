#include "Mahony.h"

#define twoKpDef (2.0f * 0.5f) // 2 * proportional gain
#define twoKiDef (2.0f * 0.0f) // 2 * integral gain

const float twoKp = twoKpDef * 8; // 2 * proportional gain (Kp)
const float twoKi =
twoKiDef * 1; // 2 * integral gain (Ki)
// // quaternion of sensor frame relative to auxiliary frame
float integralFBx = 0.0f, integralFBy = 0.0f,
integralFBz = 0.0f; // integral error terms scaled by Ki
#ifndef _MATH_COMMON_H_
#define IS_NAN_INF(x) (!isnan(x) || !isinf(x))
static float math_invsqrt(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long *)&y;
	i = 0x5f375a86 - (i >> 1);
	y = *(float *)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}
static float math_sqrt(float x) { return 1 / math_invsqrt(x); }
#endif


void Mahony_AHRS_Update(float quat[4], float sample_time, Accel_Data_t *accel,
	Gyro_Data_t *gyro, Mag_Data_t *mag) {
	// 将传入的加速度计、角速度计和磁力计数据结构解引用，做局部副本
	Accel_Data_t accel_ = *accel;
	Gyro_Data_t gyro_ = *gyro;
	Mag_Data_t mag_ = *mag;

	// 定义一些局部变量用于后续计算
	float accelInvNorm, accel_norm;
	float mamInvNorm;
	float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
	float hx, hy, bx, bz;
	float halfvx, halfvy, halfvz, halfwx = 0, halfwy = 0, halfwz = 0;
	float halfex = 0, halfey = 0, halfez = 0;
	float qa, qb, qc;

	// 只在角速度计数据有效时才进行运算
	if (IS_NAN_INF(gyro_.x) || IS_NAN_INF(gyro_.y) || IS_NAN_INF(gyro_.z))
		return;

	// 只在加速度计数据有效时才进行运算
	if (accel != NULL)
		if (!(IS_NAN_INF(accel_.x) || IS_NAN_INF(accel_.y) ||
			IS_NAN_INF(accel_.z)) &&
			(accel_.x != 0 || accel_.y != 0 || accel_.z != 0)) {

			// 四元数各分量的平方和乘积
			q0q0 = quat[0] * quat[0];
			q0q1 = quat[0] * quat[1];
			q0q2 = quat[0] * quat[2];
			q0q3 = quat[0] * quat[3];
			q1q1 = quat[1] * quat[1];
			q1q2 = quat[1] * quat[2];
			q1q3 = quat[1] * quat[3];
			q2q2 = quat[2] * quat[2];
			q2q3 = quat[2] * quat[3];
			q3q3 = quat[3] * quat[3];

			// 将加速度计得到的实际重力加速度向量v单位化
			accelInvNorm = math_invsqrt(accel_.x * accel_.x + accel_.y * accel_.y +
				accel_.z * accel_.z);
			accel_.x *= accelInvNorm;
			accel_.y *= accelInvNorm;
			accel_.z *= accelInvNorm;

			halfvx = q1q3 - q0q2;
			halfvy = q0q1 + q2q3;
			halfvz = q0q0 - 0.5f + q3q3;

			// 只在磁力计数据有效时才进行运算
			if (mag != NULL)
				if ((mag_.x != 0.0f || mag_.y != 0.0f || mag_.z != 0.0f) &&
					!(IS_NAN_INF(mag_.x) || IS_NAN_INF(mag_.y) || IS_NAN_INF(mag_.z))) {

					// 将磁力计得到的实际磁场向量m单位化
					mamInvNorm =
						math_invsqrt(mag_.x * mag_.x + mag_.y * mag_.y + mag_.z * mag_.z);
					mag_.x *= mamInvNorm;
					mag_.y *= mamInvNorm;
					mag_.z *= mamInvNorm;

					// 通过磁力计测量值与坐标转换矩阵得到大地坐标系下的理论地磁向量
					hx = 2.0f * (mag_.x * (0.5f - q2q2 - q3q3) + mag_.y * (q1q2 - q0q3) +
						mag_.z * (q1q3 + q0q2));
					hy = 2.0f * (mag_.x * (q1q2 + q0q3) + mag_.y * (0.5f - q1q1 - q3q3) +
						mag_.z * (q2q3 - q0q1));
					bx = math_sqrt(hx * hx + hy * hy);
					bz = 2.0f * (mag_.x * (q1q3 - q0q2) + mag_.y * (q2q3 + q0q1) +
						mag_.z * (0.5f - q1q1 - q2q2));

					// 将理论重力加速度向量与理论地磁向量变换至机体坐标系
					halfwx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
					halfwy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
					halfwz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2);
				}

			// 计算加速度的模长
			accel_norm = 1 / accelInvNorm;

			// 加速度和标定时候的重力加速度的差别过大,认为加速度不可信.
			if (accel_norm > g_norm - 0.05 && accel_norm < g_norm + 0.05) {
				// 通过向量外积得到重力加速度向量和地磁向量的实际值与测量值之间误差
				halfex = (accel_.y * halfvz - accel_.z * halfvy) +
					(mag_.y * halfwz - mag_.z * halfwy);
				halfey = (accel_.z * halfvx - accel_.x * halfvz) +
					(mag_.z * halfwx - mag_.x * halfwz);
				halfez = (accel_.x * halfvy - accel_.y * halfvx) +
					(mag_.x * halfwy - mag_.y * halfwx);

				// 在PI补偿器中积分项使能情况下计算并应用积分项
				if (twoKi > 0.0f) {
					// 对误差进行积分，并乘以积分增益Ki
					integralFBx += twoKi * halfex * sample_time;
					integralFBy += twoKi * halfey * sample_time;
					integralFBz += twoKi * halfez * sample_time;

					// 应用误差补偿中的积分项
					gyro_.x += integralFBx;
					gyro_.y += integralFBy;
					gyro_.z += integralFBz;
				} else {
					// 避免为负值的Ki时积分异常饱和，直接将积分设为0
					integralFBx = 0.0f;
					integralFBy = 0.0f;
					integralFBz = 0.0f;
				}

				// 应用误差补偿中的比例项
				gyro_.x += twoKp * halfex;
				gyro_.y += twoKp * halfey;
				gyro_.z += twoKp * halfez;
			}
		}

	// 微分方程迭代求解，更新四元数
	gyro_.x *= 0.5f * sample_time; // 预乘公共因子
	gyro_.y *= 0.5f * sample_time;
	gyro_.z *= 0.5f * sample_time;
	qa = quat[0];
	qb = quat[1];
	qc = quat[2];
	quat[0] += (-qb * gyro_.x - qc * gyro_.y - quat[3] * gyro_.z);
	quat[1] += (+qa * gyro_.x + qc * gyro_.z - quat[3] * gyro_.y);
	quat[2] += (+qa * gyro_.y - qb * gyro_.z + quat[3] * gyro_.x);
	quat[3] += (+qa * gyro_.z + qb * gyro_.y - qc * gyro_.x);

	// 单位化四元数 保证四元数在迭代过程中保持单位性质
	accelInvNorm = math_invsqrt(quat[0] * quat[0] + quat[1] * quat[1] +
		quat[2] * quat[2] + quat[3] * quat[3]);
	quat[0] *= accelInvNorm;
	quat[1] *= accelInvNorm;
	quat[2] *= accelInvNorm;
	quat[3] *= accelInvNorm;
}