#ifndef _USART_H_
#define _USART_H_
#include "sys.h"

#include "stm32f10x.h"


#define USART_REC_LEN1  			200  	//定义最大接收字节数 200	  	
extern u8  USART_RX_BUF1[USART_REC_LEN1]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA1;         		//接收状态标记	

extern u8 USART_RX_BUF_FX[200];

#define USART_DEBUG USART1	


void Usart1_Init(unsigned int baud);
void Usart2_Init(unsigned int baud);
void Usart3_Init(unsigned int baud);
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);

// PM2.5相关声明
extern uint16_t PM25_Value;           // PM2.5数值
extern uint8_t PM25_RxFlag;           // 接收完成标志
uint8_t PM25_GetRxFlag(void);         // 获取接收标志
void PM25_ReadData(void);             // 读取PM2.5数据

#endif
