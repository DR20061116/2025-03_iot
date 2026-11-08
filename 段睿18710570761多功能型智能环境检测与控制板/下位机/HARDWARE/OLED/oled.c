#include "oledfont.h"
#include "oled.h"
#include "delay.h"

//C库
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

u8 OLED_GRAM[128][8];	
/*引脚初始化*/
void OLED_I2C_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(IIC_SCK_CLK, ENABLE);			/* 打开GPIO时钟 */
	GPIO_InitStructure.GPIO_Pin = IIC_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	/* 开漏输出 */
	GPIO_Init(IIC_SCK_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(IIC_SDA_CLK, ENABLE);			/* 打开GPIO时钟 */
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	/* 开漏输出 */
	GPIO_Init(IIC_SCK_PORT, &GPIO_InitStructure);
	
	IIC_SCL=1;
	IIC_SDA=1;
}

/*
*********************************************************************************************************
*	函 数 名: IIC_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void IIC_Start(void)
{
	/* SDA设置为输出 */
	IIC_SDA;
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	I2C_SDA_1();
	I2C_SCK_1();
	delay_us(4);
	I2C_SDA_0();
	delay_us(4);
	I2C_SCK_0();
	delay_us(4);
}


/*
*********************************************************************************************************
*	函 数 名: IIC_Stop
*	功能说明: CPU发起I2C总线停止信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void IIC_Stop(void)
{
	/* SDA设置为输出 */
	IIC_SDA;
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	I2C_SDA_0();
	I2C_SCK_1();
	delay_us(4);
	I2C_SDA_1();						   	
}


/*
*********************************************************************************************************
*	函 数 名: IIC_Wait_Ack
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
u8 IIC_Wait_Ack(void)
{
	/* SDA设置为输入 */
	READ_SDA;
	I2C_SDA_1();	/* CPU释放SDA总线 */
	delay_us(4);
	I2C_SCK_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	delay_us(4);
	if (I2C_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		I2C_SCK_0();
		delay_us(4);
		return 1;
	}
	else
	{
		I2C_SCK_0();
		delay_us(4);
		return 0;
	}
} 


/*
*********************************************************************************************************
*	函 数 名: IIC_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void IIC_Ack(void)
{
	/* SDA设置为输出 */
	IIC_SDA;
	I2C_SDA_0();	/* CPU驱动SDA = 0 */
	delay_us(4);
	I2C_SCK_1();	/* CPU产生1个时钟 */
	delay_us(4);
	I2C_SCK_0();
	delay_us(4);
	I2C_SDA_1();	/* CPU释放SDA总线 */
}


/*
*********************************************************************************************************
*	函 数 名: IIC_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/	    
void IIC_NAck(void)
{
	/* SDA设置为输出 */
	IIC_SDA;
	I2C_SDA_1();	/* CPU驱动SDA = 1 */
	delay_us(4);
	I2C_SCK_1();	/* CPU产生1个时钟 */
	delay_us(4);
	I2C_SCK_0();
	delay_us(4);	
}					 				     


/*
*********************************************************************************************************
*	函 数 名: IIC_Send_Byte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参：txd ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void IIC_Send_Byte(u8 txd)
{
  u8 i;
	/* SDA设置为输出 */
	IIC_SDA;
	IIC_SCL=0;//拉低时钟开始数据传输
	for(i=0;i<8;i++)
	{              
		IIC_SDA=(txd&0x80)>>7;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
	}
}


/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参：ack ： ack=1时，发送ACK，ack=0，发送nACK
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	/* SDA设置为输入 */
	READ_SDA;
  for(i=0;i<8;i++ )
	{
		IIC_SCL=0; 
		delay_us(2);
		IIC_SCL=1;
		receive<<=1;
		if(READ_SDA)
			receive++;   
		delay_us(1); 
  }
	if (!ack)
		IIC_NAck();//发送nACK
	else
		IIC_Ack(); //发送ACK   
	return receive;
}

void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
			OLED_WriteCommand(0xB0+i);					//设置Y位置
		    OLED_WriteCommand(0x00);			//设置X位置低4位
			OLED_WriteCommand(0x10);	//设置X位置高4位
			
		OLED_WriteData(0x00);              //每页的列偏移两个像素点
		OLED_WriteData(0x00);
		for(n=0;n<128;n++)
		{
			OLED_WriteData(OLED_GRAM[n][i]); 
		}
	}   
}


/**********************************************
// IIC Write Command
**********************************************/
void OLED_WriteCommand(unsigned char IIC_Command)
{
  IIC_Start();
  IIC_Send_Byte(OLED_Address);					//OLED设备地址
	IIC_Wait_Ack();
  IIC_Send_Byte(OLED_Cmd_Address);			//写命令地址
	IIC_Wait_Ack();
  IIC_Send_Byte(IIC_Command);						//写入命令
	IIC_Wait_Ack();
  IIC_Stop();
}


/**********************************************
// IIC Write Data
**********************************************/
void OLED_WriteData(unsigned char IIC_Data)
{
  IIC_Start();
  IIC_Send_Byte(OLED_Address);					//OLED设备地址
	IIC_Wait_Ack();
  IIC_Send_Byte(OLED_Data_Address);			//写数据地址
	IIC_Wait_Ack();	
  IIC_Send_Byte(IIC_Data);							//写入数据
	IIC_Wait_Ack();
  IIC_Stop();
}


void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
	{
		OLED_WriteData(dat);
	}
	else
	{
   OLED_WriteCommand(dat);
	}
}

void OLED_Show_Image(const unsigned char *image)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
			OLED_WriteCommand(0xB0+(7-i));					//设置Y位置
		    OLED_WriteCommand(0x00);			//设置X位置低4位
			OLED_WriteCommand(0x10);	//设置X位置高4位
			
		OLED_WriteData(0x00);              //每页的列偏移两个像素点
		OLED_WriteData(0x00);
		for(n=0;n<128;n++)
		{
			OLED_WriteData(image[(n+(i*128))]); 
		}
	}   
}

//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空	      2-129			   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)
	{
		return;//超出范围了.
	}
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)
	{
		OLED_GRAM[x][pos]|=temp;
	}
	else OLED_GRAM[x][pos]&=~temp;	    
}

//在起始位置显示一根线
//x:0~127
//y:0~63
//width: 线宽度 横向0-128 纵向0-64				 
//len：线长度 横向0-128 纵向0-64
//mode：0： 横向画线 1：纵向画线
void OLED_Show_Line(u8 x,u8 y,u8 width ,u8 len,u8 mode)
{
	u8 width_temp,len_temp;
	if(mode==0)
	{
		for(width_temp=0;width_temp<width;width_temp++)
		{
			for(len_temp=0;len_temp<len;len_temp++)
			{
				OLED_DrawPoint(x+len_temp,y+width_temp,1);
			}
		}
   }
   else if(mode==1)
   {
		for(len_temp=0;len_temp<len;len_temp++)
		{
			for(width_temp=0;width_temp<width;width_temp++)
			{
				OLED_DrawPoint(x+width_temp,y+len_temp,1);
			}
		}
   }
}
//画一个指定宽度的矩形边框
//x:矩形x起点
//y:矩形y起点
//x_end:矩形x终点
//y_end:矩形y终点
//width:矩形边框宽度
void OLED_Show_Rectangle(u8 x,u8 y,u8 len,u8 width,u8 border_width)
{
	if(x+len>127 || y+width>63)
		return;
	OLED_Show_Line(x,y,border_width,len,0);
	OLED_Show_Line(x,y+width-border_width,border_width,len,0);
	OLED_Show_Line(x,y+border_width,border_width,width-2*border_width,1);
	OLED_Show_Line(x+len-border_width,y+border_width,border_width,width-2*border_width,1);
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 12/16/24/32
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	chr=chr-' ';//得到偏移后的值		 
    for(t=0;t<csize;t++)
    {   
		 
		
		if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
		else if(size==32)temp=asc2_3216[chr*64+t];	//调用3216字体
		else return;								//没有的字库
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	

//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//显示字符串
//x,y:起点坐标  
//size:字体大小 
//*p:字符串起始地址 
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}

 
void OLED_ShowPrintf(u8 x,u8 y,u8 size,char *fmt,...)
{	
  unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
	va_end(ap);
	
	while(*pStr != 0)
	{
		if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*pStr,size,1);	 
        x+=size/2;
        pStr++;
	}
}


void OLED_ShowPrintfed(u8 x,u8 y,u8 size,char *fmt,...)
{
	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
	va_end(ap);
	
	while(*pStr != 0)
	{
		if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*pStr,size,0);	 
        x+=size/2;
        pStr++;
	}
}


//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)
	{
		for(n=0;n<128;n++)
		{
			OLED_GRAM[n][i]=0X00;  
		}
	}
	//OLED_Refresh_Gram();//更新显示
}
//显示汉字
//x,y:起点坐标 
//num:字库中位数
//size:字体高度 
//mode:模式	0,填充模式;1,叠加模式
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=32;		//得到字体一个字符对应点阵集所占的字节数	 
    for(t=0;t<csize;t++)
    {   
		if(size==16)temp=Oled_Chinese[num][t];	//调用1608字体
		else return;								//没有的字库
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	OLED_I2C_Init();			//端口初始化
	IIC_Stop();
	delay_ms(800);
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0x20);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x10);
	
	OLED_WriteCommand(0xb0);	//设置多路复用率
	OLED_WriteCommand(0xc0);
	
	OLED_WriteCommand(0x00);	//设置显示偏移
	OLED_WriteCommand(0x10);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0x81);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xff);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xa1);	//设置COM引脚硬件配置
	OLED_WriteCommand(0xa6);
	
	OLED_WriteCommand(0xa8);	//设置对比度控制
	OLED_WriteCommand(0x3f);

	OLED_WriteCommand(0xa4);	//设置预充电周期
	OLED_WriteCommand(0xd3);

	OLED_WriteCommand(0x00);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0xd5);

	OLED_WriteCommand(0xf0);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xd9);	//设置正常/倒转显示

	OLED_WriteCommand(0x22);	//设置充电泵
	OLED_WriteCommand(0xda);

	OLED_WriteCommand(0x12);	//开启显示
    OLED_WriteCommand(0xdb);	//设置整个显示打开/关闭

	OLED_WriteCommand(0x20);	//设置正常/倒转显示

	OLED_WriteCommand(0x8d);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xaf);	//开启显示
}

