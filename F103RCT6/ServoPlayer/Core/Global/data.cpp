#include "data.h"

std::array<uint16_t, 2> voltage_adc_buffer{}; // 电池电压和输入电压的 ADC 数据

std::array<float, 2> voltage_data{}; // 电压数据最近 50 次平均值

BMI088::RectangularCoordinate bmi088_data{}; // BMI088 传感器数据
BMI088::RectangularCoordinate bmi088_data_accumulator{}; // BMI088 传感器数据累加器
BMI088::RectangularCoordinate bmi088_data_average{}; // BMI088 传感器数据平均值