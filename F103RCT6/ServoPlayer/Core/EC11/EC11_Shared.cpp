#include "EC11_Shared.h"

static const PortPinPair encoderPair(Encoder_SW_GPIO_Port, Encoder_SW_Pin);
static Button encoderButton(encoderPair);

EC11 ec11(htim3, encoderButton);
