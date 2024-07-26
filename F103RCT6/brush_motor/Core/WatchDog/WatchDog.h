#pragma once

#include <cstdint>
#include <functional>

/**
 * @brief 看门狗类
 */
class WatchDog {
public:
  /**
   * @brief 构造函数
   */
  explicit WatchDog(uint32_t timeout_ms,
                    std::function<void()> callback = nullptr);

  /**
   * @brief 喂狗方法
   * @param isFeed 是否喂狗
   */
  void Feed(bool isFeed = true);

  /**
   * @brief 看门狗计时
   * @param isTick 是否计时
   */
  void Tick(bool isTick = true);

  // 启用看门狗
  void Enable();

  // 禁用看门狗
  void Disable();

  /**
   * @brief 设置超时时间
   * @param new_timeout_ms 新的超时时间
   */
  void SetTimeout(uint32_t new_timeout_ms);

  /**
   * @brief 注册超时回调函数
   * @param new_callback 新的回调函数
   */
  void RegisterTimeoutCallback(std::function<void()> new_callback);

private:
  uint32_t _timeout;
  uint32_t _lastTime;
  std::function<void()> _timeoutCallback;
  bool _isEnabled;

  // 获取当前时间的方法（假设使用 HAL 库）
  uint32_t _getCurrentTime();
};