#pragma once

#include <map>

#include "spi.h"
#include "PortPinPair.h"

// 基于 SPI 通信的 BMI088 传感器驱动
class BMI088 {
public:
	// 函数返回状态
	enum class Status {
		OK,
		ERROR,
		TIMEOUT,
		NO_SENSOR
	};

	// 传感器范围
	enum class Scope {
		Accelerometer,
		Gyroscope,
	};

	// 三轴直角坐标
	struct RectangularCoordinate {
		float x;
		float y;
		float z;
	};

	/**
	 * @brief 构造函数
	 * @param hspi SPI 句柄
	 * @param accel_cs 加速度计片选引脚
	 * @param gyro_cs 陀螺仪片选引脚
	 */
	explicit BMI088(SPI_HandleTypeDef &hspi, const PortPinPair &accel_cs, const PortPinPair &gyro_cs)
		: _hspi(hspi), _accelerometer_cs(accel_cs), _gyroscope_cs(gyro_cs) { }

	/**
	 * @brief 启动传感器
	 * @return 初始化结果
	 * @note 返回值为两个 Status 类型的元组，分别表示加速度计和陀螺仪的初始化结果
	 */
	std::tuple<Status, Status> Start() {
		Status status = InitAccelerometer();
		if (status == Status::NO_SENSOR) {
			return { Status::NO_SENSOR, Status::NO_SENSOR };
		}
		// Status status = Status::OK;

		Status gyro_status = InitGyroscope();
		return { status, gyro_status };
	}

	/**
	 * @brief 读取传感器数据
	 * @param sensor 传感器范围
	 * @param data 三轴直角坐标
	 */
	void Read(Scope sensor, RectangularCoordinate &data) {
		uint8_t buffer[6];
		Read(sensor, data, buffer);
	}

	/**
	 * @brief 读取传感器数据
	 * @param accelerometerData 加速度计数据
	 * @param gyroscopeData 陀螺仪数据
	 * @param temperature 温度
	 */
	void Read(RectangularCoordinate &accelerometerData, RectangularCoordinate &gyroscopeData, float &temperature);

private:
	// Static constants
	static constexpr uint8_t LONG_DELAY_MS = 80;

	static constexpr float TEMPERATURE_FACTOR = 0.125f;
	static constexpr float TEMPERATURE_OFFSET = 23.0f;

	static constexpr uint8_t ACCELEROMETER_CHIP_ID_VALUE = 0x1E;
	static constexpr uint8_t GYROSCOPE_CHIP_ID_VALUE = 0x0F;

	static constexpr uint8_t SOFTRESET_VALUE = 0xB6;
	static constexpr uint8_t ACCELEROMETER_ENABLE = 0x04;
	static constexpr uint8_t ACCELEROMETER_ACTIVE_MODE = 0x00;

	static constexpr uint8_t ACCELEROMETER_ODR_400HZ_OSR_4 = (0x08 << 4) | 0x0A;
	static constexpr uint8_t GYROSCOPE_ODR_400HZ_BANDWIDTH_47HZ = 0x03;
	static constexpr uint8_t ACCELEROMETER_RANGE_3G = 0x00;
	static constexpr uint8_t GYROSCOPE_RANGE_1000DPS = 0x01;

	// Register map
	static constexpr uint8_t ACCELEROMETER_CHIP_ID = 0x00;
	static constexpr uint8_t GYROSCOPE_CHIP_ID = 0x00;

	static constexpr uint8_t ACCELEROMETER_SOFTRESET = 0x7E;
	static constexpr uint8_t GYROSCOPE_SOFTRESET = 0x14;
	static constexpr uint8_t ACCELEROMETER_POWER_CONFIG = 0x7C; // power config: active mode
	static constexpr uint8_t ACCELEROMETER_POWER_CONTROL = 0x7D; // power control: enable accelerometer

	static constexpr uint8_t ACCELEROMETER_CONF = 0x40; // ODR: 400Hz, OSR: 4
	static constexpr uint8_t ACCELEROMETER_RANGE = 0x41; // range: 3G
	static constexpr uint8_t GYROSCOPE_RANGE = 0x0F; // range: 1000dps
	static constexpr uint8_t GYROSCOPE_BANDWIDTH = 0x10; // bandwidth: 47Hz

	static constexpr uint8_t ACCELEROMETER_AXIS_X_LOW = 0x12;
	static constexpr uint8_t GYROSCOPE_AXIS_X_LOW = 0x02;
	static constexpr uint8_t TEMPERATURE_HIGH = 0x22;

	// Peripheral handles
	SPI_HandleTypeDef &_hspi;
	PortPinPair _accelerometer_cs;
	PortPinPair _gyroscope_cs;

	// Private methods
	void CsControl(Scope sensor, bool enable);
	uint8_t TransmitReceive(uint8_t data);
	void WriteRegister(Scope sensor, uint8_t reg, uint8_t value);
	uint8_t ReadRegister(Scope sensor, uint8_t reg);
	void ReadRegisters(Scope sensor, uint8_t reg, uint8_t *data, uint16_t len);

	void Read(Scope sensor, RectangularCoordinate &data, uint8_t *buffer);

	Status InitAccelerometer();
	Status InitGyroscope();

	void SoftReset(Scope sensor);
};