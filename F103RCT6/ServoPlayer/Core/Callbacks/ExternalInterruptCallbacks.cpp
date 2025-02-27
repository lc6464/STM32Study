#include "Button_Shared.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (ec11Button.HandleInterrupt(GPIO_Pin)) {
		return;
	}
}