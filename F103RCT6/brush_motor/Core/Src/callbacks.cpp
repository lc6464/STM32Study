#include "mailboxes.h"
#include "main-addition.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {
		// 100ms TIM6

		float speed0 = encoder0.Update();
		// float speed1 = encoder1.Update();
		// float speed2 = encoder2.Update();
		// float speed3 = encoder3.Update();

		float result0 = pid0.Update(static_cast<float>(/*round_speed + */target_speed), speed0);
		// float result1 = pid1.Update(static_cast<float>(round_speed + target_speed), speed1);
		// float result2 = pid2.Update(static_cast<float>(round_speed - target_speed), speed2);
		// float result3 = pid3.Update(static_cast<float>(-round_speed + target_speed), speed3);

		motor0.SetSpeed(static_cast<int16_t>(result0));
		// motor1.SetSpeed(static_cast<int16_t>(result1));
		// motor2.SetSpeed(static_cast<int16_t>(result2));
		// motor3.SetSpeed(static_cast<int16_t>(result3));

		// 发送电机速度
		auto mailbox = Mailboxes::Create();
		auto sent_result = community.SendMotorSpeed(0x1f0, mailbox, static_cast<int16_t>(speed0), target_speed, static_cast<int16_t>(result0));

		if (sent_result != HAL_OK) {
			// 发送失败，灯灭
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
			return;
		} else if (sent_times > 3) {
			// 无响应超过三次，灯常亮，低电平有效
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		} else {
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		}

		sent_times++;

		return;
	}            

	// 编码器定时器溢出更新
	if (encoder0.OverflowCallback(htim)) {
		return;
	}

	// if (encoder1.OverflowCallback(htim)) {
	// 	return;
	// }

	// if (encoder2.OverflowCallback(htim)) {
	// 	return;
	// }

	// if (encoder3.OverflowCallback(htim)) {
	// 	return;
	// }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	CAN_RxHeaderTypeDef rx_header;
	uint8_t rx_data[8];

	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data); // 接收数据

	if (community.ExecuteRxCallback(&rx_header, rx_data)) {
		return;
	}
}