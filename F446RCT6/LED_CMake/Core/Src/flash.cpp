#include "gpio.h"

extern "C" {
void led_flash(GPIO_TypeDef *port, uint16_t pin, uint32_t delay) {
  HAL_GPIO_TogglePin(port, pin);
  HAL_Delay(delay);
  HAL_GPIO_TogglePin(port, pin);
}

// void led_flash(GPIO_TypeDef *port, uint16_t pin) {
// 	HAL_GPIO_TogglePin(port, pin);
// 	HAL_Delay(500);
// 	HAL_GPIO_TogglePin(port, pin);
// }
}