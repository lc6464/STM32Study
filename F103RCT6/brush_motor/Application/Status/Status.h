namespace Status {

	enum class SystemStatus {
		Running, // 正常运行
		Error,  // 错误状态（与上级控制器通信断开）
		Idle,   // 空闲状态（尚未连接上级控制器）
		Debug  // 调试状态
	};

	enum class DebugStatus {
		None,    // 无调试状态
		Preparing, // 准备调试
		LeftFront, // 左前轮调试
		RightFront, // 右前轮调试
		LeftBack, // 左后轮调试
		RightBack, // 右后轮调试
		Full, // 全部调试
		Finished // 调试完成
	};

	extern SystemStatus systemStatus; // 系统状态，默认为空闲状态
	extern DebugStatus debugStatus;   // 调试状态，默认为无调试状态

	// 电机实际速度
	extern float leftFrontSpeed;   // 左前轮速度
	extern float rightFrontSpeed;  // 右前轮速度
	extern float leftBackSpeed;    // 左后轮速度
	extern float rightBackSpeed;   // 右后轮速度

	// 电机目标速度
	extern float leftFrontTarget;  // 左前轮目标速度
	extern float rightFrontTarget; // 右前轮目标速度
	extern float leftBackTarget;   // 左后轮目标速度
	extern float rightBackTarget;  // 右后轮目标速度

};