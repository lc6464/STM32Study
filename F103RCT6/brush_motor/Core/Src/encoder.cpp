#include "encoder.h"

/**
 * @brief 构造函数，初始化编码器
 * @param htim 编码器定时器句柄指针
 */
Encoder::Encoder(TIM_HandleTypeDef *htim)
    : _htim(htim), _lastCount(0), _overflowCount(0), _lastTime(0), _speed(0),
      _isStopped(false), _lastUpdateTime(0) {
  assert_param(htim != nullptr);
}

/**
 * @brief 启动定时器编码器
 * @return 启动状态
 */
HAL_StatusTypeDef Encoder::Start() {
  __HAL_TIM_ENABLE_IT(_htim, TIM_IT_UPDATE); // 启用溢出中断

  __HAL_TIM_SET_COUNTER(_htim, 0); // 重置编码器定时器

  _lastCount = 0;
  _overflowCount = 0;
  _speed = 0;
  _isStopped = false;
  _lastTime = HAL_GetTick();
  _lastUpdateTime = _lastTime;

  return HAL_TIM_Encoder_Start(_htim, TIM_CHANNEL_ALL);
}

/**
 * @brief 停止定时器编码器
 * @return 停止状态
 */
HAL_StatusTypeDef Encoder::Stop() {
  __HAL_TIM_DISABLE_IT(_htim, TIM_IT_UPDATE); // 禁用溢出中断

  return HAL_TIM_Encoder_Stop(_htim, TIM_CHANNEL_ALL);
}

/**
 * @brief 更新编码器数据
 * @note 定时调用，如在 HAL_TIM_PeriodElapsedCallback 函数中调用
 */
void Encoder::Update() {
  // 读取当前计数值和时间
  uint16_t currentCount = __HAL_TIM_GET_COUNTER(_htim);
  uint32_t currentTime = HAL_GetTick();
  uint8_t isTimeOverflow = currentTime < _lastTime;

  // 计算计数值位移
  int32_t diff =
      static_cast<int32_t>(currentCount) - static_cast<int32_t>(_lastCount);

  // 处理溢出情况
  diff += static_cast<int32_t>(_overflowCount) *
          static_cast<int32_t>(_htim->Instance->ARR + 1);
  _overflowCount = 0;

  // 更新最后一次的计数和时间
  _lastCount = currentCount;
  _lastTime = currentTime;

  if (diff == 0) {
    if (!_isStopped) { // 已设为停止状态则直接 return

      // 检查是否超过 200ms 没有更新
      uint64_t timeSinceLastUpdate =
          static_cast<uint64_t>(currentTime) +
          (static_cast<uint64_t>(isTimeOverflow) * UINT32_MAX) -
          static_cast<uint64_t>(_lastUpdateTime);
      if (timeSinceLastUpdate >= 200) {
        _lastCount = 0;
        _overflowCount = 0;
        _speed = 0;
        _isStopped = true;

        // 重置编码器定时器
        __HAL_TIM_SET_COUNTER(_htim, 0);
      }
    }
    return;
  }

  // 处理停止状态
  _isStopped = false;
  _lastUpdateTime = currentTime;

  // 计算速度（这里直接使用diff作为原始速度）
  float rawSpeed = static_cast<float>(diff);

  // 应用低通滤波
  _speed = _speed * (1.0f - FILTER_ALPHA) + rawSpeed * FILTER_ALPHA;
}

/**
 * @brief 编码器溢出回调函数
 * @param htim 定时器句柄指针
 * @note 在 HAL_TIM_PeriodElapsedCallback 函数中无条件调用
 */
void Encoder::OverflowCallback(const TIM_HandleTypeDef *htim) {
  assert_param(htim != nullptr);

  // 如果是当前编码器的定时器
  if (_htim->Instance == htim->Instance) {
    // 检查方向
    if (__HAL_TIM_IS_TIM_COUNTING_DOWN(_htim)) {
      _overflowCount--; // 向下计数时溢出
    } else {
      _overflowCount++; // 向上计数时溢出
    }
  }
}