#ifndef __RC522_H
#define __RC522_H

#include "stm32f10x.h"

#define MAXRLEN 18

// RC522引脚定义
#define MF522_RST_PIN     GPIO_Pin_11//RES 复位
#define MF522_RST_PORT    GPIOB
#define MF522_RST_CLK     RCC_APB2Periph_GPIOB

#define MF522_MISO_PIN    GPIO_Pin_10//MISO 主入从出引脚
#define MF522_MISO_PORT   GPIOB
#define MF522_MISO_CLK    RCC_APB2Periph_GPIOB

#define MF522_MOSI_PIN    GPIO_Pin_1//MOSI 从如主出引脚
#define MF522_MOSI_PORT   GPIOB
#define MF522_MOSI_CLK    RCC_APB2Periph_GPIOB

#define MF522_SCK_PIN     GPIO_Pin_0//SCK 时钟
#define MF522_SCK_PORT    GPIOB
#define MF522_SCK_CLK     RCC_APB2Periph_GPIOB

#define MF522_NSS_PIN     GPIO_Pin_7//SDA 片选
#define MF522_NSS_PORT    GPIOA
#define MF522_NSS_CLK     RCC_APB2Periph_GPIOA

// 引脚操作宏定义
#define RST_H     GPIO_SetBits(MF522_RST_PORT, MF522_RST_PIN) 			//复位引脚高电平
#define RST_L     GPIO_ResetBits(MF522_RST_PORT, MF522_RST_PIN) 		//复位引脚低电平
#define MOSI_H    GPIO_SetBits(MF522_MOSI_PORT, MF522_MOSI_PIN) 		//MOSI高电平
#define MOSI_L    GPIO_ResetBits(MF522_MOSI_PORT, MF522_MOSI_PIN)		//MOSI低电平
#define SCK_H     GPIO_SetBits(MF522_SCK_PORT, MF522_SCK_PIN)			//时钟高电平
#define SCK_L     GPIO_ResetBits(MF522_SCK_PORT, MF522_SCK_PIN)			//时钟低电平
#define NSS_H     GPIO_SetBits(MF522_NSS_PORT, MF522_NSS_PIN)			//片选高电平
#define NSS_L     GPIO_ResetBits(MF522_NSS_PORT, MF522_NSS_PIN)			//片选低电平
#define READ_MISO GPIO_ReadInputDataBit(MF522_MISO_PORT, MF522_MISO_PIN)//读取MISO引脚

// 模式定义
#define MODE_READ    0 //读卡模式
#define MODE_ADD     1 //加卡模式
#define MODE_DELETE  2 //删卡模式

// 错误代码
#define MI_OK          0 //操作成功
#define MI_NOTAGERR   -1 //无卡片错误
#define MI_ERR        -2 //其他错误

// 函数声明
void RC522_Init(void);																//RC522初始化
void RC522_Reset(void);																//RC522复位
void RC522_AntennaOn(void);															//开启天线
char RC522_Request(uint8_t req_code, uint8_t *pTagType);							//寻卡请求
char RC522_Anticoll(uint8_t *pSnr);													//防冲撞，获取卡片序列号
char RC522_Select(uint8_t *pSnr);													//选择卡片
char RC522_Auth(uint8_t auth_mode, uint8_t addr, uint8_t *pKey, uint8_t *pSnr);		//认证
char RC522_Read(uint8_t addr, uint8_t *pData);										//读块数据
char RC522_Write(uint8_t addr, uint8_t *pData);										//写块数据
char RC522_Halt(void);																//休眠卡片
char RC522_ToCard(uint8_t command, uint8_t *pInData, uint8_t InLenByte, 			//与卡片通信
                  uint8_t *pOutData, uint16_t *pOutLenBit);							
uint8_t RC522_CheckCard(void);														//检查卡片状态
uint8_t RC522_AddCard(void);														//添加卡片
uint8_t RC522_DeleteCard(uint8_t card_num);											//删除指定卡片
void RC522_ReadAllCards(void);														//读取所有已存储卡片
void RC522_SetMode(uint8_t mode);													//设置工作模式

// 底层寄存器操作函数
void WriteRawRC(uint8_t Address, uint8_t value);									//写寄存器
uint8_t ReadRawRC(uint8_t Address);													//读寄存器
void SetBitMask(uint8_t reg, uint8_t mask);											//设置寄存器掩码
void ClearBitMask(uint8_t reg, uint8_t mask);										//清除寄存器掩码

// 全局变量声明
extern uint8_t Work_Mode;		//当前工作模式
extern uint8_t Card_UID[4][4];	//存储4张卡的UID
extern uint8_t Current_UID[4]; 	//当前读取到的UID

#endif
