#pragma once

class PIDController {
public:
	/**
	 * @brief 构造函数，初始化 PID 控制器
	 * @param kp 比例增益
	 * @param ki 积分增益
	 * @param kd 微分增益
	 * @param tau 微分低通滤波器时间常数
	 * @param limMin 输出最小值限制
	 * @param limMax 输出最大值限制
	 * @param limMinInt 积分器最小值限制
	 * @param limMaxInt 积分器最大值限制
	 * @param sampleTime 采样时间 (s)
	 */
	PIDController(float kp, float ki, float kd, float tau,
		float limMin, float limMax,
		float limMinInt, float limMaxInt,
		float sampleTime);

	/**
	 * @brief 更新 PID 控制器并计算新的控制输出
	 * @param target 目标值
	 * @param measurement 当前测量值
	 * @return 计算得到的控制输出
	 */
	float Update(float target, float measurement);

	/**
	 * @brief 重置 PID 控制器状态
	 */
	void Reset();

	/**
	 * @brief 重新设置 PID 控制器的参数
	 * @param kp 新的比例增益
	 * @param ki 新的积分增益
	 * @param kd 新的微分增益
	 * @param tau 新的微分低通滤波器时间常数
	 * @param limMin 新的输出最小值限制
	 * @param limMax 新的输出最大值限制
	 * @param limMinInt 新的积分器最小值限制
	 * @param limMaxInt 新的积分器最大值限制
	 * @param sampleTime 新的采样时间 (s)
	 */
	void SetParameters(float kp, float ki, float kd, float tau,
		float limMin, float limMax,
		float limMinInt, float limMaxInt,
		float sampleTime);

	/**
	 * @brief 获取 PID 控制器的输出
	 * @return PID 控制器的输出
	 */
	float GetOutput() const { return _out; }

private:
	// 控制器增益参数
	float _kp; // 比例增益
	float _ki; // 积分增益
	float _kd; // 微分增益

	float _tau; // 微分低通滤波器的时间常数

	// 输出限制
	float _limMin; // 输出最小值限制
	float _limMax; // 输出最大值限制

	// 积分器限制
	float _limMinInt; // 积分器最小值限制
	float _limMaxInt; // 积分器最大值限制

	float _T; // 采样时间(秒)

	// 控制器"记忆"变量
	float _integrator; // 积分器
	float _prevError; // 上一次的误差
	float _differentiator; // 微分器
	float _prevMeasurement; // 上一次的测量值

	float _out; ///< 控制器输出
};