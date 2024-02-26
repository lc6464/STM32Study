/**
 ******************************************************************************
 * @file    BMI088driver.c
 * @author
 * @version V1.2.0
 * @date    2022/3/8
 * @brief
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 */
#include "BMI088driver.h"
#include "BMI088reg.h"

float BMI088_ACCEL_SEN = BMI088_ACCEL_3G_SEN;
float BMI088_GYRO_SEN = BMI088_GYRO_2000_SEN;

static uint8_t res = 0;
static uint8_t write_reg_num = 0;
static uint8_t error = BMI088_NO_ERROR;
float gyroDiff[3], gNormDiff;

uint8_t caliOffset = 1;
int16_t caliCount = 0;

uint8_t BMI088_read_write_byte(SPI_Handle_t *spi_handle, uint8_t txdata) {
  uint8_t rx_data;
  SEML_SPI_TransmitReceive(spi_handle, &txdata, &rx_data, 1, 1000);
  return rx_data;
}
#define BMI088_USE_SPI
#if defined(BMI088_USE_SPI)

#define BMI088_ACCEL_NS_L(handle) SEML_GPIO_Pin_Reset(handle)
#define BMI088_ACCEL_NS_H(handle) SEML_GPIO_Pin_Set(handle)

#define BMI088_GYRO_NS_L(handle) SEML_GPIO_Pin_Reset(handle)
#define BMI088_GYRO_NS_H(handle) SEML_GPIO_Pin_Set(handle)

#define BMI088_accel_write_single_reg(handle, reg, data)                       \
  do {                                                                         \
    BMI088_ACCEL_NS_L(&(handle)->CS1_Accel_handle);                            \
    BMI088_write_single_reg(&(handle)->SPI_Handle, (reg), (data));             \
    BMI088_ACCEL_NS_H(&(handle)->CS1_Accel_handle);                            \
  } while (0)
#define BMI088_accel_read_single_reg(handle, reg, data)                        \
  do {                                                                         \
    BMI088_ACCEL_NS_L(&(handle)->CS1_Accel_handle);                            \
    BMI088_read_write_byte(&(handle)->SPI_Handle, (reg) | 0x80);               \
    BMI088_read_write_byte(&(handle)->SPI_Handle, 0x55);                       \
    (data) = BMI088_read_write_byte(&(handle)->SPI_Handle, 0x55);              \
    BMI088_ACCEL_NS_H(&(handle)->CS1_Accel_handle);                            \
  } while (0)
#define BMI088_accel_read_muli_reg(handle, reg, data, len)                     \
  do {                                                                         \
    BMI088_ACCEL_NS_L(&(handle)->CS1_Accel_handle);                            \
    BMI088_read_write_byte(&(handle)->SPI_Handle, (reg) | 0x80);               \
    BMI088_read_muli_reg(&(handle)->SPI_Handle, reg, data, len);               \
    BMI088_ACCEL_NS_H(&(handle)->CS1_Accel_handle);                            \
  } while (0)

#define BMI088_gyro_write_single_reg(handle, reg, data)                        \
  do {                                                                         \
    BMI088_GYRO_NS_L(&(handle)->CS1_Gyro_handle);                              \
    BMI088_write_single_reg(&(handle)->SPI_Handle, (reg), (data));             \
    BMI088_GYRO_NS_H(&(handle)->CS1_Gyro_handle);                              \
  } while (0)
#define BMI088_gyro_read_single_reg(handle, reg, data)                         \
  do {                                                                         \
    BMI088_GYRO_NS_L(&(handle)->CS1_Gyro_handle);                              \
    BMI088_read_single_reg(&(handle)->SPI_Handle, (reg), &(data));             \
    BMI088_GYRO_NS_H(&(handle)->CS1_Gyro_handle);                              \
  } while (0)
#define BMI088_gyro_read_muli_reg(handle, reg, data, len)                      \
  do {                                                                         \
    BMI088_GYRO_NS_L(&(handle)->CS1_Gyro_handle);                              \
    BMI088_read_muli_reg(&(handle)->SPI_Handle, (reg), (data), (len));         \
    BMI088_GYRO_NS_H(&(handle)->CS1_Gyro_handle);                              \
  } while (0)

static void BMI088_write_single_reg(SPI_Handle_t *handle, uint8_t reg,
                                    uint8_t data) {
  BMI088_read_write_byte(handle, reg);
  BMI088_read_write_byte(handle, data);
}

static void BMI088_read_single_reg(SPI_Handle_t *handle, uint8_t reg,
                                   uint8_t *return_data) {
  BMI088_read_write_byte(handle, reg | 0x80);
  *return_data = BMI088_read_write_byte(handle, 0x55);
}

static void BMI088_read_muli_reg(SPI_Handle_t *handle, uint8_t reg,
                                 uint8_t *buf, uint8_t len) {
  BMI088_read_write_byte(handle, reg | 0x80);

  while (len != 0) {

    *buf = BMI088_read_write_byte(handle, 0x55);
    buf++;
    len--;
  }
}
#elif defined(BMI088_USE_IIC)

#endif

static uint8_t
    write_BMI088_accel_reg_data_error[BMI088_WRITE_ACCEL_REG_NUM][3] = {
        {BMI088_ACC_PWR_CTRL, BMI088_ACC_ENABLE_ACC_ON,
         BMI088_ACC_PWR_CTRL_ERROR},
        {BMI088_ACC_PWR_CONF, BMI088_ACC_PWR_ACTIVE_MODE,
         BMI088_ACC_PWR_CONF_ERROR},
        {BMI088_ACC_CONF,
         BMI088_ACC_NORMAL | BMI088_ACC_800_HZ | BMI088_ACC_CONF_MUST_Set,
         BMI088_ACC_CONF_ERROR},
        {BMI088_ACC_RANGE, BMI088_ACC_RANGE_3G, BMI088_ACC_RANGE_ERROR},
        {BMI088_INT1_IO_CTRL,
         BMI088_ACC_INT1_IO_ENABLE | BMI088_ACC_INT1_GPIO_PP |
             BMI088_ACC_INT1_GPIO_LOW,
         BMI088_INT1_IO_CTRL_ERROR},
        {BMI088_INT_MAP_DATA, BMI088_ACC_INT1_DRDY_INTERRUPT,
         BMI088_INT_MAP_DATA_ERROR}

};

static uint8_t write_BMI088_gyro_reg_data_error[BMI088_WRITE_GYRO_REG_NUM][3] =
    {{BMI088_GYRO_RANGE, BMI088_GYRO_2000, BMI088_GYRO_RANGE_ERROR},
     {BMI088_GYRO_BANDWIDTH,
      BMI088_GYRO_1000_116_HZ | BMI088_GYRO_BANDWIDTH_MUST_Set,
      BMI088_GYRO_BANDWIDTH_ERROR},
     {BMI088_GYRO_LPM1, BMI088_GYRO_NORMAL_MODE, BMI088_GYRO_LPM1_ERROR},
     {BMI088_GYRO_CTRL, BMI088_DRDY_ON, BMI088_GYRO_CTRL_ERROR},
     {BMI088_GYRO_INT3_INT4_IO_CONF,
      BMI088_GYRO_INT3_GPIO_PP | BMI088_GYRO_INT3_GPIO_LOW,
      BMI088_GYRO_INT3_INT4_IO_CONF_ERROR},
     {BMI088_GYRO_INT3_INT4_IO_MAP, BMI088_GYRO_DRDY_IO_INT3,
      BMI088_GYRO_INT3_INT4_IO_MAP_ERROR}

};

uint8_t BMI088_Accel_Init(BMI088_Data_t *bmi088) {
  // check commiunication
  BMI088_accel_read_single_reg(bmi088, BMI088_ACC_CHIP_ID, res);
  BMI088_Delay(1);
  BMI088_accel_read_single_reg(bmi088, BMI088_ACC_CHIP_ID, res);
  BMI088_Delay(1);

  // accel software reset
  BMI088_accel_write_single_reg(bmi088, BMI088_ACC_SOFTRESET,
                                BMI088_ACC_SOFTRESET_VALUE);
  BMI088_Delay(BMI088_LONG_DELAY_TIME);

  // check commiunication is normal after reset
  BMI088_accel_read_single_reg(bmi088, BMI088_ACC_CHIP_ID, res);
  BMI088_Delay(1);
  BMI088_accel_read_single_reg(bmi088, BMI088_ACC_CHIP_ID, res);
  BMI088_Delay(1);

  // check the "who am I"
  if (res != BMI088_ACC_CHIP_ID_VALUE)
    return BMI088_NO_SENSOR;

  // set accel sonsor config and check
  for (write_reg_num = 0; write_reg_num < BMI088_WRITE_ACCEL_REG_NUM;
       write_reg_num++) {

    BMI088_accel_write_single_reg(
        bmi088, write_BMI088_accel_reg_data_error[write_reg_num][0],
        write_BMI088_accel_reg_data_error[write_reg_num][1]);
    BMI088_Delay(1);

    BMI088_accel_read_single_reg(
        bmi088, write_BMI088_accel_reg_data_error[write_reg_num][0], res);
    BMI088_Delay(1);

    if (res != write_BMI088_accel_reg_data_error[write_reg_num][1]) {
      // write_reg_num--;
      // return write_BMI088_accel_reg_data_error[write_reg_num][2];
      error |= write_BMI088_accel_reg_data_error[write_reg_num][2];
    }
  }
  return BMI088_NO_ERROR;
}

uint8_t BMI088_Gyro_Init(BMI088_Data_t *bmi088) {
  // check commiunication
  BMI088_gyro_read_single_reg(bmi088, BMI088_GYRO_CHIP_ID, res);
  BMI088_Delay(1);
  BMI088_gyro_read_single_reg(bmi088, BMI088_GYRO_CHIP_ID, res);
  BMI088_Delay(1);

  // reset the gyro sensor
  BMI088_gyro_write_single_reg(bmi088, BMI088_GYRO_SOFTRESET,
                               BMI088_GYRO_SOFTRESET_VALUE);
  BMI088_Delay(BMI088_LONG_DELAY_TIME);
  // check commiunication is normal after reset
  BMI088_gyro_read_single_reg(bmi088, BMI088_GYRO_CHIP_ID, res);
  BMI088_Delay(1);
  BMI088_gyro_read_single_reg(bmi088, BMI088_GYRO_CHIP_ID, res);
  BMI088_Delay(1);

  // check the "who am I"
  if (res != BMI088_GYRO_CHIP_ID_VALUE)
    return BMI088_NO_SENSOR;

  // set gyro sonsor config and check
  for (write_reg_num = 0; write_reg_num < BMI088_WRITE_GYRO_REG_NUM;
       write_reg_num++) {

    BMI088_gyro_write_single_reg(
        bmi088, write_BMI088_gyro_reg_data_error[write_reg_num][0],
        write_BMI088_gyro_reg_data_error[write_reg_num][1]);
    BMI088_Delay(1);

    BMI088_gyro_read_single_reg(
        bmi088, write_BMI088_gyro_reg_data_error[write_reg_num][0], res);
    BMI088_Delay(1);

    if (res != write_BMI088_gyro_reg_data_error[write_reg_num][1]) {
      write_reg_num--;
      // return write_BMI088_gyro_reg_data_error[write_reg_num][2];
      error |= write_BMI088_accel_reg_data_error[write_reg_num][2];
    }
  }

  return BMI088_NO_ERROR;
}

uint8_t BMI088_Init(BMI088_Data_t *bmi088) {

  uint8_t error = BMI088_NO_ERROR;
  error |= BMI088_Accel_Init(bmi088);
  error |= BMI088_Gyro_Init(bmi088);
  return error;
}

void BMI088_Read(BMI088_Data_t *bmi088, Accel_Data_t *accel_data,
                 Gyro_Data_t *gyro_data) {
  static uint8_t buf[8] = {0, 0, 0, 0, 0, 0};
  static int16_t bmi088_raw_temp;

  BMI088_accel_read_muli_reg(bmi088, BMI088_ACCEL_XOUT_L, buf, 6);

  bmi088_raw_temp = (int16_t)((buf[1]) << 8) | buf[0];
  accel_data->x = bmi088_raw_temp * BMI088_ACCEL_SEN;
  bmi088_raw_temp = (int16_t)((buf[3]) << 8) | buf[2];
  accel_data->y = bmi088_raw_temp * BMI088_ACCEL_SEN;
  bmi088_raw_temp = (int16_t)((buf[5]) << 8) | buf[4];
  accel_data->z = bmi088_raw_temp * BMI088_ACCEL_SEN;

  BMI088_gyro_read_muli_reg(bmi088, BMI088_GYRO_CHIP_ID, buf, 8);
  if (buf[0] == BMI088_GYRO_CHIP_ID_VALUE) {
    bmi088_raw_temp = (int16_t)((buf[3]) << 8) | buf[2];
    gyro_data->x = bmi088_raw_temp * BMI088_GYRO_SEN;
    bmi088_raw_temp = (int16_t)((buf[5]) << 8) | buf[4];
    gyro_data->y = bmi088_raw_temp * BMI088_GYRO_SEN;
    bmi088_raw_temp = (int16_t)((buf[7]) << 8) | buf[6];
    gyro_data->z = bmi088_raw_temp * BMI088_GYRO_SEN;
  }
  BMI088_accel_read_muli_reg(bmi088, BMI088_TEMP_M, buf, 2);

  bmi088_raw_temp = (int16_t)((buf[0] << 3) | (buf[1] >> 5));

  if (bmi088_raw_temp > 1023) {
    bmi088_raw_temp -= 2048;
  }

  bmi088->Temperature =
      bmi088_raw_temp * BMI088_TEMP_FACTOR + BMI088_TEMP_OFFSET;
}
