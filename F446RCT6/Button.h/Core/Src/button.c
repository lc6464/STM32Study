#include "button.h"
Button_t *button_root;

#define Get_Button_Event_flag(handler, event_) (handler->event == event_)

#define Button_Event(handler, event_)              \
	do                                               \
	{                                                \
		handler->event = event_;                       \
		if (handler->callback_fun[event_ - 1] != NULL) \
			handler->callback_fun[event_ - 1](handler);  \
	} while (0)

/**
 * @brief 向按键链表添加节点
 * @param button 按键句柄
 */
static void Button_linked_add(Button_t *button)
{
	Button_t *button_temp = button_root;
	if (button_root == NULL)
	{
		button_root = button;
		return;
	}
	// 遍历链表到子节点为空的节点
	while (button_temp->next)
	{
		// 防止链表中元素重新加入
		if (button_temp == button)
			return;
		button_temp = button_temp->next;
	}

	button_temp->next = button;
}
#ifdef USE_SEML_LIB
/**
 * @brief 按键初始化
 * @param button 按键句柄
 * @param active_level 按键按下电平
 */
void Button_Init(Button_t *button, FunctionalState active_level)
{
	assert_param(button != NULL);
	assert_param(active_level == 0 || active_level == 1);

	button->active_level = active_level;
	button->event = 0;
	button->press_down_time = 0;
	button->press_up_time = 0;
	button->press_count = 0;
	button->clicks_count = 0;
	for (int i = 0; i < Button_Event_Num; i++)
		button->callback_fun[i] = 0;
	button->next = NULL;
	Button_linked_add(button);
}
#else
/**
 * @brief 按键初始化
 *
 * @param button 按键句柄
 * @param get_level_fun 获取按键函数
 * @param get_level_config 获取按键配置
 * @param active_level 按键按下电平
 */
void Button_Init(Button_t *button, uint8_t (*get_level_fun)(void *), void *get_level_config, FunctionalState active_level)
{
	assert_param(button != NULL);
	assert_param(get_level_fun != NULL);
	assert_param(get_level_config != NULL);
	assert_param(active_level == 0 || active_level == 1);

	button->active_level = active_level;
	button->get_level_fun = get_level_fun;
	button->get_level_conf = get_level_config;
	button->event = 0;
	button->press_down_time = 0;
	button->press_up_time = 0;
	button->press_count = 0;
	button->clicks_count = 0;
	for (int i = 0; i < Button_Event_Num; i++)
		button->callback_fun[i] = 0;
	button->next = NULL;
	Button_linked_add(button);
}
#endif
/**
 * @brief 按键事件回调注册
 * @param button 按键句柄
 * @param event 按键事件
 * @param callback_fun 事件回调函数
 */
void Button_Register(Button_t *button, Button_Event_t event, void (*callback_fun)(Button_t *))
{
	assert_param(button != NULL);
	assert_param(callback_fun != NULL);
	assert_param(IS_BUTTON_EVENT(event));
	button->callback_fun[event - 1] = callback_fun;
}

/**
 * @brief 按键状态更新函数
 * @param button 按键句柄
 */
void Button_Handle(Button_t *button)
{
	assert_param(button != NULL);
#ifndef Button_GetTick
#define Button_GetTick() button_tick
#if Button_Sample_Time < 1
#error Button in the sampling time is less than 1.(button.c)
#endif
	static uint32_t button_tick;
	button_tick += Button_Sample_Time;
#endif
#ifdef USE_SEML_LIB
	uint8_t button_level = SEML_GPIO_Pin_Input(&button->GPIO_Handle);
#else
	uint8_t button_level = button->get_level_fun(button->get_level_conf);
#endif
	// 按键消抖
	if (button_level == button->active_level)
		button->press_count = (button->press_count >= 2 * (Button_Sampling_Window)-1) ? 2 * (Button_Sampling_Window)-1 : button->press_count + 1;
	else
		button->press_count = (button->press_count <= 0) ? 0 : button->press_count - 1;
	// 确认按键状态,开始处理
	if (button->press_count >= Button_Sampling_Window)
	{
		// 按键按下
		switch (button->status)
		{
		case 0:
		case 2:
			// 产生按键按下事件
			button->press_down_time = Button_GetTick();
			button->clicks_count++;
			Button_Event(button, Press_Down);
			button->status = 1;
			break;
		case 1:
			// 按键按着
			if (Button_GetTick() - button->press_down_time > Button_Long_Press_Threshold)
			{
				// 产生长按开始事件
				if (!button->long_press_flag)
				{
					Button_Event(button, Long_Press_Start);
					button->long_press_flag = 1;
				}
				// 产生长按事件
				Button_Event(button, Long_Press_Hold);
			}
			break;
		}
	}
	else
	{
		// 按键松开
		switch (button->status)
		{
		case 1:
			button->press_up_time = Button_GetTick();
			// 产生按键松开事件
			Button_Event(button, Press_Up);
			// 产生长按松开事件
			if (button->long_press_flag)
				Button_Event(button, Long_press_Release);
			button->status = 2;
			// 重置按下时间戳
			button->press_down_time = 0;
			break;
		case 2:
			// 等待再按下超时
			if (Button_GetTick() - button->press_up_time > Button_Clink_Timeout && button->press_up_time != 0)
			{
				// 长按和点击互斥
				if (!button->long_press_flag)
				{
					switch (button->clicks_count)
					{
					case 0:
						assert_param(0);
						break;
					// 产生单击按键事件
					case 1:
						Button_Event(button, Single_Clink);
						break;
					// 产生双击按键事件
					case 2:
						Button_Event(button, Double_Clink);
						break;
					// 产生多次点击按键事件
					default:
						Button_Event(button, Multiple_clicks);
						break;
					}
				}
				button->press_up_time = 0;
				button->clicks_count = 0;
				button->status = 0;
			}
			break;
		default:
			button->status = 0;
			button->long_press_flag = 0;
			break;
		}
	}
}

/**
 * @brief 按键扫描函数
 * 对所有按键进行扫描
 */
void Button_Scan(void *config)
{
	Button_t *button_temp = button_root;
	if (button_temp != NULL)
	{
		while (button_temp)
		{
			Button_Handle(button_temp);
			button_temp = button_temp->next;
		}
	}
}