#ifndef __OLED_H
#define __OLED_H
#include "stm32f10x.h"


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

#define Max_Column					128  						  
#define OLED_Address				0x78
#define OLED_Cmd_Address		0x00
#define OLED_Data_Address		0x40

/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */

#define	IIC_SCK_PIN		GPIO_Pin_0						/* 连接到SCK时钟线的GPIO */
#define IIC_SCK_PORT	GPIOB									/* GPIO端口 */
#define IIC_SCK_CLK		RCC_APB2Periph_GPIOB	/* GPIO端口时钟 */
#define IIC_SCL    		PBout(0)							//SCL

#define	IIC_SDA_PIN		GPIO_Pin_1						/* 连接到SDA时钟线的GPIO */
#define IIC_SDA_PORT	GPIOB									/* GPIO端口 */
#define IIC_SDA_CLK		RCC_APB2Periph_GPIOB	/* GPIO端口时钟 */
#define IIC_SDA    		PBout(1)							//SDA
#define READ_SDA   		PBin(1)								//输入SDA


/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */
#if 0		/* 条件编译： 1 选择GPIO的库函数实现IO读写 */
	#define I2C_SCK_1()  GPIO_SetBits(IIC_SCK_PORT,IIC_SCK_PIN)			/* SCL = 1 */
	#define I2C_SCK_0()  GPIO_ResetBits(IIC_SCK_PORT,IIC_SCK_PIN)		/* SCL = 0 */
	
	#define I2C_SDA_1()  GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN)			/* SDA = 1 */
	#define I2C_SDA_0()  GPIO_ResetBits(IIC_SDA_PORT,IIC_SDA_PIN)		/* SDA = 0 */
	
	#define I2C_SDA_READ()  GPIO_ReadInputDataBit(GPIO_PORT_I2C,I2C_SDA_PIN)	/* 读SDA口线状态 */
#else	/* 这个分支选择直接寄存器操作实现IO读写 */
	/*　注意：如下写法，在IAR最高级别优化时，会被编译器错误优化 */
	#define I2C_SCK_1()  IIC_SCK_PORT->BSRR = IIC_SCK_PIN				/* SCL = 1 */
	#define I2C_SCK_0()  IIC_SCK_PORT->BRR = IIC_SCK_PIN				/* SCL = 0 */
	
	#define I2C_SDA_1()  IIC_SDA_PORT->BSRR = IIC_SDA_PIN				/* SDA = 1 */
	#define I2C_SDA_0()  IIC_SDA_PORT->BRR = IIC_SDA_PIN				/* SDA = 0 */
	
	#define I2C_SDA_READ()  ((IIC_SDA_PORT->IDR & IIC_SDA_PIN) != 0)	/* 读SDA口线状态 */
#endif

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);								//发送IIC开始信号
void IIC_Stop(void);	  						//发送IIC停止信号
void IIC_Ack(void);									//IIC发送ACK信号
void IIC_NAck(void);								//IIC不发送ACK信号
u8 IIC_Wait_Ack(void); 							//IIC等待ACK信号
void IIC_Send_Byte(u8 txd);					//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节

void OLED_Init(void);//OLED初始化
void OLED_Clear(void);//清屏
void OLED_WriteCommand(unsigned char IIC_Command);
void OLED_WriteData(unsigned char IIC_Data);
void OLED_WR_Byte(unsigned dat,unsigned cmd);
void OLED_Refresh_Gram(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);//画点
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);//oled显示字符
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size);//oled显示字符串
void OLED_ShowPrintf(u8 x,u8 y,u8 size,char *fmt,...);
void OLED_ShowPrintfed(u8 x,u8 y,u8 size,char *fmt,...);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);//oled显示数字
u32 mypow(u8 m,u8 n);//m^n函数
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size,u8 mode);
void OLED_Show_Image(const unsigned char *image);
void OLED_Show_Line(u8 x,u8 y,u8 width ,u8 len,u8 mode);
void OLED_Show_Rectangle(u8 x,u8 y,u8 len,u8 width,u8 border_width);
#endif

