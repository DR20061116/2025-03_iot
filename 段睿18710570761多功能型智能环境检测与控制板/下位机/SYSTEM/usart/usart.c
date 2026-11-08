//硬件驱动
#include "usart.h"
#include "delay.h"

//C库
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

/*
************************************************************
*	函数名称：	Usart1_Init
*
*	函数功能：	串口1初始化
*
*	入口参数：	baud：设定的波特率
*
*	返回参数：	无
*
*	说明：		TX-PA9		RX-PA10
************************************************************
*/
void Usart1_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpio_initstruct;
	USART_InitTypeDef usart_initstruct;
	NVIC_InitTypeDef nvic_initstruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//PA9	TXD
	gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_9;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio_initstruct);
	
	//PA10	RXD
	gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_10;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio_initstruct);
	
	usart_initstruct.USART_BaudRate = baud;
	usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	usart_initstruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;						//接收和发送
	usart_initstruct.USART_Parity = USART_Parity_No;									//无校验
	usart_initstruct.USART_StopBits = USART_StopBits_1;								//1位停止位
	usart_initstruct.USART_WordLength = USART_WordLength_8b;							//8位数据位
	USART_Init(USART1, &usart_initstruct);
	
	USART_Cmd(USART1, ENABLE);														//使能串口
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									//使能接收中断
	
	nvic_initstruct.NVIC_IRQChannel = USART1_IRQn;
	nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
	nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 3;
	nvic_initstruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&nvic_initstruct);

}

void Usart2_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpio_initstruct;
	USART_InitTypeDef usart_initstruct;
	NVIC_InitTypeDef nvic_initstruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//PA2	TXD
	gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_2;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio_initstruct);
	
	//PA3	RXD
	gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_3;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio_initstruct);
	
	usart_initstruct.USART_BaudRate = baud;
	usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//接收和发送
	usart_initstruct.USART_Parity = USART_Parity_No;									//无校验
	usart_initstruct.USART_StopBits = USART_StopBits_1;								//1位停止位
	usart_initstruct.USART_WordLength = USART_WordLength_8b;							//8位数据位
	USART_Init(USART2, &usart_initstruct);
	
	USART_Cmd(USART2, ENABLE);														//使能串口
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//使能接收中断
	
	nvic_initstruct.NVIC_IRQChannel = USART2_IRQn;
	nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
	nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 3;
	nvic_initstruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&nvic_initstruct);

}



void Usart3_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpio_initstruct;
	USART_InitTypeDef usart_initstruct;
	NVIC_InitTypeDef nvic_initstruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	//PB10	TXD
	gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_10;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_initstruct);
	
	//PB11	RXD
	gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_11;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_initstruct);
	
	usart_initstruct.USART_BaudRate = baud;
	usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//接收和发送
	usart_initstruct.USART_Parity = USART_Parity_No;									//无校验
	usart_initstruct.USART_StopBits = USART_StopBits_1;								//1位停止位
	usart_initstruct.USART_WordLength = USART_WordLength_8b;							//8位数据位
	USART_Init(USART3, &usart_initstruct);
	
	USART_Cmd(USART3, ENABLE);														//使能串口
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);									//使能接收中断
	
	nvic_initstruct.NVIC_IRQChannel = USART3_IRQn;
	nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
	nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 3;
	nvic_initstruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&nvic_initstruct);

}
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//发送数据
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);		//等待发送完成
	}

}

/*
************************************************************
*	函数名称：	UsartPrintf
*
*	函数功能：	格式化打印
*
*	入口参数：	USARTx：串口组
*				fmt：不定长参
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{

	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
	va_end(ap);
	
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
	}

}


// 变量名仍保留PM25前缀，但实际存储CO?数据
uint16_t PM25_Value = 0;
uint8_t PM25_RxFlag = 0;
uint8_t PM25_RxPacket[6];             // 改为6字节（适配CO?传感器数据帧）

/*
************************************************************
*	函数名称：	PM25_GetRxFlag
*
*	函数功能：	获取接收标志（实际是CO?接收完成标志）
*
*	入口参数：	无
*
*	返回参数：	接收标志
*
*	说明：		读取后自动清除标志，函数名不变
************************************************************
*/
uint8_t PM25_GetRxFlag(void)
{
	if (PM25_RxFlag == 1)
	{
		PM25_RxFlag = 0;
		return 1;
	}
	return 0;
}

/*
************************************************************
*	函数名称：	PM25_ReadData
*
*	函数功能：	读取数据（实际是CO?浓度数据）
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		将接收到的6字节数据转换为CO?数值，函数名不变
************************************************************
*/
void PM25_ReadData(void)
{
    if(PM25_GetRxFlag() == 1)
    {
        // CO?数据解析：第1、2字节合并为浓度值（替换原PM2.5的2字节解析）
        PM25_Value = PM25_RxPacket[1] * 256 + PM25_RxPacket[2];
    }
}

/*
************************************************************
*	函数名称：	USART2_IRQHandler
*	函数功能：	串口2中断服务函数 - 用于接收CO?数据（PA2/PA3）
*	入口参数：	无
*	返回参数：	无
*	说明：		极简修改，仅修复状态机逻辑，匹配6字节CO?协议
************************************************************
*/
void USART2_IRQHandler(void)
{
    uint8_t Res;
    static uint8_t RxState = 0;		// 状态机状态
    static uint8_t pRxPacket = 0;	// 接收数据索引

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  // 接收中断触发
    {
        Res = USART_ReceiveData(USART2);	// 读取串口2数据（PA3）	
        
        // 极简状态机：仅保留必要逻辑，匹配6字节CO2协议
        if(RxState == 0)
        {
            // 检测帧头0x2C，匹配则开始接收
            if(Res == 0x2C)
            {
                PM25_RxPacket[pRxPacket++] = Res;
                RxState = 1;
            }
        }
        else if(RxState == 1)
        {
            // 接收剩余5字节，凑齐6字节
            PM25_RxPacket[pRxPacket++] = Res;
            if(pRxPacket >= 6)
            {
                // 直接计算校验和，无需额外状态
                uint8_t CheckSum = PM25_RxPacket[0] + PM25_RxPacket[1] + PM25_RxPacket[2] + PM25_RxPacket[3] + PM25_RxPacket[4];
                if(PM25_RxPacket[5] == CheckSum)
                {
                    PM25_RxFlag = 1;	// 校验通过，置位完成标志
                }
                // 重置，准备下一次接收
                pRxPacket = 0;
                RxState = 0;
            }
        }
        
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);	// 清除中断标志
    } 
}

u8 USART_RX_BUF_FX[200];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
u16 USART_RX_STA_FX=0;       //接收状态标记	

// 串口3中断逻辑不变（仍用于风速数据）
void USART3_IRQHandler(void)
{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收中断
	{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
	
		USART_RX_BUF_FX[USART_RX_STA_FX]=Res;   //存储风速数据
		
		USART_RX_STA_FX++;
		if(USART_RX_STA_FX==9)
			USART_RX_STA_FX=0;
		
		if(USART_RX_STA_FX>=200)            //如果接收数据数组索引大于等于200，防止溢出就标记为0
			USART_RX_STA_FX=0;
	}
}
