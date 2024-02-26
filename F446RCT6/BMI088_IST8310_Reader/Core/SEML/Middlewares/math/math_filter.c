#include "math_filter.h"
#include "math_common.h"

/**
 * @brief 均方根滤波器初始化
 * @param[out] RMS_filter  均方根配置结构体指针
 * @param[in] window_width 滑动窗口大小
 * @param[in] init_value 初始值
 * @param[in,out] buffer 队列缓存数组
*/
void RMS_filter_Init(RMS_filter_t *RMS_filter,const uint16_t window_width,const float init_value,float *buffer)
{
    uint16_t i;
    float temp = init_value * init_value / window_width;
    RMS_filter->window_width = window_width;
    RMS_filter->gain = 1.0f/window_width;
    RMS_filter->square_sum = init_value * init_value;
    if(InitQueue(&RMS_filter->data_queue,sizeof(float),window_width,buffer,queue_full_hander_error) == queue_no_error)
    {
        for(i = 0;i < window_width;i++)
            EnQueue(&RMS_filter->data_queue,&temp);
    }
}

/**
 * @brief 窗口均方根滤波器
 * @param[in,out] RMS_filter 滤波器指针
 * @param[in] data 输入数据
 * @return 滤波后结果
*/
float RMS_filter(RMS_filter_t *RMS_filter,const float data)
{
    float old_square_data,square_data = data * data * RMS_filter->gain;
    
    DeQueue(&RMS_filter->data_queue,&old_square_data);
    EnQueue(&RMS_filter->data_queue,&square_data);

    RMS_filter->square_sum += square_data - old_square_data;
    RMS_filter->RMS_value = math_sqrt(RMS_filter->square_sum);
    return RMS_filter->RMS_value;
}

/**
 * @brief 删除均方根滤波器
 * @param[out] RMS_filter 均方根配置结构体指针
 */
void RMS_filter_DeInit(RMS_filter_t *RMS_filter)
{
    RMS_filter->window_width = 0;
    RMS_filter->square_sum = 0;
    DeleteQueue(&RMS_filter->data_queue);
}


/**
 * @brief 滑动均值滤波器初始化
 * @param[out] ave_filter  滑动均值配置结构体指针
 * @param[in] window_width 滑动窗口大小
 * @param[in] init_value 初始值
 * @param[in,out] buffer 队列缓存数组
*/
void average_filter_Init(average_filter_t *ave_filter,const uint16_t window_width,const float init_value,float *buffer)
{
    uint16_t i;
    float temp = init_value / window_width;
    ave_filter->window_width = window_width;
    ave_filter->gain = 1.0f/window_width;
    ave_filter->average_value = init_value;
    if(InitQueue(&ave_filter->data_queue,sizeof(float),window_width,buffer,queue_full_hander_error) == queue_no_error)
    {
        for(i = 0;i < window_width;i++)
            EnQueue(&ave_filter->data_queue,&temp);
    }
}

/**
 * @brief 滑动均值滤波器
 * @param[in,out] ave_filter 滤波器指针
 * @param[in] data 输入数据
 * @return 滤波后结果
*/
float average_filter(average_filter_t *ave_filter,const float data)
{
    float old_data,new_data = data * ave_filter->gain;
    DeQueue(&ave_filter->data_queue,&old_data);
    EnQueue(&ave_filter->data_queue,&new_data);

    ave_filter->average_value += new_data - old_data;

    return ave_filter->average_value;
}

/**
 * @brief 删除均值滤波器
 * @param[out] ave_filter 滤波器指针
 */
void average_filter_DeInit(average_filter_t *ave_filter)
{
    ave_filter->window_width = 0;
    ave_filter->average_value = 0;
    DeleteQueue(&ave_filter->data_queue);
}
