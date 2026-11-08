#include "ds18b20.h"
#include "delay.h"	

void DS18B20_IO_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

 	DS18B20_GPIO_APBxClkCmd(DS18B20_GPIO_CLK,ENABLE);//ʹ��PORTA,PORTCʱ��
	
	GPIO_InitStructure.GPIO_Pin  = DS18B20_DQ_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //��������
 	GPIO_Init(DS18B20_DQ_GPIO_PORT, &GPIO_InitStructure);//��ʼ��
}

void DS18B20_IO_OUT(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 DS18B20_GPIO_APBxClkCmd(DS18B20_GPIO_CLK, ENABLE);	 //ʹ��
	
 GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ΆάˤԶ
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IOࠚ̙׈Ϊ50MHz
 GPIO_Init(DS18B20_DQ_GPIO_PORT, &GPIO_InitStructure);					 //ٹߝʨ֨ӎ˽Եʼۯ
}
//ԵʼۯDS18B20քIOࠚ DQ ͬʱݬӢDSքզ՚
//׵ܘ1:һզ՚
//׵ܘ0:զ՚    	 
u8 DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	DS18B20_GPIO_APBxClkCmd(DS18B20_GPIO_CLK, ENABLE);	 //ʹŜPORTAࠚʱד 
	
 	GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;				//PORTA0 ΆάˤԶ
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(DS18B20_DQ_GPIO_PORT, &GPIO_InitStructure);

 	GPIO_SetBits(DS18B20_DQ_GPIO_PORT,DS18B20_DQ_GPIO_PIN);    //ˤԶ1

	DS18B20_Rst();

	return DS18B20_Check();
}  


//شλDS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); //SET PA0 OUTPUT
    DS18B20_DQ_OUT(0); //-֍DQ
    delay_us(750);    //-֍750us
    DS18B20_DQ_OUT(1); //DQ=1 
	delay_us(15);     //15US
}
//ֈսDS18B20քܘӦ
//׵ܘ1:δݬӢսDS18B20քզ՚
//׵ܘ0:զ՚
u8 DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();//SET PA0 INPUT	 
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}
//ՓDS18B20ׁȡһٶλ
//׵ܘֵú1/0
u8 DS18B20_Read_Bit(void) 			 // read one bit
{
    u8 data;
	DS18B20_IO_OUT();//SET PA0 OUTPUT
    DS18B20_DQ_OUT(0); 
	delay_us(2);
    DS18B20_DQ_OUT(1); 
	DS18B20_IO_IN();//SET PA0 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
    else data=0;	 
    delay_us(50);           
    return data;
}
//ՓDS18B20ׁȡһٶؖޚ
//׵ܘֵúׁսք˽ߝ
u8 DS18B20_Read_Byte(void)    // read one byte
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
//дһٶؖޚսDS18B20
//datúҪдɫքؖޚ
void DS18B20_Write_Byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
	DS18B20_IO_OUT();//SET PA0 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT(0);// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT(1);
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT(0);// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT(1);
            delay_us(2);                          
        }
    }
}
//ߪʼς׈תۻ
void DS18B20_Start(void)// ds1820 start convert
{   						               
    DS18B20_Rst();	   
	DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 

//Փds18b20փսς׈ֵ
//ޫ׈ú0.1C
//׵ܘֵúς׈ֵ è-550~1250é 
short DS18B20_Get_Temp(void)
{
    u8 temp;
    u8 TL,TH;
	short tem;
    DS18B20_Start ();                    // ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// convert	    
    TL=DS18B20_Read_Byte(); // LSB   
    TH=DS18B20_Read_Byte(); // MSB  
	    	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//ς׈Ϊغ  
    }else temp=1;//ς׈Ϊֽ	  	  
    tem=TH; //ܱփٟыλ
    tem<<=8;    
    tem+=TL;//ܱփ֗ыλ
    tem=(float)tem*0.625;//תۻ
	if(tem>500)
		tem=0;
	if(temp)return tem; //׵ܘς׈ֵ
	else return -tem;    
} 
 
