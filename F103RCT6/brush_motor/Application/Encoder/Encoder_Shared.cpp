#include "Encoder_Shared.h"

// 四个编码器
Encoder leftFrontEncoder(&htim5);
Encoder rightFrontEncoder(&htim3);
Encoder leftBackEncoder(&htim2);
Encoder rightBackEncoder(&htim4);