#include "INFRARED.h"

void INFRARED_Init(void)
{ 
	//初始化结构体
  GPIO_InitTypeDef GPIO_InitStructure;   
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
  //INFRARED  IO 初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;       	//发送高点平引脚
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//初始化GPIOA
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

