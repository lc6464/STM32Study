#include "PID.hpp"

#include <algorithm>

PIDController::PIDController(float kp, float ki, float kd, float tau,
	float limMin, float limMax,
	float limMinInt, float limMaxInt,
	float sampleTime)
	: _kp(kp), _ki(ki), _kd(kd), _tau(tau),
	_limMin(limMin), _limMax(limMax),
	_limMinInt(limMinInt), _limMaxInt(limMaxInt),
	_T(sampleTime) {
	Reset();
}

float PIDController::Update(float target, float measurement) {
	// 计算误差
	float error = target - measurement;

	// 比例项
	float proportional = _kp * error;

	// 积分项
	_integrator += 0.5f * _ki * _T * (error + _prevError);

	// 积分限幅
	_integrator = std::clamp(_integrator, _limMinInt, _limMaxInt);

	// 微分项 (带限制的微分器)
	_differentiator = -(2.0f * _kd * (measurement - _prevMeasurement)
		+ (2.0f * _tau - _T) * _differentiator)
		/ (2.0f * _tau + _T);

	// 计算输出并应用限制
	_out = proportional + _integrator + _differentiator;
	_out = std::clamp(_out, _limMin, _limMax);

	// 存储本次误差和测量值，用于下次计算
	_prevError = error;
	_prevMeasurement = measurement;

	return _out;
}

void PIDController::Reset() {
	_integrator = 0.0f;
	_prevError = 0.0f;
	_differentiator = 0.0f;
	_prevMeasurement = 0.0f;
	_out = 0.0f;
}

void PIDController::SetParameters(float kp, float ki, float kd, float tau,
	float limMin, float limMax,
	float limMinInt, float limMaxInt,
	float sampleTime) {
	_kp = kp;
	_ki = ki;
	_kd = kd;
	_tau = tau;
	_limMin = limMin;
	_limMax = limMax;
	_limMinInt = limMinInt;
	_limMaxInt = limMaxInt;
	_T = sampleTime;

	// 重置控制器状态，以确保平滑地变换参数
	Reset();
}
