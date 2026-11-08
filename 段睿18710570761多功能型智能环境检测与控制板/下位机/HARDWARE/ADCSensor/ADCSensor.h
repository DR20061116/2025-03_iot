#ifndef __ADCSENSOR_H
#define __ADCSENSOR_H
#include "sys.h" 
#include "stm32f10x.h"

#define SENSOR_READ_TIMES    10        // ADC读取次数，取平均值

// ADC通道定义 ★仅修改注释：甲醛→MQ5，宏名完全不变
#define ADC_LIGHT_CHANNEL    1    // PA1 - 光敏传感器
#define ADC_MQ4_CHANNEL      4    // PA4 - MQ4传感器
#define ADC_HCHO_CHANNEL     0    // PA0 - MQ5传感器（原甲醛通道）

extern void Adc_Init(void);
extern u16 Get_Adc(u8 ch);

void Sensor_Init(void);                     // 初始化ADC传感器
u16 LightSensor_GetValue(void);             // 读取光敏传感器值（百分比0-100）
u16 MQ4Sensor_GetValue(void);               // 读取MQ4传感器值（百分比0-100）
u16 HCHOSensor_GetPPM(void);                // 读取MQ5传感器值（百分比0-100）★仅修改注释：甲醛→MQ5

#endif

