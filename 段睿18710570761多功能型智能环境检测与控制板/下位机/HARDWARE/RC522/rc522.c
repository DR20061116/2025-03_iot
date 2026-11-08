#include "stm32f10x.h"
#include "RC522.h"
#include "usart.h"
#include "delay.h"
#include <string.h>

// 存储4张卡片的UID 初始值全部为FF(空位)
uint8_t Card_UID[4][4] = {
    {0xFF, 0xFF, 0xFF, 0xFF},  // 卡1
    {0xFF, 0xFF, 0xFF, 0xFF},  // 卡2  
    {0xFF, 0xFF, 0xFF, 0xFF},  // 卡3
    {0xFF, 0xFF, 0xFF, 0xFF}   // 卡4
};

uint8_t Current_UID[4];  // 当前读取到的UID
uint8_t Work_Mode = MODE_READ;  // 默认工作模式为读卡模式

// RC522初始化
void RC522_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 初始化RC522引脚
    RCC_APB2PeriphClockCmd(MF522_RST_CLK | MF522_MISO_CLK | MF522_MOSI_CLK | 
                          MF522_SCK_CLK | MF522_NSS_CLK, ENABLE);
    
    // RST引脚 推挽输出
    GPIO_InitStructure.GPIO_Pin = MF522_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MF522_RST_PORT, &GPIO_InitStructure);
    
    // MISO引脚 浮空输入
    GPIO_InitStructure.GPIO_Pin = MF522_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(MF522_MISO_PORT, &GPIO_InitStructure);
    
    // MOSI引脚 推挽输出
    GPIO_InitStructure.GPIO_Pin = MF522_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MF522_MOSI_PORT, &GPIO_InitStructure);
    
    // SCK引脚 推挽输出
    GPIO_InitStructure.GPIO_Pin = MF522_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MF522_SCK_PORT, &GPIO_InitStructure);
    
    // NSS引脚 推挽输出
    GPIO_InitStructure.GPIO_Pin = MF522_NSS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MF522_NSS_PORT, &GPIO_InitStructure);
    
    // 复位RC522
    RC522_Reset();
    
    UsartPrintf(USART1, "RC522 Initialize Success!\r\n");
}

// RC522复位
void RC522_Reset(void)
{
	//复位序列 高-低-高
    RST_H;
    delay_ms(1);
    RST_L;
    delay_ms(1);
    RST_H;
    delay_ms(10);
    
	//初始化RC522寄存器
    WriteRawRC(0x01, 0x0F); //软复位
    WriteRawRC(0x2A, 0x8D);	//定时器设置
    WriteRawRC(0x2B, 0x3E);	//定时器预分频
    WriteRawRC(0x2D, 30);	//定时器重载值低字节
    WriteRawRC(0x2C, 0);	//定时器重载值高字节
    WriteRawRC(0x15, 0x40);	//传输ASK设置
    WriteRawRC(0x11, 0x3D);	//模式设置
    
    RC522_AntennaOn();	//开启天线
}

// 开启天线
void RC522_AntennaOn(void)
{
    uint8_t temp;
    temp = ReadRawRC(0x14);	
    if (!(temp & 0x03))
    {
        SetBitMask(0x14, 0x03);
    }
}

// 寻卡请求函数
// req_code: 寻卡指令(0x52寻感应区内所有符合14443A标准的卡)
// pTagType: 返回卡片类型
char RC522_Request(uint8_t req_code, uint8_t *pTagType)
{
    char status;
    uint16_t unLen;
    uint8_t ucComMF522Buf[MAXRLEN];
    
    ClearBitMask(0x08, 0x08);
    WriteRawRC(0x0D, 0x07);	//允许中断请求
    SetBitMask(0x14, 0x03);	//开启天线
    
	//发送寻卡指令
    ucComMF522Buf[0] = req_code;
    status = RC522_ToCard(0x0C, ucComMF522Buf, 1, ucComMF522Buf, &unLen);
    //检查响应
    if ((status == MI_OK) && (unLen == 0x10))
    {
        *pTagType = ucComMF522Buf[0];
        *(pTagType+1) = ucComMF522Buf[1];
    }
    else
    {
        status = MI_ERR;
    }
    
    return status;
}

// 防冲撞函数 - 获取卡片序列号(UID)
// pSnr: 返回的4字节序列号
char RC522_Anticoll(uint8_t *pSnr)
{
    char status;
    uint8_t i, snr_check = 0;
    uint16_t unLen;
    uint8_t ucComMF522Buf[MAXRLEN];
    
    ClearBitMask(0x08, 0x08);
    WriteRawRC(0x0D, 0x00);		//禁用中断
    ClearBitMask(0x0E, 0x80);
    
    ucComMF522Buf[0] = 0x93;
    ucComMF522Buf[1] = 0x20;
    
    status = RC522_ToCard(0x0C, ucComMF522Buf, 2, ucComMF522Buf, &unLen);
    
    if (status == MI_OK)
    {
        for (i = 0; i < 4; i++)
        {
            *(pSnr + i) = ucComMF522Buf[i];
            snr_check ^= ucComMF522Buf[i];
        }
        if (snr_check != ucComMF522Buf[i])
        {
            status = MI_ERR;
        }
    }
    
    SetBitMask(0x0E, 0x80);	//恢复寄存器设置
    return status;
}

// RC522与卡片通信核心函数
// command: 命令字
// pInData: 输入数据缓冲区
// InLenByte: 输入数据长度
// pOutData: 输出数据缓冲区
// pOutLenBit: 输出数据位长度
char RC522_ToCard(uint8_t command, uint8_t *pInData, uint8_t InLenByte, 
                  uint8_t *pOutData, uint16_t *pOutLenBit)
{
    char status = MI_ERR;
    uint8_t irqEn = 0x00;	//中断使能
    uint8_t waitFor = 0x00;	//等待中断标志
    uint8_t lastBits;		//最后接收的位数
    uint8_t n;				//接收字节数
    uint16_t i;				//循环计数器
    
	//根据命令设置中断参数
    switch (command)
    {
        case 0x0E:	//认证命令
            irqEn = 0x12;
            waitFor = 0x10;
            break;
        case 0x0C:	//收发数据命令
            irqEn = 0x77;
            waitFor = 0x30;
            break;
        default:
            break;
    }
    
    WriteRawRC(0x02, irqEn | 0x80);	//设置中断使能
    ClearBitMask(0x04, 0x80);		//清除所有中断位
    WriteRawRC(0x01, 0x00);			//清除缓冲区
    SetBitMask(0x0A, 0x80);			//清空FIO
    
	//写入数据到FIO
    for (i = 0; i < InLenByte; i++)
    {
        WriteRawRC(0x09, pInData[i]);
    }
    
    WriteRawRC(0x01, command);
    //执行命令
    if (command == 0x0C)
    {
        SetBitMask(0x0D, 0x80);
    }
    //等待命令完成或者超时
    i = 2000;
    do
    {
        n = ReadRawRC(0x04);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor));
    
    ClearBitMask(0x0D, 0x80);
    
    if (i != 0)
    {
        if (!(ReadRawRC(0x06) & 0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;
            }
            if (command == 0x0C)
            {
                n = ReadRawRC(0x0A);
                lastBits = ReadRawRC(0x0C) & 0x07;
                if (lastBits)
                {
                    *pOutLenBit = (n - 1) * 8 + lastBits;
                }
                else
                {
                    *pOutLenBit = n * 8;
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAXRLEN)
                {
                    n = MAXRLEN;
                }
                for (i = 0; i < n; i++)
                {
                    pOutData[i] = ReadRawRC(0x09);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }
    }
    //清理寄存器
    SetBitMask(0x0C, 0x80);
    WriteRawRC(0x01, 0x00);
    return status;
}

// 检查卡片 读取模式下使用
uint8_t RC522_CheckCard(void)
{
    uint8_t TagType[2];
    uint8_t card_num;
    
    if (RC522_Request(0x52, TagType) == MI_OK)
    {
        if (RC522_Anticoll(Current_UID) == MI_OK)
        {
            // 检查是否为已录入的卡片
            card_num = 0;
            for (int i = 0; i < 4; i++)
            {
                if (memcmp(Current_UID, Card_UID[i], 4) == 0)
                {
                    card_num = i + 1;	//找到匹配的卡片
                    break;
                }
            }
            
            if (card_num > 0)
            {
                UsartPrintf(USART1, "Valid Card! Card Number: %d, UID: %02X %02X %02X %02X\r\n", 
                           card_num, Current_UID[0], Current_UID[1], Current_UID[2], Current_UID[3]);
                return card_num;
            }
            else
            {
                UsartPrintf(USART1, "Invalid Card! UID: %02X %02X %02X %02X\r\n", 
                           Current_UID[0], Current_UID[1], Current_UID[2], Current_UID[3]);
                return 0;	//未注册卡片
            }
        }
    }
    return 0xFF;  // 没有检测到卡片
}

// 添加卡片 在加卡模式下使用
uint8_t RC522_AddCard(void)
{
    uint8_t TagType[2];
    uint8_t empty_slot = 0xFF; //空槽位索引
    
    if (RC522_Request(0x52, TagType) == MI_OK)
    {
        if (RC522_Anticoll(Current_UID) == MI_OK)
        {
            // 检查卡片是否已存在
            for (int i = 0; i < 4; i++)
            {
                if (memcmp(Current_UID, Card_UID[i], 4) == 0)
                {
                    UsartPrintf(USART1, "Card Already Exists! Card Number: %d\r\n", i + 1);
                    return 0xFF;	//卡片已经存在
                }
            }
            
            // 寻找空位
            for (int i = 0; i < 4; i++)
            {
                if (Card_UID[i][0] == 0xFF && Card_UID[i][1] == 0xFF && 
                    Card_UID[i][2] == 0xFF && Card_UID[i][3] == 0xFF)
                {
                    empty_slot = i;
                    break;
                }
            }
            
            if (empty_slot != 0xFF)
            {
				//存储卡片UID
                memcpy(Card_UID[empty_slot], Current_UID, 4);
                UsartPrintf(USART1, "Add Card Success! Card Number: %d, UID: %02X %02X %02X %02X\r\n", 
                           empty_slot + 1, Current_UID[0], Current_UID[1], Current_UID[2], Current_UID[3]);
                return empty_slot + 1;	//返回卡片编号
            }
            else
            {
                UsartPrintf(USART1, "No Empty Slot! Cannot Add New Card.\r\n");
                return 0xFE;	//无空槽位
            }
        }
    }
    return 0xFF;  // 没有检测到卡片
}

// 删除卡片函数
// card_num: 要删除的卡片编号(1-4)
uint8_t RC522_DeleteCard(uint8_t card_num)
{
	//验证卡片编号有效性
    if (card_num < 1 || card_num > 4)
    {
        UsartPrintf(USART1, "Invalid Card Number! Please input 1-4.\r\n");
        return 0;
    }
    //检查卡槽位是否为空
    if (Card_UID[card_num - 1][0] == 0xFF && Card_UID[card_num - 1][1] == 0xFF && 
        Card_UID[card_num - 1][2] == 0xFF && Card_UID[card_num - 1][3] == 0xFF)
    {
        UsartPrintf(USART1, "Card Slot %d is Already Empty!\r\n", card_num);
        return 0;
    }
    //输出删除信息
    UsartPrintf(USART1, "Delete Card %d Success! UID: %02X %02X %02X %02X\r\n", 
               card_num, Card_UID[card_num - 1][0], Card_UID[card_num - 1][1], 
               Card_UID[card_num - 1][2], Card_UID[card_num - 1][3]);
    
    // 清空卡片数据
    Card_UID[card_num - 1][0] = 0xFF;
    Card_UID[card_num - 1][1] = 0xFF;
    Card_UID[card_num - 1][2] = 0xFF;
    Card_UID[card_num - 1][3] = 0xFF;
    
    return 1; //删除成功
}

// 读取所有卡片信息
void RC522_ReadAllCards(void)
{
    UsartPrintf(USART1, "=== Stored Cards Information ===\r\n");
    for (int i = 0; i < 4; i++)
    {
        if (Card_UID[i][0] != 0xFF || Card_UID[i][1] != 0xFF || 
            Card_UID[i][2] != 0xFF || Card_UID[i][3] != 0xFF)
        {
            UsartPrintf(USART1, "Card %d: %02X %02X %02X %02X\r\n", 
                       i + 1, Card_UID[i][0], Card_UID[i][1], Card_UID[i][2], Card_UID[i][3]);
        }
        else
        {
            UsartPrintf(USART1, "Card %d: Empty\r\n", i + 1);
        }
    }
    UsartPrintf(USART1, "================================\r\n");
}

// 设置工作模式
// mode: 工作模式(MODE_READ, MODE_ADD, MODE_DELETE)
void RC522_SetMode(uint8_t mode)
{
    Work_Mode = mode;
    switch (mode)
    {
        case MODE_READ:
            UsartPrintf(USART1, "Switch to READ Mode\r\n");
            break;
        case MODE_ADD:
            UsartPrintf(USART1, "Switch to ADD Card Mode\r\n");
            break;
        case MODE_DELETE:
            UsartPrintf(USART1, "Switch to DELETE Card Mode\r\n");
            break;
    }
}

// 底层SPI写寄存器函数
// Address: 寄存器地址
// value: 要写入的值
void WriteRawRC(uint8_t Address, uint8_t value)
{
    uint8_t i, ucAddr;
    
    SCK_L;	//时钟低电平
    NSS_L;	//片选低电平，开始传输
    ucAddr = ((Address << 1) & 0x7E);
    //发送地址字节（7位地址+读写位0）
    for (i = 8; i > 0; i--)
    {
        if (ucAddr & 0x80)
            MOSI_H;	//发送1
        else
            MOSI_L; //发送0
        SCK_H;		//时钟上升沿
        ucAddr <<= 1;
        SCK_L;		//时钟下降沿
    }
    //发送数据字节
    for (i = 8; i > 0; i--)
    {
        if (value & 0x80)
            MOSI_H;	//发送1
        else
            MOSI_L;	//发送0
        SCK_H;		//时钟上升沿
        value <<= 1;
        SCK_L;		//时钟下降沿
    }
    NSS_H;			//片选高电平，结束传输
    SCK_H;			//时钟高电平
}


// 底层SPI读寄存器函数
// Address: 寄存器地址
// 返回值: 读取的寄存器值
uint8_t ReadRawRC(uint8_t Address)
{
    uint8_t i, ucAddr;
    uint8_t ucResult = 0;
    
    NSS_L;
    ucAddr = ((Address << 1) & 0x7E) | 0x80;
    
    for (i = 8; i > 0; i--)
    {
        SCK_L;
        if (ucAddr & 0x80)
            MOSI_H;
        else
            MOSI_L;
        SCK_H;
        ucAddr <<= 1;
    }
    
    for (i = 8; i > 0; i--)
    {
        SCK_L;
        ucResult <<= 1;
        SCK_H;
        if (READ_MISO == 1)
            ucResult |= 1;
    }
    
    NSS_H;
    SCK_H;
    return ucResult;
}

// 设置寄存器位掩码
// reg: 寄存器地址
// mask: 位掩码
void SetBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp | mask);
}

// 清除寄存器位掩码
// reg: 寄存器地址
// mask: 位掩码
void ClearBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);
}
