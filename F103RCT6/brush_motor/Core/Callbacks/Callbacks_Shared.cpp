#include "Callbacks_Shared.h"
#include "Community_Shared.h"

Status systemStatus = Status::Error;

WatchDog systemWatchDog(300, []() {
  systemStatus = Status::Error;
  leftTarget = 0;
  rightTarget = 0;
});