#pragma once

#include <cstdint>

namespace Power {
	namespace Battery {
		// 启用电池电压采样
		void enableVoltageSampling();

		// 禁用电池电压采样
		void disableVoltageSampling();

		// 计算电池电压
		float updateVoltage(uint16_t adc_value);

		// 读取上一次采样的电池电压
		float getVoltage();
	}

	namespace PowerSupply {
		// 计算电源电压
		float updateVoltage(uint16_t adc_value);

		// 读取上一次采样的电源电压
		float getVoltage();

		// 判断是否有外部电源输入
		bool isExternalPower();
	}
}