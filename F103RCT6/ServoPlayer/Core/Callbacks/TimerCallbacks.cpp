#include "tim.h"

#include "Button_Shared.h"
#include "data.h"
#include "Power.h"

void ReadBMI088();
void RefreshScreen();
void ServoControl();

// 除以 50
static inline uint16_t dividedBy50(uint32_t data) {
	return static_cast<uint16_t>(data / 50);
}

static std::array<uint32_t, 2> voltage_adc_accumulator{};

static uint8_t data_processor_scaler = 0;
static uint16_t tim6_scaler_50Hz = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		// 2.5kHz

		ReadBMI088();

		// 累加 50 次采样的值
		if (data_processor_scaler >= 50) {
			for (uint8_t i = 0; i < voltage_adc_buffer.size(); i++) {
				voltage_adc_accumulator[i] += voltage_adc_buffer[i];
			}
		}

		if (++data_processor_scaler == 100) {
			data_processor_scaler = 0;
			// 25Hz

			// 对后 50 次采样的电压 ADC 值取平均值
			Power::Battery::updateVoltage(dividedBy50(voltage_adc_accumulator[0]));
			Power::PowerSupply::updateVoltage(dividedBy50(voltage_adc_accumulator[1]));

			// 清空缓存
			voltage_adc_accumulator.fill(0);
		}
	} else if (htim->Instance == TIM6) {
		// 1kHz

		ec11Button.Tick();

		RefreshScreen();

		if (++tim6_scaler_50Hz >= 20) {
			tim6_scaler_50Hz = 0;
			// 10Hz

			ServoControl();
		}
	} //else if (htim->Instance == TIM7) {
		// 20kHz

	//}
}