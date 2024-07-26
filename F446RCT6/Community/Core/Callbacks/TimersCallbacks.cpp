#include <cstring>

#include "RemoteControl_Shared.h"
#include "SSD1306_Shared.h"
#include "strings.h"
#include "tim.h"

inline char *SwitchToString(RemoteControl::SwitchPosition position, char *buffer) {
	switch (position) {
	case RemoteControl::SwitchPosition::Up:
		return strcpy(buffer, "U");
	case RemoteControl::SwitchPosition::Down:
		return strcpy(buffer, "D");
	case RemoteControl::SwitchPosition::Middle:
		return strcpy(buffer, "M");
	default:
		return strcpy(buffer, "?");
	}
};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) { // 100ms
		// RC Watchdog
		remoteControl.WatchDog();


		char buffer[8] = { 0 };

		ssd1306.Clear();

		// 展示遥控器信息
		ssd1306.SetCursor(25, 0);
		ssd1306.WriteString("RC Info", SSD1306Font_11x18);

		if (remoteControl.GetStatus() != RemoteControl::Status::Running) {
			ssd1306.SetCursor(25, 30);
			ssd1306.WriteString("Error ", SSD1306Font_11x18);
			uint8ToString(static_cast<uint8_t>(remoteControl.GetStatus()), buffer);
			ssd1306.WriteString(buffer, SSD1306Font_11x18);
		} else {
			ssd1306.SetCursor(0, 20);
			ssd1306.WriteString("S L:", SSD1306Font_7x10);
			ssd1306.SetCursor(0, 30);
			ssd1306.WriteString("S R:", SSD1306Font_7x10);
			ssd1306.SetCursor(0, 40);
			ssd1306.WriteString("3PS:", SSD1306Font_7x10);
			ssd1306.SetCursor(0, 50);
			ssd1306.WriteString("Dial:", SSD1306Font_7x10);

			// Stick L: X,Y
			ssd1306.SetCursor(40, 20);
			int16ToString(controllerData.LeftStickX, buffer, 0);
			ssd1306.WriteString(buffer, SSD1306Font_7x10);
			ssd1306.WriteChar(',', SSD1306Font_7x10);
			int16ToString(controllerData.LeftStickY, buffer, 0);
			ssd1306.WriteString(buffer, SSD1306Font_7x10);

			// Stick R: X,Y
			ssd1306.SetCursor(40, 30);
			int16ToString(controllerData.RightStickX, buffer, 0);
			ssd1306.WriteString(buffer, SSD1306Font_7x10);
			ssd1306.WriteChar(',', SSD1306Font_7x10);
			int16ToString(controllerData.RightStickY, buffer, 0);
			ssd1306.WriteString(buffer, SSD1306Font_7x10);

			// 3PS: L,R
			ssd1306.SetCursor(40, 40);
			SwitchToString(controllerData.LeftSwitch, buffer);
			ssd1306.WriteString(buffer, SSD1306Font_7x10);
			ssd1306.WriteChar(',', SSD1306Font_7x10);
			SwitchToString(controllerData.RightSwitch, buffer);
			ssd1306.WriteString(buffer, SSD1306Font_7x10);

			// Dial: D
			ssd1306.SetCursor(40, 50);
			int16ToString(controllerData.Dial, buffer, 0);
			ssd1306.WriteString(buffer, SSD1306Font_7x10);
		}

		ssd1306.UpdateScreen();
	}
}