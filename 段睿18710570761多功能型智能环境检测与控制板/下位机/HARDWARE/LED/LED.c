#include "stm32f10x.h"                  // Device header
#include "LED.h"
void LED_Init(void)
{
	//声明结构体变量
	GPIO_InitTypeDef led_init;
	//打开控制GPIOA的时钟(APB2)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//配置结构体	
	led_init.GPIO_Pin   = GPIO_Pin_8;      
	led_init.GPIO_Mode  = GPIO_Mode_Out_PP; //推挽输出	
	led_init.GPIO_Speed = GPIO_Speed_50MHz; //10MHz
	
	//初始化
	GPIO_Init(GPIOC, &led_init);
	//默认熄灭
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}
void LED_Open(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
}
void LED_Close(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}
