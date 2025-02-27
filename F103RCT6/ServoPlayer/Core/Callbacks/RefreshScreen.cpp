#include "data.h"
#include "Power.h"
#include "SSD1306_Shared.h"
#include "Status.h"
#include "strings.h"

static uint8_t screen_scaler = 0;
static bool screen_switch = false;

static std::array<char, 16> string_buffer{};

// 1kHz
void RefreshScreen() {
	if (++screen_scaler < 40) {
		return;
	}

	screen_scaler = 0;

	// 25Hz
	if (screen_switch) {
		// 12.5Hz

		ssd1306_0.Clear();

		// 电源状态
		ssd1306_0.SetCursor(0, 0);
		ssd1306_0.WriteString(Power::PowerSupply::isExternalPower() ? "Charging     " : "Discharging  ", SSD1306Fonts::Font_7x10);

		uint8_t string_index = 0;
		string_index += floatToString(Power::Battery::getVoltage(), string_buffer.data() + string_index, 2);
		string_buffer[string_index++] = 'V';
		string_buffer[string_index++] = 0;

		ssd1306_0.WriteString(string_buffer.data(), SSD1306Fonts::Font_7x10);

		// BMI088 加速度
		ssd1306_0.SetCursor(0, 15);
		floatToString(bmi088_data.x, string_buffer.data(), 5);
		ssd1306_0.WriteString(string_buffer.data(), SSD1306Fonts::Font_7x10);

		ssd1306_0.SetCursor(0, 30);
		floatToString(bmi088_data.y, string_buffer.data(), 5);
		ssd1306_0.WriteString(string_buffer.data(), SSD1306Fonts::Font_7x10);

		ssd1306_0.SetCursor(0, 45);
		floatToString(bmi088_data.z, string_buffer.data(), 5);
		ssd1306_0.WriteString(string_buffer.data(), SSD1306Fonts::Font_7x10);

		ssd1306_0.UpdateScreen();
	} else {
		// 12.5Hz

		ssd1306_1.Clear();

		// ec11 旋转值
		ssd1306_1.SetCursor(0, 0);
		ssd1306_1.WriteString("EC11: ", SSD1306Fonts::Font_7x10);
		auto string_index = int16ToString(Status::ec11RotationAccumulator, string_buffer.data());
		string_buffer[string_index++] = ' ';
		string_buffer[string_index++] = ' ';
		string_buffer[string_index++] = ' ';
		string_buffer[string_index++] = ' ';
		string_buffer[string_index++] = ' ';
		string_buffer[string_index++] = 0;
		ssd1306_1.WriteString(string_buffer.data(), SSD1306Fonts::Font_7x10);

		// 舵机角度
		ssd1306_1.SetCursor(0, 15);
		int16ToString(Status::servoAngle, string_buffer.data());
		ssd1306_1.WriteString(string_buffer.data(), SSD1306Fonts::Font_7x10);

		// BMI088 角度
		ssd1306_1.SetCursor(0, 30);
		floatToString(Status::bmi088_angle, string_buffer.data(), 5);
		ssd1306_1.WriteString(string_buffer.data(), SSD1306Fonts::Font_7x10);

		// 目标角度
		ssd1306_1.SetCursor(0, 45);
		int16ToString(Status::targetAngle, string_buffer.data());
		ssd1306_1.WriteString(string_buffer.data(), SSD1306Fonts::Font_7x10);

		ssd1306_1.UpdateScreen();
	}
	screen_switch = !screen_switch;
}