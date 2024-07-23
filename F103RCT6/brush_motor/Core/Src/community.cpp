#include "community.h"

void Community::Start() {
  CAN_FilterTypeDef filterConfig;

  filterConfig.FilterBank = 0;
  filterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  filterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  filterConfig.FilterIdHigh = 0;
  filterConfig.FilterIdLow = 0;
  filterConfig.FilterMaskIdHigh = 0;
  filterConfig.FilterMaskIdLow = 0;
  filterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  filterConfig.FilterActivation = ENABLE;
  filterConfig.SlaveStartFilterBank = 14;

  Start(&filterConfig);
}

void Community::Start(CAN_FilterTypeDef *filterConfig) {
  HAL_CAN_ConfigFilter(_hcan, filterConfig);
  HAL_CAN_Start(_hcan);
  HAL_CAN_ActivateNotification(_hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void Community::Stop() { HAL_CAN_Stop(_hcan); }

HAL_StatusTypeDef Community::Transmit(CAN_TxHeaderTypeDef *txHeader,
                                      uint8_t *txData) {
  uint32_t mailbox;
  return HAL_CAN_AddTxMessage(_hcan, txHeader, txData, &mailbox);
}

HAL_StatusTypeDef Community::SendMotorSpeed(uint16_t canId, int16_t speed,
                                            int16_t target, int16_t round,
                                            int16_t pidOut) {
  CAN_TxHeaderTypeDef txHeader;
  uint8_t txData[8];

  txHeader.StdId = canId;
  txHeader.IDE = CAN_ID_STD;
  txHeader.RTR = CAN_RTR_DATA;
  txHeader.DLC = 8;
  txHeader.TransmitGlobalTime = DISABLE;

  txData[0] = static_cast<uint8_t>(speed);
  txData[1] = static_cast<uint8_t>(speed >> 8);
  txData[2] = static_cast<uint8_t>(target);
  txData[3] = static_cast<uint8_t>(target >> 8);
  txData[4] = static_cast<uint8_t>(round);
  txData[5] = static_cast<uint8_t>(round >> 8);
  txData[6] = static_cast<uint8_t>(pidOut);
  txData[7] = static_cast<uint8_t>(pidOut >> 8);

  return Transmit(&txHeader, txData);
}

bool Community::ExecuteRxCallback(CAN_RxHeaderTypeDef *rxHeader,
                                  uint8_t *rxData) {
  if (_rxCallback != nullptr) {
    return _rxCallback(rxHeader, rxData);
  }

  return false;
}