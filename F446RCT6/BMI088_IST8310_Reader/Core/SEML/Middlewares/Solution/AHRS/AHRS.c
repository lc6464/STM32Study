#include "AHRS.h"

#define GxOFFSET 0.00055712f
#define GyOFFSET -0.0056608f
#define GzOFFSET -0.0009765f
#define gNORM 9.82609f

float g_norm = gNORM;

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
void AHRS_Init(AHRS_t *AHRS, float sample_time, AHRS_Calculate_fun_t AHRS_Calculate_fun, Get_IMU_fun_t Get_IMU_fun, void *IMU_handle, Get_Mag_fun_t Get_Mag_fun, void *mag_handle)
{
	Rectangular_Coordinate_t temp = {
			.x = 0,
			.y = 0,
			.z = 0};
	AHRS->accel = temp;
	AHRS->gyro = temp;
	AHRS->mag = temp;
	AHRS->gyro_offset.x = GxOFFSET;
	AHRS->gyro_offset.y = GyOFFSET;
	AHRS->gyro_offset.z = GzOFFSET;
	AHRS->euler_angle.pitch = 0;
	AHRS->euler_angle.yaw = 0;
	AHRS->euler_angle.roll = 0;
	AHRS->sample_time = sample_time;
	AHRS->quat[0] = 1.0f;
	AHRS->quat[1] = 0.0f;
	AHRS->quat[2] = 0.0f;
	AHRS->quat[3] = 0.0f;
	AHRS->g_norm = gNORM;
	AHRS->AHRS_Calculate_fun = AHRS_Calculate_fun;
	AHRS->Get_IMU_fun = Get_IMU_fun;
	AHRS->IMU_handle = IMU_handle;
	AHRS->Get_Mag_fun = Get_Mag_fun;
	AHRS->mag_handle = mag_handle;
}

/**
 * @brief 矫正角速度零飘
 * @param[in,out] AHRS AHRS句柄
 * @return 函数执行状态
 *  - SEML_OK 矫正成功
 * 	- SEML_TIMEOUT 矫正超时
 *  - SEML_ERROR 矫正失败(IMU数据异常)
 */
SEML_StatusTypeDef Calibrate_IMU_Offset(AHRS_t *AHRS)
{
	// 标定数据量 需要足够多的数据才能得到有效陀螺仪零偏校准结果
	const uint16_t CaliTimes = 1000;
	uint32_t start_time = SEML_GetTick(), lest_time;
	uint16_t error_count = 0, restart = 0;
	float gNormTemp, k;
	start_time = SEML_GetTick();
	do
	{
		// 校准超时
		if (SEML_GetTick() - start_time > 60000)
		{
			AHRS->gyro_offset.x = GxOFFSET;
			AHRS->gyro_offset.y = GyOFFSET;
			AHRS->gyro_offset.z = GzOFFSET;
			AHRS->g_norm = gNORM;
			return SEML_TIMEOUT;
		}
		// 数据初始化
		AHRS->g_norm = 0;
		AHRS->gyro_offset.x = 0;
		AHRS->gyro_offset.y = 0;
		AHRS->gyro_offset.z = 0;
		k = 0;
		error_count = 0;
		restart = 0;
		// 读取数据进行矫正
		for (uint16_t i = 0; i < CaliTimes; i++)
		{
			lest_time = SEML_GetTick();
			// 更新矫正进度
			AHRS->count = 100 * (i + 1) / CaliTimes;
			// 获取数据
			AHRS->Get_IMU_fun(AHRS->IMU_handle, &AHRS->accel, &AHRS->gyro);
			// 数据错误检查
			if (IS_NAN_INF(AHRS->accel.x) || IS_NAN_INF(AHRS->accel.y) || IS_NAN_INF(AHRS->accel.z))
				goto _Cali_Offset_Err_Handle;
			if (IS_NAN_INF(AHRS->gyro.x) || IS_NAN_INF(AHRS->gyro.y) || IS_NAN_INF(AHRS->gyro.z))
				goto _Cali_Offset_Err_Handle;
			// 计算加速度
			gNormTemp = math_sqrt(AHRS->accel.x * AHRS->accel.x +
														AHRS->accel.y * AHRS->accel.y +
														AHRS->accel.z * AHRS->accel.z);
			// 若数据出现异常值 舍弃本次矫正数据,重新读取
			if (i != 0 && (abs(AHRS->g_norm - gNormTemp) > 0.5f ||
										 abs(AHRS->gyro_offset.x - AHRS->gyro.x) > 0.05f ||
										 abs(AHRS->gyro_offset.y - AHRS->gyro.y) > 0.05f ||
										 abs(AHRS->gyro_offset.z - AHRS->gyro.z) > 0.05f))
			{
				i--;
				error_count++;
				// 出现多次错误认为数据不可信 重新矫正
				if (error_count > 100)
					{
						restart = 1;
					break;
					}
				SEML_Delay(20);
				continue;
			}
			// 更新标定平均值
			AHRS->g_norm = gNormTemp * (1 - k) + AHRS->g_norm * k;
			AHRS->gyro_offset.x = AHRS->gyro.x * (1 - k) + AHRS->gyro_offset.x * k;
			AHRS->gyro_offset.y = AHRS->gyro.y * (1 - k) + AHRS->gyro_offset.y * k;
			AHRS->gyro_offset.z = AHRS->gyro.z * (1 - k) + AHRS->gyro_offset.z * k;
			k = (i + 1.0f) / (i + 2.0f);
			if (SEML_GetTick() - lest_time == 0)
				SEML_Delay(1);
		}

		// 检查IMU是否工作
		if (AHRS->g_norm == 0.0f)
			goto _Cali_Offset_Err_Handle;
	} while (restart);
	return SEML_OK;
	// 错误处理
_Cali_Offset_Err_Handle:
	AHRS->gyro_offset.x = GxOFFSET;
	AHRS->gyro_offset.y = GyOFFSET;
	AHRS->gyro_offset.z = GzOFFSET;
	AHRS->g_norm = gNORM;
	return SEML_ERROR;
}

#include "../../math/math_filter.h"
/**
 * @brief 更新AHRS数据
 * @param[in,out] AHRS AHRS句柄
 */
void AHRS_Update(AHRS_t *AHRS)
{
	// 获取数据
	if (AHRS->Get_IMU_fun != NULL)
		AHRS->Get_IMU_fun(AHRS->IMU_handle, &AHRS->accel, &AHRS->gyro);
	if (AHRS->Get_Mag_fun != NULL)
		AHRS->Get_Mag_fun(AHRS->mag_handle, &AHRS->mag);
	else
	{
		AHRS->mag.x = 0.0f;
		AHRS->mag.y = 0.0f;
		AHRS->mag.z = 0.0f;
	}
	// 矫正零飘
	AHRS->gyro.x -= AHRS->gyro_offset.x;
	AHRS->gyro.y -= AHRS->gyro_offset.y;
	AHRS->gyro.z -= AHRS->gyro_offset.z;
	// 数据融合
	g_norm = AHRS->g_norm;
	AHRS->AHRS_Calculate_fun(AHRS->quat, AHRS->sample_time, &AHRS->accel, &AHRS->gyro, &AHRS->mag);
	// 转换为欧拉角
	AHRS_Get_Angle(AHRS->quat, &AHRS->euler_angle);
	// 转换为机体加速度
	static Accel_Data_t real_accel;
	BodyFrameToEarthFrame(AHRS->accel, &real_accel, AHRS->quat);
	real_accel.z -= AHRS->g_norm;
	AHRS->real_accel.x = Lowpass(real_accel.x, AHRS->real_accel.x, 0.6);
	AHRS->real_accel.y = Lowpass(real_accel.y, AHRS->real_accel.y, 0.6);
	AHRS->real_accel.z = Lowpass(real_accel.z, AHRS->real_accel.z, 0.6);
}

/**
 * @brief 将四元数转换为欧拉角
 * @param[in] quat 四元数
 * @param[out] euler_angle 欧拉角(degree)
 */
void AHRS_Get_Angle(const float quat[4], Euler_Data_t *euler_angle)
{
	euler_angle->yaw = math_atan2(2.0f * (quat[0] * quat[3] + quat[1] * quat[2]), 2.0f * (quat[0] * quat[0] + quat[1] * quat[1]) - 1.0f);
	euler_angle->pitch = math_atan2(2.0f * (quat[0] * quat[1] + quat[2] * quat[3]), 2.0f * (quat[0] * quat[0] + quat[3] * quat[3]) - 1.0f);
	euler_angle->roll = asinf(-2.0f * (quat[1] * quat[3] - quat[0] * quat[2]));
}

/**
 * @brief 世界坐标系转换为机体坐标系
 * @param[in] vecEF 世界坐标系坐标
 * @param[out] vecBF 机体坐标系坐标
 * @param[in] q 四元数
 */
void EarthFrameToBodyFrame(const Rectangular_Coordinate_t vecEF, Rectangular_Coordinate_t *vecBF, float q[4])
{
	vecBF->x = 2.0f * ((0.5f - q[2] * q[2] - q[3] * q[3]) * vecEF.x +
										 (q[1] * q[2] + q[0] * q[3]) * vecEF.y +
										 (q[1] * q[3] - q[0] * q[2]) * vecEF.z);

	vecBF->y = 2.0f * ((q[1] * q[2] - q[0] * q[3]) * vecEF.x +
										 (0.5f - q[1] * q[1] - q[3] * q[3]) * vecEF.y +
										 (q[2] * q[3] + q[0] * q[1]) * vecEF.z);

	vecBF->z = 2.0f * ((q[1] * q[3] + q[0] * q[2]) * vecEF.x +
										 (q[2] * q[3] - q[0] * q[1]) * vecEF.y +
										 (0.5f - q[1] * q[1] - q[2] * q[2]) * vecEF.z);
}

/**
 * @brief 机体坐标系转换为世界坐标系
 * @param[in] vecEF 机体坐标系坐标
 * @param[out] vecBF 世界坐标系坐标
 * @param[in] q 四元数
 */
void BodyFrameToEarthFrame(const Rectangular_Coordinate_t vecBF, Rectangular_Coordinate_t *vecEF, float q[4])
{
	vecEF->x = 2.0f * ((0.5f - q[2] * q[2] - q[3] * q[3]) * vecBF.x +
										 (q[1] * q[2] - q[0] * q[3]) * vecBF.y +
										 (q[1] * q[3] + q[0] * q[2]) * vecBF.z);

	vecEF->y = 2.0f * ((q[1] * q[2] + q[0] * q[3]) * vecBF.x +
										 (0.5f - q[1] * q[1] - q[3] * q[3]) * vecBF.y +
										 (q[2] * q[3] - q[0] * q[1]) * vecBF.z);

	vecEF->z = 2.0f * ((q[1] * q[3] - q[0] * q[2]) * vecBF.x +
										 (q[2] * q[3] + q[0] * q[1]) * vecBF.y +
										 (0.5f - q[1] * q[1] - q[2] * q[2]) * vecBF.z);
}
