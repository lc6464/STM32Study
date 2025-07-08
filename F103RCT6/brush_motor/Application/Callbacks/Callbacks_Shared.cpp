#include "Callbacks_Shared.h"
#include "Community_Shared.h"
#include "Status.h"

WatchDog systemWatchDog(300, []() {
	if (Status::systemStatus == Status::SystemStatus::Running) {
		Status::systemStatus = Status::SystemStatus::Error;
		Status::leftFrontTarget = 0;
		Status::rightFrontTarget = 0;
		Status::leftBackTarget = 0;
		Status::rightBackTarget = 0;
	}

	});