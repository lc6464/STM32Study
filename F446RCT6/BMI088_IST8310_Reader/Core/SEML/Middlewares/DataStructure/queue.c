#include "queue.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief 初始化队列
 * @param[in,out] queue 队列结构体指针
 * @param[in] elem_size 元素大小，通常是sizeof()
 * @param[in] size 队列大小
 * @param[in] buffer 缓存数组，使用栈区自动分配可传NULL,传入大小需比申请尺寸大1
 * @param[in] full_hander 满队处理方式
 * @attention 使用外部缓存数组时候满队处理方式不能使用queue_full_hander_reapply
 * @return 队列执行状态
 */
SEML_StatusTypeDef InitQueue(s_queue *queue, const uint16_t elem_size,
                             const uint16_t size, void *buffer,
                             const queue_full_hander_t full_hander) {
  assert_param(queue != NULL);
  assert_param(IS_QUEUE_FULL_HANDER(full_hander));
  uint16_t queue_size = size + 1;
  queue->elem_size = elem_size;
  queue->size = queue_size;
  queue->front = 0;
  queue->rear = 0;
  queue->use_extern_buffer = DISABLE;
  // 自动在堆区申请
  if (buffer == NULL) {
    queue->address = calloc(queue_size, elem_size);
    // 申请失败
    if (queue->address == NULL) {
      queue->error_code = queue_reapply_error;
      return SEML_ERROR;
    }
  }
  // 使用外部缓存数组
  else {
    queue->address = buffer;
    queue->use_extern_buffer = ENABLE;
    // 防止使用外部缓存数组时候误使用内存分配
    if (full_hander == queue_full_hander_reapply) {
      queue->error_code = queue_config_error;
      assert_param(full_hander != queue_full_hander_reapply);
    }
  }
  queue->full_hander = full_hander;

  return SEML_OK;
}

/**
 * @brief 删除队列
 * @param[in,out] queue 队列结构体指针
 */
void DeleteQueue(s_queue *queue) {
  assert_param(queue != NULL);
  queue->elem_size = 0;
  queue->size = 0;
  queue->front = 0;
  queue->rear = 0;
  queue->full_hander = queue_full_hander_error;
  if (queue->address != NULL && queue->use_extern_buffer == 0)
    free(queue->address);
  queue->use_extern_buffer = 0;
  queue->address = NULL;
}

/**
 * @brief 队列重新申请空间
 * @param[in,out] queue 队列结构体指针
 * @return 队列执行状态
 */
static SEML_StatusTypeDef queue_reapply(s_queue *queue) {
  assert_param(queue != NULL);
  assert_param(queue->address != NULL);

  void *new_queue;
  const uint16_t last_size = queue->size;
  uint16_t reapply_size = QueueReapplySize;
  // 申请空间
  new_queue =
      realloc(queue->address, (queue->size + reapply_size) * queue->elem_size);
  // 检查是否申请成功
  if (new_queue == NULL) {
    queue->error_code = queue_reapply_error;
    return SEML_ERROR;
  }
  queue->size += reapply_size;
  queue->address = new_queue;
  // 若队列出现循环 则对其进行处理
  if (queue->front > queue->rear) {
    if (queue->rear < reapply_size) {
      // 队列尾小于扩展大小，循环部分放入新扩展空间里
      // for (int i = 0; i < queue->rear; i++)
      memcpy((uint8_t *)queue->address + last_size * queue->elem_size,
             (uint8_t *)queue->address, queue->elem_size * queue->rear);
      // 队尾移动到新位置
      queue->rear += last_size;
    } else {
      // 队列尾大于扩展大小，小于扩展大小的循环部分放入新扩展空间里
      // 大于扩展大小部分前移
      for (int i = 0; i < queue->rear; i++) {
        if (i < reapply_size)
          memcpy((uint8_t *)queue->address + (last_size + i) * queue->elem_size,
                 (uint8_t *)queue->address + i * queue->elem_size,
                 queue->elem_size);
        else
          memcpy((uint8_t *)queue->address +
                     (last_size + i - reapply_size) * queue->elem_size,
                 (uint8_t *)queue->address + i * queue->elem_size,
                 queue->elem_size);
      }
      // 大于拓展部份前移
      if (queue->rear != 0)
        queue->rear -= reapply_size;
      else
        queue->rear = queue->size - reapply_size;
    }
  }
  return SEML_OK;
}

/**
 * @brief 队尾入队
 * @param[in,out] queue 队列结构体指针
 * @param[in] data 数据指针
 * @return 队列执行状态
 */
SEML_StatusTypeDef EnQueue(s_queue *queue, const void *data) {

  assert_param(queue != NULL);
  assert_param(data != NULL);
  assert_param(queue->address != NULL);
  assert_param(IS_QUEUE_FULL_HANDER(queue->full_hander));
  __SEML_LOCK(queue);
  // 满队处理
  if ((queue->rear + 1) % queue->size == queue->front) {
    switch (queue->full_hander) {
    case queue_full_hander_cover:
      queue->front = (queue->front + 1) % queue->size;
      break;
    case queue_full_hander_reapply:
      if (queue_reapply(queue) != SEML_OK) {
        queue->error_code = queue_reapply_error;
        __SEML_UNLOCK(queue);
        return SEML_ERROR;
      }
      break;
    case queue_full_hander_error:
      queue->error_code = queue_full_error;
      __SEML_UNLOCK(queue);
      return SEML_ERROR;
    }
  }

  // 入队
  memcpy((uint8_t *)queue->address + queue->rear * queue->elem_size, data,
         queue->elem_size);
  queue->rear += 1;

  // 循环队列处理
  if (queue->rear == queue->size)
    queue->rear = 0;
  __SEML_UNLOCK(queue);
  return SEML_OK;
}

/**
 * @brief 队头出队
 * @param[in,out] queue 队列结构体指针
 * @param[out] data 数据指针
 * @return 队列执行状态
 */
SEML_StatusTypeDef DeQueue(s_queue *queue, void *data) {
  assert_param(queue != NULL);
  assert_param(data != NULL);
  assert_param(queue->address != NULL);
  __SEML_LOCK(queue);
  // 空队处理
  if (queue->rear == queue->front) {
    queue->error_code = queue_empty_error;
    __SEML_UNLOCK(queue);
    return SEML_ERROR;
  }

  // 出队
  memcpy(data, (uint8_t *)queue->address + queue->front * queue->elem_size,
         queue->elem_size);
  queue->front += 1;

  // 循环队列处理
  if (queue->front >= queue->size)
    queue->front = 0;
  __SEML_UNLOCK(queue);
  return SEML_OK;
}

/**
 * @brief 队尾出队
 * @param[in,out] queue 队列结构体指针
 * @param[out] data 数据指针
 * @return 队列执行状态
 */
SEML_StatusTypeDef DeQueueRear(s_queue *queue, void *data) {
  assert_param(queue != NULL);
  assert_param(data != NULL);
  assert_param(queue->address != NULL);
  __SEML_LOCK(queue);
  // 空队处理
  if (queue->rear == queue->front) {
    queue->error_code = queue_empty_error;
    __SEML_UNLOCK(queue);
    return SEML_ERROR;
  }

  // 循环队列处理
  if (queue->rear == 0)
    queue->rear = queue->size - 1;
  else
    queue->rear -= 1;

  // 出队
  memcpy(data, (uint8_t *)queue->address + queue->rear * queue->elem_size,
         queue->elem_size);
  __SEML_UNLOCK(queue);
  return SEML_OK;
}

/**
 * @brief 获取队头数据(不出队)
 * @param[in,out] queue 队列结构体指针
 * @param[out] data 数据指针
 * @return 队列执行状态
 */
SEML_StatusTypeDef GetQueueFront(s_queue *queue, void *data) {
  assert_param(queue != NULL);
  assert_param(data != NULL);
  assert_param(queue->address != NULL);
  __SEML_LOCK(queue);
  // 空队处理
  if (queue->rear == queue->front) {
    queue->error_code = queue_empty_error;
    __SEML_UNLOCK(queue);
    return SEML_ERROR;
  }

  // 读取队头数据
  memcpy(data, (uint8_t *)queue->address + queue->front * queue->elem_size,
         queue->elem_size);
  __SEML_UNLOCK(queue);
  return SEML_OK;
}

/**
 * @brief 获取队尾数据(不出队)
 * @param[in,out] queue 队列结构体指针
 * @param[out] data 数据指针
 * @return 队列执行状态
 */
SEML_StatusTypeDef GetQueueRear(s_queue *queue, void *data) {
  uint16_t temp;
  assert_param(queue != NULL);
  assert_param(data != NULL);
  assert_param(queue->address != NULL);
  __SEML_LOCK(queue);
  // 空队处理
  if (queue->rear == queue->front) {
    queue->error_code = queue_empty_error;
    __SEML_UNLOCK(queue);
    return SEML_ERROR;
  }

  // 循环队列处理
  if (queue->rear == 0)
    temp = queue->size - 1;
  else
    temp = queue->rear - 1;

  // 读取队头数据
  memcpy(data, (uint8_t *)queue->address + temp * queue->elem_size,
         queue->elem_size);
  __SEML_UNLOCK(queue);
  return SEML_OK;
}

/**
 * @brief 获取队列长度
 * @param[in] queue 队列结构体指针
 * @return 队列长度
 */
uint16_t GetQueueLong(const s_queue *queue) {
  int32_t temp = queue->rear - queue->front;
  if (temp < 0)
    temp += queue->size;
  return temp;
}

/**
 * @brief 访问队列内元素
 * @param[in,out] queue 队列结构体指针
 * @param[in] index 元素下标,小于队列长度
 * @param[in,out] data 数据指针
 * @param[in] status 读写状态
 * @return 队列执行状态
 */
SEML_StatusTypeDef VisitQueueElem(s_queue *queue, uint16_t index, void *data,
                                  queue_read_while_t status) {
  uint16_t temp;

  assert_param(IS_QUEUE_READ_WHILE(status));
  assert_param(queue != NULL);
  assert_param(data != NULL);
  __SEML_LOCK(queue);
  if (GetQueueLong(queue) <= index) {
    queue->error_code = queue_visit_error;
    __SEML_UNLOCK(queue);
    return SEML_ERROR;
  }

  temp = queue->front + index;
  temp -= temp >= queue->size ? queue->size : 0;

  switch (status) {
  case queue_read:
    memcpy(data, (uint8_t *)queue->address + temp * queue->elem_size,
           queue->elem_size);
    break;
  case queue_write:
    memcpy((uint8_t *)queue->address + temp * queue->elem_size, data,
           queue->elem_size);
    break;
  }
  __SEML_UNLOCK(queue);
  return SEML_OK;
}

/*******************End Of File*****************************/
