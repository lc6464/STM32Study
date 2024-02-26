#include <string.h>

#include "i2c.h"
#include "main.h"

#include "ist8310.h"

IST8310_t ist8310_data;

IST8310_Status_e VerifyMegId() {
  uint8_t id = 0;

  if (IST8310_ReadSingleData(IST8310_WMI_ADDR, id)) {
    return IST8310_I2C_Error;
  }

  if (id != IST8310_WMI_VALUE) {
    return IST8310_ID_Error;
  }

  return IST8310_OK;
}

IST8310_Status_e IST8310_Init(IST8310_t *ist8310_data) {
  memset(ist8310_data, 0, sizeof(IST8310_t));

  // 重启磁力计
  HAL_GPIO_WritePin(IST8310_Reset_GPIO_Port, IST8310_Reset_Pin, GPIO_PIN_RESET);
  HAL_Delay(35);
  HAL_GPIO_WritePin(IST8310_Reset_GPIO_Port, IST8310_Reset_Pin, GPIO_PIN_SET);
  HAL_Delay(65);

  // 验证 ID
  ist8310_data->Status = VerifyMegId();
  if (ist8310_data->Status != IST8310_OK) {
    return ist8310_data->Status;
  }

  // 设置磁力计连续工作模式，数据准备中断关闭
  uint8_t data[] = {IST8310_CNTL1_CONTINUE, IST8310_CNTL2_DRDY_DISABLE};
  if (IST8310_WriteMultiData(IST8310_CNTL1_ADDR, data, 2)) {
    ist8310_data->Status = IST8310_I2C_Error;
    return IST8310_I2C_Error;
  }

  // 设置磁力计采样次数和性能模式
  data[0] = IST8310_AVGCNTL_FOURTH_FOURTH;
  data[1] = IST8310_PDCNTL_PERFORMANCE;
  if (IST8310_WriteMultiData(IST8310_AVGCNTL_ADDR, data, 2)) {
    ist8310_data->Status = IST8310_I2C_Error;
    return IST8310_I2C_Error;
  }

  return IST8310_OK;
}

HAL_StatusTypeDef IST8310_ReadMegData(IST8310_t *ist8310_data) {
  IST8310_RawData *raw_data = &ist8310_data->Data;

  uint8_t buf[6];
  int16_t temp = 0;

  HAL_StatusTypeDef result =
      IST8310_ReadMultiData(IST8310_DATA_XL_ADDR, buf, 6);

  temp = (int16_t)((buf[1] << 8) | buf[0]);
  raw_data->X = MAG_RESO * temp;
  temp = (int16_t)((buf[3] << 8) | buf[2]);
  raw_data->Y = MAG_RESO * temp;
  temp = (int16_t)((buf[5] << 8) | buf[4]);
  raw_data->Z = MAG_RESO * temp;

  return result;
}