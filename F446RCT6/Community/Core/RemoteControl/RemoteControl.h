#pragma once

#include <array>
#include <cstdint>
#include <optional>

#include "usart.h"
#include "WatchDog.h"

class RemoteControl {
public:
	/**
	 * @brief 遥控器状态枚举
	 */
	enum class Status {
		Running,
		Timeout,
		Error,
		Stopped,
		Unknown
	};

	/**
	 * @brief 遥控器拨杆位置
	 */
	enum class SwitchPosition {
		Unknown = 0,
		Up = 1,
		Middle = 3,
		Down = 2
	};

	/**
	 * @brief 遥控器控制器数据结构
	 */
	struct ControllerData {
		int16_t RightStickX;
		int16_t RightStickY;
		int16_t LeftStickX;
		int16_t LeftStickY;
		SwitchPosition LeftSwitch;
		SwitchPosition RightSwitch;
		int16_t Dial;
	};

	/**
	 * @brief 构造函数
	 * @param huart 串口句柄
	 */
	explicit RemoteControl(UART_HandleTypeDef *huart);

	/**
	 * @brief 开始接收数据
	 */
	void Start();

	/**
	 * @brief 停止接收数据
	 */
	void Stop();

	/**
	 * @brief 接收回调函数
	 * @param huart 接收句柄
	 * @return 是否接收成功
	 */
	bool RxCallback(const UART_HandleTypeDef *huart);

	/**
	 * @brief 看门狗函数，检查是否超时
	 * @return 是否超时
	 */
	bool Tick();

	/**
	 * @brief 获取当前遥控器状态
	 * @return 遥控器状态
	 */
	Status GetStatus() const { return _status; }

	/**
	 * @brief 获取遥控器数据
	 * @return 如果状态正常，返回遥控器数据；否则返回std::nullopt
	 */
	std::optional<ControllerData> GetControllerData() const;

	/**
	 * @brief 重载 >> 操作符，用于获取控制器数据
	 * @param controllerData 用于存储获取到的数据
	 * @return 是否成功获取数据
	 */
	bool operator>>(ControllerData &controllerData);

private:
	static constexpr size_t BUFFER_SIZE = 18;
	static constexpr uint32_t TIMEOUT_DURATION = 50; // ms

	std::array<uint8_t, BUFFER_SIZE> _buffer{};
	ControllerData _controllerData;
	UART_HandleTypeDef *_huart;
	Status _status;

	WatchDog _watchDog;

	void Reset();
	void RxTimeoutCallback();
	bool RxCallbackErrorHandler();
	bool ParseReceivedData();

	static bool IsValidChannelData(int16_t data) {
		return data >= -660 && data <= 660;
	}
};