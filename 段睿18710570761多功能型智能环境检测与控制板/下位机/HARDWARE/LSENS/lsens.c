#include "lsens.h"
#include "delay.h"

  
//初始化光敏传感器
void Lsens_Init(void)
{
     GPIO_InitTypeDef GPIO_InitStructure;
	
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA时钟	
	
	 GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1 | GPIO_Pin_0 | GPIO_Pin_2;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	 GPIO_Init(GPIOA, &GPIO_InitStructure);	
	 Adc_Init();
}
//读取Light Sens的值
//0~100:0,最暗;100,最亮 
u16 Lsens_Get_Val(u8 i)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<LSENS_READ_TIMES;t++)
	{
		temp_val+=Get_Adc(i);	//读取ADC值
		delay_ms(5);
	}
	temp_val/=LSENS_READ_TIMES;//得到平均值 
	return (u16)(temp_val);
}













