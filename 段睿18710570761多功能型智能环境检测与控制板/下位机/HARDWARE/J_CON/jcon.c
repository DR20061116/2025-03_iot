#include "jcon.h"
#include "delay.h"								    
//初始化
void BEEP_Init(void)  //接口初始化
{ 
	GPIO_InitTypeDef  GPIO_InitStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //APB2外设GPIO时钟使能  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	BEEP=0; 
}




