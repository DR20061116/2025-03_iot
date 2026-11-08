#include "stm32f10x.h"                  // Device header
#include "pwm.h"

/*
	1.打开时钟
	2.配置时基单元
	3.配置输出比较单元
	4.配置GPIO
	5.启动运行控制

*/
void Pwm_Init()
{
	/*	1:		*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	/*	2:		*/
	TIM_InternalClockConfig(TIM2);
	
	/*4:   TIM2引脚的GPIO配置   */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				//对定时器GPIO设置为复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;							//PA1 PA0引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM2);
	
 
	/*	3:	20KHZ,50%占空比	*/
	TIM_TimeBaseInitTypeDef TIM2_InitStructuer;
	TIM2_InitStructuer.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM2_InitStructuer.TIM_CounterMode = TIM_CounterMode_Up ;
	TIM2_InitStructuer.TIM_Period = 100-1;   //ARR
	TIM2_InitStructuer.TIM_Prescaler = 36 - 1;	//PSC
	TIM2_InitStructuer.TIM_RepetitionCounter = 0;  //重复计数器高级计时器用
	TIM_TimeBaseInit(TIM2,&TIM2_InitStructuer);
	//设置的PWM频率为 72M/((ARR+1)*(PSC+1))=72M/(100*36)=20KHZ
	TIM_OCInitTypeDef TIM2_OCInitStructure;
	//TIM_OCStructInit(&TIM2_OCInitStructure); //赋默认值，方便修改需要的值
	TIM2_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM1模式
	TIM2_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //高电平有效值
	TIM2_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//使能
	TIM2_OCInitStructure.TIM_Pulse = 0; //CCR
	TIM_OC1Init(TIM2,&TIM2_OCInitStructure);
	TIM_OC2Init(TIM2,&TIM2_OCInitStructure);
	
	/*	5:		*/
	TIM_Cmd(TIM2,ENABLE);

} 

void car_io_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能PORTB时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;//小车
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //
 	GPIO_Init(GPIOC, &GPIO_InitStructure);

}


void Car_Left_Tire(uint8_t Compare,uint8_t mode) //小车左电机PWM  mode:1正转,0反转
{
	TIM_SetCompare1(TIM2, Compare);
	if(mode == 1)
	{
		BN1 = 0;
		BN2 = 1;
	}	
	else if (mode ==0)
	{
		BN1 = 1;
		BN2 = 0;
	}
	
}
void Car_Right_Tire(uint8_t Compare,uint8_t mode)//小车右电机PWM
{
	TIM_SetCompare2(TIM2, Compare);
	if(mode == 1)
	{
		AN1 = 1;
		AN2 = 0;
	}	
	else if(mode ==0)
	{
		AN1 = 0;
		AN2 = 1;
	}
}
