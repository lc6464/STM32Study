#include "spi_if.h"

/**
 * @brief SPI注册函数
 * @param[out] SPI_Handle SPI句柄
 * @param[in] hspi SPI接口句柄
 * @param[in] SPI_Transmit SPI发送函数
 * @param[in] SPI_Receive SPI接收函数
 * @param[in] SPI_TransmitReceive SPI发送接收函数
 */
void SEML_SPI_Register(SPI_Handle_t *SPI_Handle, void *hspi,
                        SPI_Transmit_t SPI_Transmit, SPI_Receive_t SPI_Receive, SPI_TransmitReceive_t SPI_TransmitReceive)
{
    assert_param(SPI_Handle != NULL);
    assert_param(hspi != NULL);
    assert_param(SPI_Transmit != NULL);
    assert_param(SPI_Receive != NULL);
    assert_param(SPI_TransmitReceive != NULL);

    SPI_Handle->hspi = hspi;
    SPI_Handle->SPI_Receive = SPI_Receive;
    SPI_Handle->SPI_Transmit = SPI_Transmit;
    SPI_Handle->SPI_TransmitReceive = SPI_TransmitReceive;
}

/**
 * @brief SPI接收函数
 * @param[in] SPI_Handle SPI句柄
 * @param[out] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_SPI_Receive(SPI_Handle_t *SPI_Handle, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    assert_param(SPI_Handle != NULL);
    assert_param(pData != NULL);

    return SPI_Handle->SPI_Receive(SPI_Handle->hspi, pData, Size, Timeout);
}

/**
 * @brief SPI发送函数
 * @param[in] SPI_Handle SPI句柄
 * @param[in] pData 数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_SPI_Transmit(SPI_Handle_t *SPI_Handle, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    assert_param(SPI_Handle != NULL);
    assert_param(pData != NULL);

    return SPI_Handle->SPI_Transmit(SPI_Handle->hspi, pData, Size, Timeout);
}

/**
 * @brief SPI发送接收函数
 * @param[in] SPI_Handle SPI句柄
 * @param[in] pTxData 发送数据指针
 * @param[out] pRxData 接收数据指针
 * @param[in] Size 数据大小
 * @param[in] Timeout 超时时间
 */
SEML_StatusTypeDef SEML_SPI_TransmitReceive(SPI_Handle_t *SPI_Handle, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size,uint32_t Timeout)
{
    assert_param(SPI_Handle != NULL);
    assert_param(pTxData != NULL);
    assert_param(pRxData != NULL);

    return SPI_Handle->SPI_TransmitReceive(SPI_Handle->hspi, pTxData, pRxData, Size, Timeout);
}
