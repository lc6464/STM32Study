#include "Power.h"

#include "adc.h"
#include "data.h"

namespace Power {
	// 启用电池电压采样
	void Battery::enableVoltageSampling() {
		HAL_GPIO_WritePin(VBat_Sample_Enable_GPIO_Port, VBat_Sample_Enable_Pin, GPIO_PIN_SET);
	}

	// 禁用电池电压采样
	void Battery::disableVoltageSampling() {
		HAL_GPIO_WritePin(VBat_Sample_Enable_GPIO_Port, VBat_Sample_Enable_Pin, GPIO_PIN_RESET);
	}

	// 计算电池电压
	float Battery::updateVoltage(uint16_t adc_value) {
		// ADC 采样值转换为电压值
		float voltage = adc_value * 3.3f / 4096;

		// 上 20k 下 47k 电压分压，做一点点补偿
		auto result = voltage * (20 + 47 + 0.18) / 47;

		voltage_data[0] = result;
		return result;
	}

	// 读取上一次采样的电池电压
	float Battery::getVoltage() {
		return voltage_data[0];
	}


	// 计算电源电压
	float PowerSupply::updateVoltage(uint16_t adc_value) {
		// ADC 采样值转换为电压值
		float voltage = adc_value * 3.3f / 4096;

		// 上 20k 下 20k 电压分压
		auto result = voltage * (20 + 20) / 20;

		voltage_data[1] = result;
		return result;
	}

	// 读取上一次采样的电源电压
	float PowerSupply::getVoltage() {
		return voltage_data[1];
	}

	// 判断是否有外部电源输入
	bool PowerSupply::isExternalPower() {
		// 电源电压采样值
		float voltage = getVoltage();

		// 5V USB 电源
		return voltage > 4.35f;
	}
}