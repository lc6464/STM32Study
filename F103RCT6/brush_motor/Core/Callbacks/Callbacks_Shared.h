#pragma once

#include "WatchDog.h"

enum class Status { OK, Error };

extern Status systemStatus;

extern WatchDog systemWatchDog;