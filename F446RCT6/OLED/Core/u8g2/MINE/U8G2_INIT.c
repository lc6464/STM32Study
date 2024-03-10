#include "U8G2_INIT.h"


#define SEND_BUFFER_DISPLAY_MS(u8g2, ms)\
  do {\
    u8g2_SendBuffer(u8g2); \
    HAL_Delay(ms);\
  }while(0);

uint8_t u8g2_gpio_and_delay_stm32_hal(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {						// can be used to setup pins
    case U8X8_MSG_DELAY_MILLI:			// delay arg_int * 1 milli second
	  HAL_Delay(arg_int);
      break;
    case U8X8_MSG_GPIO_CS1:				// CS1 (chip select) pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_CS2:				// CS2 (chip select) pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_I2C_CLOCK:		// arg_int=0: Output low at I2C clock pin
      break;							// arg_int=1: Input dir with pullup high for I2C clock pin
    case U8X8_MSG_GPIO_I2C_DATA:			// arg_int=0: Output low at I2C data pin
      break;							// arg_int=1: Input dir with pullup high for I2C data pin
    case U8X8_MSG_GPIO_MENU_SELECT:
      u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
      break;
    case U8X8_MSG_GPIO_MENU_NEXT:
      u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
      break;
    case U8X8_MSG_GPIO_MENU_PREV:
      u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
      break;
    case U8X8_MSG_GPIO_MENU_HOME:
      u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
      break;
    default:
      u8x8_SetGPIOResult(u8x8, 1);			// default return value
      break;
  }
  return 1;
}

uint8_t u8x8_byte_stm32_hal_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  static uint8_t buffer[32];		/* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
  static uint8_t buf_idx;
  uint8_t *data;

  switch(msg)
  {
    case U8X8_MSG_BYTE_SEND:
      data = (uint8_t *)arg_ptr;
      while( arg_int > 0 )
      {
	        buffer[buf_idx++] = *data;
	        data++;
	        arg_int--;
      }
      break;
    case U8X8_MSG_BYTE_INIT:
      /* add your custom code to init i2c subsystem */
      break;
    case U8X8_MSG_BYTE_SET_DC:
      /* ignored for i2c */
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
      buf_idx = 0;
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:
      HAL_I2C_Master_Transmit(&hi2c2,u8x8_GetI2CAddress(u8x8), buffer, buf_idx,1000);
      break;
    default:
      return 0;
  }
  return 1;
}

//进度条显示
void testDrawProcess(u8g2_t *u8g2)
{
    for(int i=10;i<=80;i=i+2)
    {
        u8g2_ClearBuffer(u8g2);

        char buff[20];
        sprintf(buff,"%d%%",(int)(i/80.0*100));

        u8g2_SetFont(u8g2,u8g2_font_ncenB12_tf);
        u8g2_DrawStr(u8g2,16,32,"STM32 U8g2");//字符显示

        u8g2_SetFont(u8g2,u8g2_font_ncenB08_tf);
        u8g2_DrawStr(u8g2,100,49,buff);//当前进度显示

        u8g2_DrawRBox(u8g2,16,40,i,10,4);//圆角填充框矩形框
        u8g2_DrawRFrame(u8g2,16,40,80,10,4);//圆角矩形

        u8g2_SendBuffer(u8g2);
    }
    HAL_Delay(500);
}

//字体测试 数字英文可选用 u8g2_font_ncenB..（粗）系列字体
//u8g2_font_unifont_t_symbols/u8g2_font_unifont_h_symbols(ϸ Բ��)
void testShowFont(u8g2_t *u8g2)
{
    int t = 1000;
    char testStr[14] = "STM32F446RCT6";

    u8g2_ClearBuffer(u8g2);

    u8g2_SetFont(u8g2,u8g2_font_u8glib_4_tf);
    u8g2_DrawStr(u8g2,0,5,testStr);
    SEND_BUFFER_DISPLAY_MS(u8g2,t);

    u8g2_SetFont(u8g2,u8g2_font_ncenB08_tf);
    u8g2_DrawStr(u8g2,0,30,testStr);
    SEND_BUFFER_DISPLAY_MS(u8g2,t);

    u8g2_SetFont(u8g2,u8g2_font_ncenB10_tr);
    u8g2_DrawStr(u8g2,0,60,testStr);
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
}


//画空心矩形
void testDrawFrame(u8g2_t *u8g2)
{
    int t = 1000;
    int x = 16;
    int y = 32;
    int w = 50;
    int h = 20;
    u8g2_ClearBuffer(u8g2);
    u8g2_DrawStr(u8g2,0, 15, "DrawFrame");

    u8g2_DrawFrame(u8g2, x, y, w, h);
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
    u8g2_DrawFrame(u8g2, x+w+5, y-10, w-20, h+20);
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
}

//画实心圆角矩形
void testDrawRBox(u8g2_t *u8g2)
{
    int t = 1000;
    int x = 16;
    int y = 32;
    int w = 50;
    int h = 20;
    int r = 3;
    u8g2_ClearBuffer(u8g2);
    u8g2_DrawStr(u8g2,0, 15, "DrawRBox");

    u8g2_DrawRBox(u8g2, x, y, w, h, r);
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
    u8g2_DrawRBox(u8g2, x+w+5, y-10, w-20, h+20, r);
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
}

//画空心圆
void testDrawCircle(u8g2_t *u8g2)
{
    int t = 600;
    int stx = 0;  //画图起始x
    int sty = 16; //画图起始y
    int with = 16;//一块图形的间隔
    int r = 15;   //圆的半径
    u8g2_ClearBuffer(u8g2);
    u8g2_DrawStr(u8g2, 0, 15, "DrawCircle");

    u8g2_DrawCircle(u8g2, stx, sty - 1 + with, r, U8G2_DRAW_UPPER_RIGHT); //����
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
    u8g2_DrawCircle(u8g2, stx + with, sty, r, U8G2_DRAW_LOWER_RIGHT); //����
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
    u8g2_DrawCircle(u8g2, stx - 1 + with * 3, sty - 1 + with, r, U8G2_DRAW_UPPER_LEFT); //����
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
    u8g2_DrawCircle(u8g2, stx - 1 + with * 4, sty, r, U8G2_DRAW_LOWER_LEFT); //����
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
    u8g2_DrawCircle(u8g2, stx - 1 + with * 2, sty - 1 + with * 2, r, U8G2_DRAW_ALL);//����Բ
    SEND_BUFFER_DISPLAY_MS(u8g2,t);

    u8g2_DrawCircle(u8g2, 32*3, 32, 31, U8G2_DRAW_ALL);//�Ҳ�����Բ
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
}

//画实心椭圆
void testDrawFilledEllipse(u8g2_t *u8g2)
{
    int t = 800;
    int with = 16;//一个图块的间隔
    int rx = 27;  //椭圆x方向的半径
    int ry = 22;  //椭圆y方向的半径
    u8g2_ClearBuffer(u8g2);
    u8g2_DrawStr(u8g2,0, 14, "DrawFilledEllipse");

    SEND_BUFFER_DISPLAY_MS(u8g2,t);
    u8g2_DrawFilledEllipse(u8g2, 0, with, rx, ry, U8G2_DRAW_LOWER_RIGHT);//����
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
    u8g2_DrawFilledEllipse(u8g2, with * 4 - 1, with, rx, ry, U8G2_DRAW_LOWER_LEFT); //����
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
    u8g2_DrawFilledEllipse(u8g2, 0, with * 4 - 1, rx, ry, U8G2_DRAW_UPPER_RIGHT); //����
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
    u8g2_DrawFilledEllipse(u8g2, with * 4 - 1, with * 4 - 1, rx, ry, U8G2_DRAW_UPPER_LEFT); //����
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
    u8g2_DrawFilledEllipse(u8g2, with * 6, with * 2.5, rx, ry, U8G2_DRAW_ALL);//������Բ
    SEND_BUFFER_DISPLAY_MS(u8g2,t);
}

//环形测试
void testDrawMulti(u8g2_t *u8g2)
{
    u8g2_ClearBuffer(u8g2);
    for (int j = 0; j < 64; j+=16)
    {
        for (int i = 0; i < 128; i+=16)
        {
            u8g2_DrawPixel(u8g2, i, j);
            u8g2_SendBuffer(u8g2);
        }
    }

    //实心矩形逐渐变大
    u8g2_ClearBuffer(u8g2);
    for(int i=30; i>0; i-=2)
    {
        u8g2_DrawBox(u8g2,i*2,i,128-i*4,64-2*i);
        u8g2_SendBuffer(u8g2);
    }
    //空心矩形逐渐变小
    u8g2_ClearBuffer(u8g2);
    for(int i=0; i<32; i+=2)
    {
        u8g2_DrawFrame(u8g2,i*2,i,128-i*4,64-2*i);
        u8g2_SendBuffer(u8g2);
    }

    //实心圆角矩形逐渐变大
    u8g2_ClearBuffer(u8g2);
    for(int i=30; i>0; i-=2)
    {
        u8g2_DrawRBox(u8g2,i*2,i,128-i*4,64-2*i,10-i/3);
        u8g2_SendBuffer(u8g2);
    }
    //空心圆角矩形逐渐变小
    u8g2_ClearBuffer(u8g2);
    for(int i=0; i<32; i+=2)
    {
        u8g2_DrawRFrame(u8g2,i*2,i,128-i*4,64-2*i,10-i/3);
        u8g2_SendBuffer(u8g2);
    }

    //实心圆逐渐变大
    u8g2_ClearBuffer(u8g2);
    for(int i=2; i<64; i+=3)
    {
        u8g2_DrawDisc(u8g2,64,32,i, U8G2_DRAW_ALL);
        u8g2_SendBuffer(u8g2);
    }
    //空心圆逐渐变小
    u8g2_ClearBuffer(u8g2);
    for(int i=64; i>0; i-=3)
    {
        u8g2_DrawCircle(u8g2,64,32,i, U8G2_DRAW_ALL);
        u8g2_SendBuffer(u8g2);
    }

    //实心椭圆逐渐变大
    u8g2_ClearBuffer(u8g2);
    for(int i=2; i<32; i+=3)
    {
        u8g2_DrawFilledEllipse(u8g2,64,32, i*2, i, U8G2_DRAW_ALL);
        u8g2_SendBuffer(u8g2);
    }
    //空心椭圆逐渐变小
    u8g2_ClearBuffer(u8g2);
    for(int i=32; i>0; i-=3)
    {
        u8g2_DrawEllipse(u8g2,64,32, i*2, i, U8G2_DRAW_ALL);
        u8g2_SendBuffer(u8g2);
    }
}
//u8g2初始化

void U8G2_Init(u8g2_t *u8g2)
{
  u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_stm32_hal_hw_i2c, u8g2_gpio_and_delay_stm32_hal);
  u8g2_InitDisplay(u8g2);     // 发送init序列到显示器，显示器在此之后处于睡眠模式
  u8g2_SetPowerSave(u8g2, 0); // 唤醒显示器
  u8g2_ClearDisplay(u8g2);    // 清除屏幕缓冲区
  u8g2_ClearBuffer(u8g2);      // 清除u8g2显示缓冲区
  u8g2_SetFont(u8g2, u8g2_font_wqy16_t_gb2312a);        // 设置字库为u8g2_font_wqy12_t_gb2312a(字库全，但是110KB..12为字体大小)
}


