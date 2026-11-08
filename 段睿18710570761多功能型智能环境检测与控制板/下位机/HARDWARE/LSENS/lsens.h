#ifndef __LSENS_H
#define __LSENS_H	
#include "sys.h" 

#define LSENS_READ_TIMES	10		//定义光敏传感器读取次数,读这么多次,然后取平均值
#define LSENS_ADC_CHX		ADC_Channel_5	//定义光敏传感器所在的ADC通道编号
extern void Adc_Init(void);
extern u16 Get_Adc(u8 ch);
void Lsens_Init(void); 				//初始化光敏传感器
u16 Lsens_Get_Val(u8 i);				//读取光敏传感器的值
#endif 





















