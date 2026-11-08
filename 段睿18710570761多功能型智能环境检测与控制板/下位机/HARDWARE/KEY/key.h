#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

// 按键引脚定义
#define KEY1_PIN  GPIO_Pin_12
#define KEY1_PORT GPIOB

#define KEY2_PIN  GPIO_Pin_13 
#define KEY2_PORT GPIOB

#define KEY3_PIN  GPIO_Pin_14
#define KEY3_PORT GPIOB

#define KEY4_PIN  GPIO_Pin_15
#define KEY4_PORT GPIOB

// 读取按键状态
#define KEY1  GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN)
#define KEY2  GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN)
#define KEY3  GPIO_ReadInputDataBit(KEY3_PORT, KEY3_PIN)
#define KEY4  GPIO_ReadInputDataBit(KEY4_PORT, KEY4_PIN)

// 按键按下返回值
#define KEY1_PRES 	1
#define KEY2_PRES 	2
#define KEY3_PRES 	3
#define KEY4_PRES 	4

// 函数声明
void KEY_Init(void);        // 按键初始化
u8 KEY_Scan(u8 mode);       // 按键扫描函数
#endif
