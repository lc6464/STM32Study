#include "i2c_if.h"

/**
 * @brief i2c注册函数
 * @param I2C_Handle i2c句柄
 * @param hi2c i2c接口句柄
 * @param I2C_Transmit i2c发送函数
 * @param I2C_Receive i2c接收函数
 * @param I2C_Mem_Write i2c写寄存器函数
 * @param I2C_Mem_Read i2c读寄存器函数
 */
void SEML_I2C_Register(I2C_Handle_t *I2C_Handle, void *hi2c,
                       I2C_Transmit_t I2C_Transmit, I2C_Receive_t I2C_Receive,
                       I2C_Mem_Write_t I2C_Mem_Write,
                       I2C_Mem_Read_t I2C_Mem_Read) {
  assert_param(I2C_Handle != NULL);
  assert_param(hi2c != NULL);
  assert_param(I2C_Transmit != NULL);
  assert_param(I2C_Receive != NULL);
  assert_param(I2C_Mem_Read != NULL);
  assert_param(I2C_Mem_Write != NULL);

  I2C_Handle->hi2c = hi2c;
  I2C_Handle->I2C_Receive = I2C_Receive;
  I2C_Handle->I2C_Transmit = I2C_Transmit;
  I2C_Handle->I2C_Mem_Read = I2C_Mem_Read;
  I2C_Handle->I2C_Mem_Write = I2C_Mem_Write;
}

/**
 * @brief i2c发送函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[in] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_I2C_Transmit(I2C_Handle_t *I2C_Handle,
                                     uint16_t DevAddress, uint8_t *pData,
                                     uint16_t Size, uint32_t Timeout) {
  assert_param(I2C_Handle != NULL);
  assert_param(pData != NULL);

  return I2C_Handle->I2C_Transmit(I2C_Handle->hi2c, DevAddress, pData, Size,
                                  Timeout);
}
/**
 * @brief i2c接收函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[out] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_I2C_Receive(I2C_Handle_t *I2C_Handle,
                                    uint16_t DevAddress, uint8_t *pData,
                                    uint16_t Size, uint32_t Timeout) {
  assert_param(I2C_Handle != NULL);
  assert_param(pData != NULL);

  return I2C_Handle->I2C_Receive(I2C_Handle->hi2c, DevAddress, pData, Size,
                                 Timeout);
}

/**
 * @brief i2c读寄存器函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[in] MemAddress 内存地址
 * @param[in] MemAddSize 内存地址大小
 *  - I2C_MEMADD_SIZE_8BIT 内存地址为1字节
 *  - I2C_MEMADD_SIZE_16BIT 内存地址为2字节
 * @param[out] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_I2C_Mem_Read(I2C_Handle_t *I2C_Handle,
                                     uint16_t DevAddress, uint16_t MemAddress,
                                     uint16_t MemAddSize, uint8_t *pData,
                                     uint16_t Size, uint32_t Timeout) {
  assert_param(I2C_Handle != NULL);
  assert_param(pData != NULL);

  return I2C_Handle->I2C_Mem_Read(I2C_Handle->hi2c, DevAddress, MemAddress,
                                  MemAddSize, pData, Size, Timeout);
}

/**
 * @brief i2c写寄存器函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[in] MemAddress 内存地址
 * @param[in] MemAddSize 内存地址大小
 *  - I2C_MEMADD_SIZE_8BIT 内存地址为1字节
 *  - I2C_MEMADD_SIZE_16BIT 内存地址为2字节
 * @param[in] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_I2C_Mem_Write(I2C_Handle_t *I2C_Handle,
                                      uint16_t DevAddress, uint16_t MemAddress,
                                      uint16_t MemAddSize, uint8_t *pData,
                                      uint16_t Size, uint32_t Timeout) {
  assert_param(I2C_Handle != NULL);
  assert_param(pData != NULL);

  return I2C_Handle->I2C_Mem_Write(I2C_Handle->hi2c, DevAddress, MemAddress,
                                   MemAddSize, pData, Size, Timeout);
}

void i2c_delay_us(uint32_t delay_us) {
  const uint32_t a = SystemCoreClock / 10000000;
  uint32_t temp = a * delay_us;
  while (temp--)
    ;
}
#ifdef USE_I2C_DELAY
#ifndef i2c_delay
#define i2c_delay() i2c_delay_us(1)
#endif
#else
#define i2c_delay()
#endif

/**
 * @brief I2C发送起始信号
 * @param[in] hi2c 软件i2c接口句柄
 * @return 函数执行状态
 * @retval SEML_BUSY 总线被占用
 * @retval SEML_OK 执行完成
 */
static SEML_StatusTypeDef Software_I2C_Start(Software_I2C_HandleTypeDef *hi2c) {
  /* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
  // 拉高电平
  SEML_GPIO_Pin_Set(&hi2c->SDA);
  i2c_delay();
  SEML_GPIO_Pin_Set(&hi2c->SCL);
  i2c_delay();
  // 总线被占用
  if (SEML_GPIO_Pin_Input(&hi2c->SDA) != SET)
    return SEML_BUSY;
  SEML_GPIO_Pin_Reset(&hi2c->SDA);
  i2c_delay();
  SEML_GPIO_Pin_Reset(&hi2c->SCL);
  i2c_delay();
  return SEML_OK;
}

/**
 * @brief I2C发送停止信号
 * @param[in] hi2c 软件I2C接口句柄
 */
static void Software_I2C_Stop(Software_I2C_HandleTypeDef *hi2c) {
  /* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
  SEML_GPIO_Pin_Set(&hi2c->SCL);
  i2c_delay();
  SEML_GPIO_Pin_Set(&hi2c->SDA);
  i2c_delay();
}

/**
 * @brief I2C发送数据
 * @param[in] hi2c 软件I2C接口句柄
 * @param[in] _ucByte 发送数据
 * @return 函数执行状态
 * @retval SEML_BUSY 总线被占用
 * @retval SEML_OK 执行完成
 */
static SEML_StatusTypeDef
Software_I2C_SendByte(Software_I2C_HandleTypeDef *hi2c, uint8_t _ucByte) {
  uint8_t i, temp;
  // 保持数据传输稳定，在传输数据的时候禁止被中断打断
  __disable_irq();
  /* 先发送字节的高位bit7 */
  for (i = 0; i < 8; i++) {
    temp = (_ucByte & 0x80) != 0x00;
    SEML_GPIO_Pin_Output(&hi2c->SDA, temp);
    i2c_delay();
    if (SEML_GPIO_Pin_Input(&hi2c->SDA) != temp) {
      __enable_irq();
      return SEML_BUSY;
    }
    SEML_GPIO_Pin_Set(&hi2c->SCL);
    i2c_delay();
    SEML_GPIO_Pin_Reset(&hi2c->SCL);
    i2c_delay();
    _ucByte <<= 1; /* 左移一个bit */
  }
  SEML_GPIO_Pin_Reset(&hi2c->SDA);
  __enable_irq();
  i2c_delay();
  return SEML_OK;
}

/**
 * @brief I2C读取数据
 * @param[in] hi2c 软件I2C接口句柄
 * @param[out] _ucByte 接收到的数据
 */
static void Software_I2C_ReadByte(Software_I2C_HandleTypeDef *hi2c,
                                  uint8_t *_ucByte) {
  uint8_t i;
  uint8_t value;
  __disable_irq();
  // 释放SDA以便传输数据
  SEML_GPIO_Pin_Set(&hi2c->SDA);
  i2c_delay();
  /* 读到第1个bit为数据的bit7 */
  value = 0;
  for (i = 0; i < 8; i++) {
    value <<= 1;
    SEML_GPIO_Pin_Set(&hi2c->SCL);
    i2c_delay();
    if (SEML_GPIO_Pin_Input(&hi2c->SDA)) {
      value |= 0x01;
    }
    SEML_GPIO_Pin_Reset(&hi2c->SCL);
    i2c_delay();
  }
  SEML_GPIO_Pin_Reset(&hi2c->SDA);
  *_ucByte = value;
  __enable_irq();
  i2c_delay();
}

/**
 * @brief I2C等待应答信号
 * @param[in] hi2c 软件I2C接口句柄
 * @param[in] timeout 超时时间
 * @return 应答状态
 * @retval SEML_TIMEOUT 总线应答超时
 * @retval SEML_OK 总线应答
 */
static SEML_StatusTypeDef Software_I2C_WaitAck(Software_I2C_HandleTypeDef *hi2c,
                                               uint32_t timeout) {
  uint8_t ack = SET;
  uint32_t tickstart = SEML_GetTick();
  SEML_GPIO_Pin_Set(&hi2c->SDA);
  i2c_delay();
  SEML_GPIO_Pin_Set(&hi2c->SCL);
  i2c_delay();
  while ((SEML_GetTick() - tickstart < timeout) && ack)
    ack = SEML_GPIO_Pin_Input(&hi2c->SDA);
  SEML_GPIO_Pin_Reset(&hi2c->SCL);
  i2c_delay();
  SEML_GPIO_Pin_Reset(&hi2c->SDA);
  i2c_delay();
  return ack ? SEML_TIMEOUT : SEML_OK;
}

/**
 * @brief I2C发送ACK应答
 * @param[in] hi2c 软件I2C接口句柄
 */
static void Software_I2C_Ack(Software_I2C_HandleTypeDef *hi2c) {
  SEML_GPIO_Pin_Reset(&hi2c->SDA);
  i2c_delay();
  SEML_GPIO_Pin_Set(&hi2c->SCL);
  i2c_delay();
  SEML_GPIO_Pin_Reset(&hi2c->SCL);
  i2c_delay();
}

/**
 * @brief I2C发送NACK应答
 * @param[in] hi2c 软件I2C接口句柄
 */
static void Software_I2C_NAck(Software_I2C_HandleTypeDef *hi2c) {
  SEML_GPIO_Pin_Set(&hi2c->SDA);
  i2c_delay();
  SEML_GPIO_Pin_Set(&hi2c->SCL);
  i2c_delay();
  SEML_GPIO_Pin_Reset(&hi2c->SCL);
  i2c_delay();
  SEML_GPIO_Pin_Reset(&hi2c->SDA);
  i2c_delay();
}

#define __SEML_I2C_ERROR_HANDLE(hi2c, error_code)                              \
  do {                                                                         \
    Software_I2C_Stop(hi2c);                                                   \
    __SEML_UNLOCK(hi2c);                                                       \
    return error_code;                                                         \
  } while (0)

/**
 * @brief 软件i2c初始化
 * @param[out] hi2c 软件i2c句柄
 * @param[in] GPIO_SDA_Part SDA的GPIO端口
 * @param[in] GPIO_SDA_Pin SDA的GPIO引脚
 * @param[in] GPIO_SCL_Part SCL的GPIO端口
 * @param[in] GPIO_SCL_Pin SCL的GPIO引脚
 */
void Software_I2C_Init(Software_I2C_HandleTypeDef *hi2c,
                       GPIO_TypeDef *GPIO_SDA_Part, uint16_t GPIO_SDA_Pin,
                       GPIO_TypeDef *GPIO_SCL_Part, uint16_t GPIO_SCL_Pin) {
  assert_param(hi2c != NULL);
  assert_param(GPIO_SDA_Part != NULL);
  assert_param(GPIO_SCL_Part != NULL);

  SEML_GPIO_Pin_Register(&hi2c->SDA, GPIO_SDA_Part, GPIO_SDA_Pin);
  SEML_GPIO_Pin_Register(&hi2c->SCL, GPIO_SCL_Part, GPIO_SCL_Pin);
  Software_I2C_Stop(hi2c);
  hi2c->Lock = SEML_UNLOCKED;
}

/**
 * @brief 软件i2c接收函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[out] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 * @return 函数执行状态
 * @retval SEML_BUSY 总线被占用
 * @retval SEML_TIMEOUT 总线应答超时
 * @retval SEML_OK 执行完成
 */
SEML_StatusTypeDef Software_I2C_Receive(Software_I2C_HandleTypeDef *hi2c,
                                        uint16_t DevAddress, uint8_t *pData,
                                        uint16_t Size, uint32_t Timeout) {

  assert_param(hi2c != NULL);
  assert_param(pData != NULL);
  __SEML_LOCK(hi2c);
  // 发送起始信号
  if (Software_I2C_Start(hi2c) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
  // 发送从机地址 读
  if (Software_I2C_SendByte(hi2c, DevAddress | 0x01) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
  if (Software_I2C_WaitAck(hi2c, Timeout) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_TIMEOUT);
  // 发送数据
  for (uint16_t i = 0; i < Size; i++) {
    Software_I2C_ReadByte(hi2c, pData++);
    // 最后一个数据接收完毕发送NACK
    if (i == Size - 1)
      Software_I2C_NAck(hi2c);
    else
      Software_I2C_Ack(hi2c);
  }
  // 发送停止信号
  Software_I2C_Stop(hi2c);
  __SEML_UNLOCK(hi2c);
  return SEML_OK;
}

/**
 * @brief 软件i2c发送函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[in] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 * @return 函数执行状态
 * @retval SEML_BUSY 总线被占用
 * @retval SEML_TIMEOUT 总线应答超时
 * @retval SEML_OK 执行完成
 */
SEML_StatusTypeDef Software_I2C_Transmit(Software_I2C_HandleTypeDef *hi2c,
                                         uint16_t DevAddress, uint8_t *pData,
                                         uint16_t Size, uint32_t Timeout) {
  assert_param(hi2c != NULL);
  assert_param(pData != NULL);
  __SEML_LOCK(hi2c);
  // 发送起始信号
  if (Software_I2C_Start(hi2c) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
  // 发送从机地址 写
  if (Software_I2C_SendByte(hi2c, DevAddress | 0x00) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
  if (Software_I2C_WaitAck(hi2c, Timeout) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_TIMEOUT);
  // 发送数据
  for (uint16_t i = 0; i < Size; i++) {
    Software_I2C_SendByte(hi2c, *pData++);
    // 最后一个数据发送完毕发送NACK
    if (i == Size - 1)
      Software_I2C_NAck(hi2c);
    else
      Software_I2C_Ack(hi2c);
  }
  // 发送停止信号
  Software_I2C_Stop(hi2c);
  __SEML_UNLOCK(hi2c);
  return SEML_OK;
}

/**
 * @brief 软件i2c读寄存器函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[in] MemAddress 内存地址
 * @param[in] MemAddSize 内存地址大小
 *  - I2C_MEMADD_SIZE_8BIT 内存地址为1字节
 *  - I2C_MEMADD_SIZE_16BIT 内存地址为2字节
 * @param[out] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 * @return 函数执行状态
 * @retval SEML_BUSY 总线被占用
 * @retval SEML_TIMEOUT 总线应答超时
 * @retval SEML_OK 执行完成
 */
SEML_StatusTypeDef Software_I2C_Mem_Read(Software_I2C_HandleTypeDef *hi2c,
                                         uint16_t DevAddress,
                                         uint16_t MemAddress,
                                         uint16_t MemAddSize, uint8_t *pData,
                                         uint16_t Size, uint32_t Timeout) {
  assert_param(hi2c != NULL);
  assert_param(pData != NULL);
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));
  __SEML_LOCK(hi2c);
  // 发送起始信号
  if (Software_I2C_Start(hi2c) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
  // 发送从机地址 写
  if (Software_I2C_SendByte(hi2c, DevAddress | 0x00) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
  if (Software_I2C_WaitAck(hi2c, Timeout) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_TIMEOUT);
  // 发送寄存器地址
  if (MemAddSize == I2C_MEMADD_SIZE_8BIT) {
    // 8位寄存器地址
    if (Software_I2C_SendByte(hi2c, MemAddress & 0x00ff) != SEML_OK)
      __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
    if (Software_I2C_WaitAck(hi2c, Timeout) != SEML_OK)
      __SEML_I2C_ERROR_HANDLE(hi2c, SEML_TIMEOUT);
  } else {
    // 16位寄存器地址
    // 发送高八位
    if (Software_I2C_SendByte(hi2c, (MemAddress & 0xff00) >> 8) != SEML_OK)
      __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
    if (Software_I2C_WaitAck(hi2c, Timeout) != SEML_OK)
      __SEML_I2C_ERROR_HANDLE(hi2c, SEML_TIMEOUT);
    // 发送低八位
    if (Software_I2C_SendByte(hi2c, MemAddress & 0x00ff) != SEML_OK)
      __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
    if (Software_I2C_WaitAck(hi2c, Timeout) != SEML_OK)
      __SEML_I2C_ERROR_HANDLE(hi2c, SEML_TIMEOUT);
  }
  // 发送起始信号
  if (Software_I2C_Start(hi2c) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
  // 发送从机地址 读
  if (Software_I2C_SendByte(hi2c, DevAddress | 0x01) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
  if (Software_I2C_WaitAck(hi2c, Timeout) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_TIMEOUT);
  // 接收数据
  for (uint16_t i = 0; i < Size; i++) {
    Software_I2C_ReadByte(hi2c, pData++);
    // 最后一个数据接收完毕发送NACK
    if (i == Size - 1)
      Software_I2C_NAck(hi2c);
    else
      Software_I2C_Ack(hi2c);
  }
  // 发送停止信号
  Software_I2C_Stop(hi2c);
  __SEML_UNLOCK(hi2c);
  return SEML_OK;
}

/**
 * @brief 软件i2c写寄存器函数
 * @param[in] I2C_Handle i2c句柄
 * @param[in] DevAddress 设备地址
 * @param[in] MemAddress 内存地址
 * @param[in] MemAddSize 内存地址大小
 *  - I2C_MEMADD_SIZE_8BIT 内存地址为1字节
 *  - I2C_MEMADD_SIZE_16BIT 内存地址为2字节
 * @param[in] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 * @return 函数执行状态
 * @retval SEML_BUSY 总线被占用
 * @retval SEML_TIMEOUT 总线应答超时
 * @retval SEML_OK 执行完成
 */
SEML_StatusTypeDef Software_I2C_Mem_Write(Software_I2C_HandleTypeDef *hi2c,
                                          uint16_t DevAddress,
                                          uint16_t MemAddress,
                                          uint16_t MemAddSize, uint8_t *pData,
                                          uint16_t Size, uint32_t Timeout) {
  assert_param(hi2c != NULL);
  assert_param(pData != NULL);
  assert_param(IS_I2C_MEMADD_SIZE(MemAddSize));
  __SEML_LOCK(hi2c);
  // 发送起始信号
  if (Software_I2C_Start(hi2c) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
  // 发送从机地址 写
  if (Software_I2C_SendByte(hi2c, DevAddress | 0x00) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
  if (Software_I2C_WaitAck(hi2c, Timeout) != SEML_OK)
    __SEML_I2C_ERROR_HANDLE(hi2c, SEML_TIMEOUT);
  // 发送寄存器地址
  if (MemAddSize == I2C_MEMADD_SIZE_8BIT) {
    if (Software_I2C_SendByte(hi2c, MemAddress & 0x00ff) != SEML_OK)
      __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
    if (Software_I2C_WaitAck(hi2c, Timeout) != SEML_OK)
      __SEML_I2C_ERROR_HANDLE(hi2c, SEML_TIMEOUT);
  } else {
    // 发送高八位
    if (Software_I2C_SendByte(hi2c, (MemAddress & 0xff00) >> 8) != SEML_OK)
      __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
    if (Software_I2C_WaitAck(hi2c, Timeout) != SEML_OK)
      __SEML_I2C_ERROR_HANDLE(hi2c, SEML_TIMEOUT);
    // 发送低八位
    if (Software_I2C_SendByte(hi2c, MemAddress & 0x00ff) != SEML_OK)
      __SEML_I2C_ERROR_HANDLE(hi2c, SEML_BUSY);
    if (Software_I2C_WaitAck(hi2c, Timeout) != SEML_OK)
      __SEML_I2C_ERROR_HANDLE(hi2c, SEML_TIMEOUT);
  }
  // 发送数据
  for (uint16_t i = 0; i < Size; i++) {
    Software_I2C_SendByte(hi2c, *pData++);
    // 最后一个数据发送完毕发送NACK
    if (i == Size - 1)
      Software_I2C_NAck(hi2c);
    else
      Software_I2C_Ack(hi2c);
  }
  // 发送停止信号
  Software_I2C_Stop(hi2c);
  __SEML_UNLOCK(hi2c);
}
