#include "Community_Shared.h"
#include "can.h"

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  CAN_RxHeaderTypeDef rx_header;
  uint8_t rx_data[8];

  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data); // 接收数据

  if (community1.ExecuteRxCallback(&rx_header, rx_data)) {
    // 对 0x200 发送 0x00
    CAN_TxHeaderTypeDef tx_header;
    uint8_t tx_data[1] = {0};

    tx_header.StdId = 0x200;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 1;
    tx_header.TransmitGlobalTime = DISABLE;

    uint32_t mailbox;

    community1.Transmit(&tx_header, tx_data, &mailbox);

    return;
  }
}