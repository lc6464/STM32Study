#pragma once

#include "gpio.h"

#ifndef __cplusplus
#define __cplusplus
#endif

void led_flash(GPIO_TypeDef *port, uint16_t pin, uint32_t delay);


//void led_flash(GPIO_TypeDef *port, uint16_t pin, uint32_t delay);
//void led_flash(GPIO_TypeDef *port, uint16_t pin);