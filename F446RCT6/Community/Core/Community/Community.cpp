#include "Community.h"

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

  Start(filterConfig);
}

void Community::Start(const CAN_FilterTypeDef &filterConfig) {
  HAL_CAN_ConfigFilter(_hcan, &filterConfig);
  HAL_CAN_Start(_hcan);
  HAL_CAN_ActivateNotification(_hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void Community::Stop() { HAL_CAN_Stop(_hcan); }

HAL_StatusTypeDef Community::Transmit(CAN_TxHeaderTypeDef *txHeader,
                                      uint8_t *txData, uint32_t *mailbox) {
  return HAL_CAN_AddTxMessage(_hcan, txHeader, txData, mailbox);
}

HAL_StatusTypeDef Community::SendSpeed(uint16_t canId, uint32_t *mailbox,
                                       float left, float right) {
  CAN_TxHeaderTypeDef txHeader = {.StdId = canId,
                                  .ExtId = 0,
                                  .IDE = CAN_ID_STD,
                                  .RTR = CAN_RTR_DATA,
                                  .DLC = 8,
                                  .TransmitGlobalTime = DISABLE};

  /*
          leftSpeed = *reinterpret_cast<float *>(rxData);
          rightSpeed = *reinterpret_cast<float *>(rxData + 4);
  */

  uint8_t txData[8] = {0};
  *reinterpret_cast<float *>(txData) = left;
  *reinterpret_cast<float *>(txData + 4) = right;

  return Transmit(&txHeader, txData, mailbox);
}

bool Community::ExecuteRxCallback(CAN_RxHeaderTypeDef *rxHeader,
                                  uint8_t *rxData) {
  if (_rxCallback != nullptr) {
    return _rxCallback(rxHeader, rxData);
  }

  return false;
}