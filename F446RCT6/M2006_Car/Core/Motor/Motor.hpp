#pragma once

#include <functional>
#include "can.h"

class Motor {
public:
	/**
	 * @brief 电机反馈数据结构
	 */
	struct Feedback {
		uint16_t angle;  // 转子机械角度
		int16_t speed;   // 转子速度
		int16_t torque;  // 实际转矩
	};

	/**
	 * @brief 反馈回调函数类型
	 */
	using FeedbackCallback = std::function<void(const Feedback &)>;

	/**
	 * @brief 构造函数
	 * @param hcan CAN句柄引用
	 * @param motorId 电机ID（1-8）
	 * @param callback 反馈回调函数（可选）
	 */
	explicit Motor(CAN_HandleTypeDef &hcan, uint8_t motorId, FeedbackCallback callback = nullptr)
		: _hcan(hcan), _motorId(motorId), _currentSpeed(0), _currentCurrent(0), _callback(callback) {
		assert_param(motorId >= 1 && motorId <= 8);
	}

	/**
	 * @brief 析构函数
	 */
	~Motor() {
		Stop();
	}

	/**
	 * @brief 设置电机电流
	 * @param current 目标电流（单位：mA，范围：-8000 到 8000）
	 */
	void SetCurrent(int16_t current) {
		_currentCurrent = std::clamp(current, static_cast<int16_t>(-8000), static_cast<int16_t>(8000));
	}

	/**
	 * @brief 获取当前速度
	 * @return 当前速度
	 */
	[[nodiscard]] int16_t GetCurrentSpeed() const { return _currentSpeed; }

	/**
	 * @brief 获取当前电流
	 * @return 当前电流（单位：mA）
	 */
	[[nodiscard]] int16_t GetCurrentCurrent() const { return _currentCurrent; }

	/**
	 * @brief 启动 CAN 通信
	 */
	void Start();

	/**
	 * @brief 启动 CAN 通信
	 * @param filterConfig 过滤器配置
	 */
	void Start(const CAN_FilterTypeDef &filterConfig) {
		HAL_CAN_ConfigFilter(&_hcan, &filterConfig);
		HAL_CAN_Start(&_hcan);
		HAL_CAN_ActivateNotification(&_hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	}

	/**
	 * @brief 停止 CAN 通信
	 */
	void Stop() {
		HAL_CAN_Stop(&_hcan);
	}

	/**
	 * @brief 处理接收到的 CAN 数据
	 * @param rxHeader CAN 接收帧头引用
	 * @param rxData 接收数据指针
	 * @return 是否成功处理数据
	 */
	bool HandleRxData(const CAN_RxHeaderTypeDef &rxHeader, const uint8_t *rxData);

	/**
	 * @brief 发送控制命令（静态方法）
	 * @param motor1 电机1引用
	 * @param motor2 电机2引用（可选）
	 * @param motor3 电机3引用（可选）
	 * @param motor4 电机4引用（可选）
	 * @return 发送状态
	 * @note 如果某个电机不需要控制，可以传入 nullptr，默认为 nullptr
	 */
	static HAL_StatusTypeDef SendControlCommand(Motor *motor1, Motor *motor2 = nullptr, Motor *motor3 = nullptr, Motor *motor4 = nullptr);

private:
	static constexpr uint32_t CAN_CONTROL_ID_BASE = 0x200;   // 控制 ID 基址（1~4 号电机）
	static constexpr uint32_t CAN_CONTROL_ID_EXTEND = 0x1FF; // 控制 ID 扩展（5~8 号电机）

	CAN_HandleTypeDef &_hcan;       // CAN句柄引用
	const uint8_t _motorId;         // 电机ID
	int16_t _currentSpeed;          // 当前速度
	int16_t _currentCurrent;        // 当前电流（单位：mA）
	FeedbackCallback _callback;     // 反馈回调函数

	/**
	 * @brief 解析反馈数据
	 * @param rxData 接收到的数据指针
	 */
	void ParseFeedback(const uint8_t *rxData);
};
