#include "BMI088.h"

#include <cstdint>

void BMI088::Read(RectangularCoordinate &accelerationData, RectangularCoordinate &gyroscopeData, float &temperature) {
	uint8_t buffer[6];

	// Read gyroscope
	Read(Scope::Gyroscope, gyroscopeData, buffer);

	// Read accelerometer
	Read(Scope::Accelerometer, accelerationData, buffer);

	// Read temperature
	ReadRegisters(Scope::Accelerometer, TEMPERATURE_HIGH, buffer, 2);
	int16_t temp_raw = (buffer[0] << 3) | (buffer[1] >> 5);
	if (temp_raw > 1023) {
		temp_raw -= 2048;
	}
	temperature = temp_raw * TEMPERATURE_FACTOR + TEMPERATURE_OFFSET;
}

// Private method implementations
void BMI088::Read(Scope sensor, RectangularCoordinate &data, uint8_t *buffer) {
	if (sensor == Scope::Accelerometer) {
		ReadRegisters(Scope::Accelerometer, ACCELEROMETER_AXIS_X_LOW, buffer, 6);
		data.x = static_cast<int16_t>((buffer[1] << 8) | buffer[0]) / 16.384f * 1.5f;
		data.y = static_cast<int16_t>((buffer[3] << 8) | buffer[2]) / 16.384f * 1.5f;
		data.z = static_cast<int16_t>((buffer[5] << 8) | buffer[4]) / 16.384f * 1.5f;
	} else {
		ReadRegisters(Scope::Gyroscope, GYROSCOPE_AXIS_X_LOW, buffer, 6);
		data.x = static_cast<int16_t>((buffer[1] << 8) | buffer[0]) / 32.768f;
		data.y = static_cast<int16_t>((buffer[3] << 8) | buffer[2]) / 32.768f;
		data.z = static_cast<int16_t>((buffer[5] << 8) | buffer[4]) / 32.768f;
	}
}

void BMI088::CsControl(Scope sensor, bool enable) {
	GPIO_PinState state = enable ? GPIO_PIN_RESET : GPIO_PIN_SET;
	PortPinPair &cs = (sensor == Scope::Accelerometer) ? _accelerometer_cs : _gyroscope_cs;
	HAL_GPIO_WritePin(cs.Port, cs.Pin, state);
}

uint8_t BMI088::TransmitReceive(uint8_t transmit_data) {
	uint8_t received_data = 0;
	HAL_SPI_TransmitReceive(&_hspi, &transmit_data, &received_data, 1, 20);
	return received_data;
}

void BMI088::WriteRegister(Scope sensor, uint8_t reg, uint8_t value) {
	CsControl(sensor, true);
	TransmitReceive(reg);
	TransmitReceive(value);
	CsControl(sensor, false);
}

uint8_t BMI088::ReadRegister(Scope sensor, uint8_t reg) {
	CsControl(sensor, true);
	TransmitReceive(reg | 0x80u);

	// 加速度计需要额外读取一个哑字节
	if (sensor == Scope::Accelerometer) {
		TransmitReceive(0xFFu); // 丢弃哑字节
	}

	uint8_t value = TransmitReceive(0xFFu);
	CsControl(sensor, false);
	return value;
}

void BMI088::ReadRegisters(Scope sensor, uint8_t reg, uint8_t *data, uint16_t len) {
	CsControl(sensor, true);
	TransmitReceive(reg | 0x80u);

	// 加速度计需要额外读取一个哑字节
	if (sensor == Scope::Accelerometer) {
		TransmitReceive(0xFFu); // 丢弃哑字节
	}

	for (uint16_t i = 0; i < len; i++) {
		data[i] = TransmitReceive(0xFFu);
	}
	CsControl(sensor, false);
}

BMI088::Status BMI088::InitAccelerometer() {
	const std::map<uint8_t, uint8_t> config = {
		{ACCELEROMETER_CONF, ACCELEROMETER_ODR_400HZ_OSR_4},
		{ACCELEROMETER_RANGE, ACCELEROMETER_RANGE_3G}
	};

	// 增加SPI模式切换
	CsControl(Scope::Accelerometer, true);
	HAL_Delay(1);
	CsControl(Scope::Accelerometer, false);
	HAL_Delay(1);

	// 软复位前先进入正常模式
	WriteRegister(Scope::Accelerometer, ACCELEROMETER_POWER_CONFIG, ACCELEROMETER_ACTIVE_MODE); // 退出挂起模式
	HAL_Delay(1);

	SoftReset(Scope::Accelerometer);
	HAL_Delay(LONG_DELAY_MS); // 手册要求至少 1ms，建议延长

	// 正确电源启动序列（手册3章）
	WriteRegister(Scope::Accelerometer, ACCELEROMETER_POWER_CONFIG, ACCELEROMETER_ACTIVE_MODE); // 退出挂起
	HAL_Delay(1);
	WriteRegister(Scope::Accelerometer, ACCELEROMETER_POWER_CONTROL, ACCELEROMETER_ENABLE); // 开启加速度计
	HAL_Delay(LONG_DELAY_MS); // 等待传感器稳定

	if (ReadRegister(Scope::Accelerometer, ACCELEROMETER_CHIP_ID) != ACCELEROMETER_CHIP_ID_VALUE) {
		return Status::NO_SENSOR;
	}

	for (const auto &[reg, val] : config) {
		WriteRegister(Scope::Accelerometer, reg, val);
		HAL_Delay(1);
	}

	return Status::OK;
}

BMI088::Status BMI088::InitGyroscope() {
	const std::map<uint8_t, uint8_t> config = {
		{GYROSCOPE_RANGE, GYROSCOPE_RANGE_1000DPS},
		{GYROSCOPE_BANDWIDTH, GYROSCOPE_ODR_400HZ_BANDWIDTH_47HZ}
	};

	// 预先读取两次寄存器，解决初始数据可能无效的问题
	ReadRegister(Scope::Gyroscope, GYROSCOPE_CHIP_ID);
	HAL_Delay(50);
	ReadRegister(Scope::Gyroscope, GYROSCOPE_CHIP_ID);
	HAL_Delay(50);

	SoftReset(Scope::Gyroscope);
	HAL_Delay(LONG_DELAY_MS);

	if (ReadRegister(Scope::Gyroscope, GYROSCOPE_CHIP_ID) != GYROSCOPE_CHIP_ID_VALUE) {
		return Status::NO_SENSOR;
	}

	for (const auto &[reg, val] : config) {
		WriteRegister(Scope::Gyroscope, reg, val);
		HAL_Delay(1);
	}

	return Status::OK;
}

void BMI088::SoftReset(Scope sensor) {
	auto address = (sensor == Scope::Accelerometer)
		? ACCELEROMETER_SOFTRESET : GYROSCOPE_SOFTRESET;

	WriteRegister(sensor, address, SOFTRESET_VALUE);
}