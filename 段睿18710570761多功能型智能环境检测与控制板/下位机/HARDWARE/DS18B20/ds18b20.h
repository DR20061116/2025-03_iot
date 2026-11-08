#ifndef __DS18B20_H
#define __DS18B20_H 
#include "stm32f10x.h"
#include "sys.h"
 										   

//修改这里的接口
#define  DS18B20_GPIO_CLK           (RCC_APB2Periph_GPIOB)
#define  DS18B20_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  DS18B20_DQ_GPIO_PORT       GPIOB   
#define  DS18B20_DQ_GPIO_PIN        GPIO_Pin_7

/*IO口操作*/
#define DS18B20_DQ_OUT(x)	GPIO_WriteBit(DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN, (BitAction)(x))
#define DS18B20_DQ_IN       GPIO_ReadInputDataBit(DS18B20_DQ_GPIO_PORT,DS18B20_DQ_GPIO_PIN)

void DS18B20_IO_IN(void);
void DS18B20_IO_OUT(void);

   	
u8 DS18B20_Init(void);			
short DS18B20_Get_Temp(void);	
void DS18B20_Start(void);		
void DS18B20_Write_Byte(u8 dat);
u8 DS18B20_Read_Byte(void);		
u8 DS18B20_Read_Bit(void);		
u8 DS18B20_Check(void);			
void DS18B20_Rst(void);			   
#endif















