#pragma once

#include "tim.h"

/**
 * @brief 编码器类，适用于不超过16位的编码器模式定时器
 */
class Encoder {
public:
  /**
   * @brief 构造函数，初始化编码器
   * @param htim 编码器定时器句柄指针
   */
  Encoder(TIM_HandleTypeDef *htim);

  /**
   * @brief 启动定时器编码器
   * @return 启动状态
   */
  HAL_StatusTypeDef Start();

  /**
   * @brief 停止定时器编码器
   * @return 停止状态
   */
  HAL_StatusTypeDef Stop();

  /**
   * @brief 更新编码器数据
   * @note 定时调用，如在 HAL_TIM_PeriodElapsedCallback 函数中调用
   * @return 当前输出速度
   */
  float Update();

  /**
   * @brief 编码器溢出回调函数
   * @param htim 中断的定时器句柄指针
   * @return 是否溢出
   * @note 在 HAL_TIM_PeriodElapsedCallback 函数中无条件调用
   */
  bool OverflowCallback(const TIM_HandleTypeDef *htim);

  /**
   * @brief 获取编码器差值
   * @return 当前编码器差值
   */
  float GetEncoderDiff() const { return _encoderDiff; }

  /**
   * @brief 获取电机转速
   * @return 电机转速（单位：rpm）
   */
  float GetMotorRPM() const { return _motorRPM; }

  /**
   * @brief 获取输出转速
   * @return 输出转速（单位：rpm）
   */
  float GetOutputRPM() const { return _outputRPM; }

private:
  static constexpr float FILTER_ALPHA = 0.3f; // 低通滤波器系数
  static constexpr float PULSES_PER_REVOLUTION =
      11.0f * 4.0f; // 每转脉冲数，考虑正交编码器系数 4
  static constexpr float GEAR_RATIO = 9.6f; // 减速比

  TIM_HandleTypeDef *_htim; // 定时器句柄指针
  uint16_t _lastCount;      // 上一次的计数值，实际不会超过 65535
  int8_t _overflowCount;    // 溢出计数
  uint32_t _lastTime;       // 上次运行时间
  bool _isStopped;          // 是否停止
  uint32_t _lastUpdateTime; // 上次更新时间

  float _encoderDiff; // 编码器差值
  float _motorRPM;    // 电机转速（rpm）
  float _outputRPM;   // 输出转速（rpm）
};