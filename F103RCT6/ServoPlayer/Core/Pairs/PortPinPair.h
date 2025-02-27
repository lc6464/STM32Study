#pragma once

#include "gpio.h"

// Port - Pin Pair
class PortPinPair {
public:
    GPIO_TypeDef *Port;
    uint16_t Pin;

    explicit PortPinPair(GPIO_TypeDef *port, uint16_t pin) : Port(port), Pin(pin) {
        assert_param(IS_GPIO_ALL_INSTANCE(port));
    }
};