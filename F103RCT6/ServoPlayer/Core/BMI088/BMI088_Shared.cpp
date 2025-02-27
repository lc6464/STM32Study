#include "BMI088_Shared.h"

#include "spi.h"
#include "PortPinPair.h"

static const PortPinPair accelCSPair(CS_Accel_GPIO_Port, CS_Accel_Pin);
static const PortPinPair gyroCSPair(CS_GYRO_GPIO_Port, CS_GYRO_Pin);

BMI088 bmi088(hspi2, accelCSPair, gyroCSPair);