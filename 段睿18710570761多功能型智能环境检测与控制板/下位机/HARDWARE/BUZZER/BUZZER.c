#include "stm32f10x.h"                  // Device header
#include "sys.h"
#include "delay.h"
#include "BUZZER.h"
void BUZZER_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;//声明结构体——GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//时钟使能GPIOB
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//频率50Mhz
	GPIO_Init(GPIOB,&GPIO_InitStructure);//初始化GPIOB
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);//高电平触发 默认关闭
}
void BUZZER_Open(void)
{
	int i;
	for(i=0;i<=3;i++)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_15);
		delay_ms(50);
		GPIO_ResetBits(GPIOB,GPIO_Pin_15);
		delay_ms(50);
		i++;
	}
}
void BUZZER_Close(void)
{
  GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}
