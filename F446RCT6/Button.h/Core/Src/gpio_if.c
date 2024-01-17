#include "gpio_if.h"

/**
 * @brief GPIO引脚输出置1
 * @param GPIO_Handle GPIO句柄
 */
void SEML_GPIO_Pin_Set(GPIO_Handle_t *GPIO_Handle)
{
	if (GPIO_Handle->GPIO_Part == NULL)
		return;
	HAL_GPIO_WritePin((GPIO_Handle)->GPIO_Part, (GPIO_Handle)->GPIO_Pin, SET);
}
/**
 * @brief GPIO引脚输出置0
 * @param GPIO_Handle GPIO句柄
 */
void SEML_GPIO_Pin_Reset(GPIO_Handle_t *GPIO_Handle)
{
	if (GPIO_Handle->GPIO_Part == NULL)
		return;
	HAL_GPIO_WritePin((GPIO_Handle)->GPIO_Part, (GPIO_Handle)->GPIO_Pin, RESET);
}
/**
 * @brief GPIO引脚输出
 * @param GPIO_Handle GPIO句柄
 * @param status 输出状态
 */
void SEML_GPIO_Pin_Output(GPIO_Handle_t *GPIO_Handle, uint16_t status)
{
	if (GPIO_Handle->GPIO_Part == NULL)
		return;
	HAL_GPIO_WritePin((GPIO_Handle)->GPIO_Part, (GPIO_Handle)->GPIO_Pin, status);
}
/**
 * @brief GPIO引脚输出反转
 * @param GPIO_Handle GPIO句柄
 */
void SEML_GPIO_Pin_Toggle(GPIO_Handle_t *GPIO_Handle)
{
	if (GPIO_Handle->GPIO_Part == NULL)
		return;
	HAL_GPIO_TogglePin((GPIO_Handle)->GPIO_Part, (GPIO_Handle)->GPIO_Pin);
}
/**
 * @brief GPIO引脚输入
 * @param GPIO_Handle GPIO句柄
 */
uint16_t SEML_GPIO_Pin_Input(GPIO_Handle_t *GPIO_Handle)
{
	if (GPIO_Handle->GPIO_Part == NULL)
		return 0;
	return HAL_GPIO_ReadPin((GPIO_Handle)->GPIO_Part, (GPIO_Handle)->GPIO_Pin);
}
