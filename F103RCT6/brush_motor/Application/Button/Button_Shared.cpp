#include "Button_Shared.h"

const static PortPinPair keyPair{ KEY_GPIO_Port, KEY_Pin };

Button keyButton(keyPair, 800); // 按钮
