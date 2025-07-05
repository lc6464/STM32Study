#include "Callbacks_Shared.h"
#include "Community_Shared.h"

Status systemStatus = Status::Error;

WatchDog systemWatchDog(300, []() { systemStatus = Status::Error; leftFrontTarget = 0; rightFrontTarget = 0; leftBackTarget = 0; rightBackTarget = 0; });