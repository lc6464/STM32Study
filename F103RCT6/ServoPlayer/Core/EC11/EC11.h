#pragma once

#include <functional>

#include "tim.h"

#include "Button.h"
#include "PortPinPair.h"

/**
 * @brief EC11 编码器类，用于读取编码器数据
 */
class EC11 {
public:
	/**
	 * @brief 构造函数，初始化编码器
	 * @param htim 编码器定时器句柄引用
	 * @param button 按键 GPIO 引用
	 */
	explicit EC11(TIM_HandleTypeDef &htim, Button &button)
		: _htim(htim), _button(button) { }

	/**
	 * @brief 析构函数
	 * @note 停止编码器，释放资源
	 */
	~EC11() { Stop(); }

	/**
	 * @brief 启动编码器
	 * @return 启动状态
	 */
	HAL_StatusTypeDef Start();

	/**
	 * @brief 停止编码器
	 * @return 停止状态
	 */
	HAL_StatusTypeDef Stop();

	/**
	 * @brief 更新编码器旋转数据
	 * @note 需要定期调用
	 * @return 旋转值
	 */
	int16_t UpdateRotation();

	/**
	 * @brief 注册逆时针旋转回调函数
	 * @param callback 回调函数
	 */
	void RegisterCounterClockwiseCallback(const std::function<void(uint16_t)> &callback) { _ccwCallback = callback; }

	/**
	 * @brief 注册顺时针旋转回调函数
	 * @param callback 回调函数
	 */
	void RegisterClockwiseCallback(const std::function<void(uint16_t)> &callback) { _cwCallback = callback; }

	/**
	 * @brief 获取按钮对象的引用
	 * @return 按钮对象的引用
	 */
	Button &GetButton() { return _button; }

private:
	static constexpr uint16_t INITIAL_COUNT = 32768;

	TIM_HandleTypeDef &_htim;  // 定时器句柄引用
	Button _button;            // 按键

	std::function<void(uint16_t)> _ccwCallback;  // 逆时针旋转回调函数
	std::function<void(uint16_t)> _cwCallback;   // 顺时针旋转回调函数
};
