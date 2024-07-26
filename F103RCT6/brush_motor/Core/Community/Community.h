#pragma once

#include <functional>

#include "can.h"

typedef std::function<bool(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData)>
    Community_RxCallback;

class Community {
public:
  /**
   * @brief 构造函数
   * @param hcan CAN 句柄指针
   * @param rxCallback 接收回调函数
   */
  explicit Community(CAN_HandleTypeDef *hcan,
                     Community_RxCallback rxCallback = nullptr)
      : _hcan(hcan), _rxCallback(rxCallback) {}

  /**
   * @brief 设置接收回调函数
   * @param callback 接收回调函数
   */
  void RegisterRxCallback(Community_RxCallback callback) {
    assert_param(callback != nullptr);

    _rxCallback = callback;
  }

  /**
   * @brief 按默认配置初始化并启动 CAN
   */
  void Start();

  /**
   * @brief 初始化并启动 CAN
   * @param filterConfig CAN 过滤器配置
   */
  void Start(const CAN_FilterTypeDef &filterConfig);

  /**
   * @brief 停止 CAN
   */
  void Stop();

  /**
   * @brief 发送数据
   * @param txHeader CAN 发送帧头
   * @param txData 发送数据
   * @param mailbox 发送邮箱
   */
  HAL_StatusTypeDef Transmit(CAN_TxHeaderTypeDef *txHeader, uint8_t *txData,
                             uint32_t *mailbox);

  /**
   * @brief 发送电机速度
   * @param canId CAN ID
   * @param mailbox 发送邮箱
   * @param left 左速度
   * @param right 右速度
   * @return 发送状态
   */
  HAL_StatusTypeDef SendSpeed(uint16_t canId, uint32_t *mailbox, float left,
                              float right);

  /**
   * @brief 执行接收回调函数
   * @param rxHeader CAN 接收帧头
   * @param rxData 接收数据
   * @return 是否执行了回调函数
   * @note 在 CAN 接收中断回调函数中无条件调用
   */
  bool ExecuteRxCallback(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData);

private:
  CAN_HandleTypeDef *_hcan;         // CAN 句柄指针
  Community_RxCallback _rxCallback; // 接收回调函数
};