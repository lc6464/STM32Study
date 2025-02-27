#include "BMI088_Shared.h"
#include "data.h"

static uint8_t read_scaler = 0;

void ReadBMI088() {
	if (++read_scaler < 50) {
		return;
	}

	// 2.5kHz -> 50Hz
	read_scaler = 0;

	bmi088.Read(BMI088::Scope::Accelerometer, bmi088_data);

	bmi088_data_accumulator.x += bmi088_data.x;
	bmi088_data_accumulator.y += bmi088_data.y;
	bmi088_data_accumulator.z += bmi088_data.z;
}