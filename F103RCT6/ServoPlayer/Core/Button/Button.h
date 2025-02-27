#pragma once

#include <functional>

#include "PortPinPair.h"

/**
 * @brief Button 类，用于处理按键输入
 */
class Button {
public:
	/**
	 * @brief 按键状态枚举
	 */
	enum class State {
		Released,   // 未按下
		Pressed,    // 已按下
		Triggered   // 已触发
	};

	/**
	 * @brief 构造函数，初始化按键
	 * @param portPin 按键的端口和引脚
	 * @param longPressDuration 长按触发时间（毫秒）
	 */
	explicit Button(const PortPinPair &portPin, uint32_t longPressDuration = 500);

	/**
	 * @brief 在中断回调中执行，更新按键状态
	 * @param GPIO_Pin 触发中断的 GPIO 引脚
	 * @return 是否更新成功
	 */
	bool HandleInterrupt(uint16_t GPIO_Pin);

	/**
	 * @brief 在定时器中调用，处理长按和短按逻辑
	 * @param deltaTime 距离上次调用的时间间隔（毫秒）
	 */
	void Tick(uint32_t deltaTime = 1);

	/**
	 * @brief 注册按下回调函数
	 * @param callback 回调函数
	 */
	void RegisterPressCallback(const std::function<void()> &callback);

	/**
	 * @brief 注册释放回调函数
	 * @param callback 回调函数
	 */
	void RegisterReleaseCallback(const std::function<void()> &callback);

	/**
	 * @brief 注册短按回调函数
	 * @param callback 回调函数
	 */
	void RegisterShortPressCallback(const std::function<void()> &callback);

	/**
	 * @brief 注册长按回调函数
	 * @param callback 回调函数
	 */
	void RegisterLongPressCallback(const std::function<void()> &callback);

	/**
	 * @brief 获取当前按键状态
	 * @return 当前按键状态
	 */
	State GetState() const;

private:
	PortPinPair _portPin;  // 按键的端口和引脚
	State _currentState;   // 当前按键状态
	uint32_t _pressDuration;  // 按下持续时间
	uint32_t _longPressDuration;  // 长按触发时间

	std::function<void()> _pressCallback;     // 按下回调函数
	std::function<void()> _releaseCallback;   // 释放回调函数
	std::function<void()> _shortPressCallback;  // 短按回调函数
	std::function<void()> _longPressCallback;   // 长按回调函数

	/**
	 * @brief 更新按键状态并触发回调
	 * @param newState 新的按键状态
	 */
	void UpdateStateAndTriggerCallback(State newState);
};
