////网络协议层头文件
#include "onenet.h"                

////网络设备头文件
#include "esp8266.h"

////外围模块头文件
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "key.h"
#include "jcon.h"
#include "oled.h"
#include "dht11.h"
#include "adc.h"
#include "adcsensor.h"
//C库
#include <string.h>
#include <stdio.h>
#define LED_DURATION 1000 //定义计数次数
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"    //云平台访问API接口

u8 temp,hum;                  //t1 h1
u8 temp_H=100,temp_L=0,hum_H=100,hum_L=0;	//t2 t3 h2 h3
u16 pm25;											//p1
u16 pm25_H=1999,pm25_L=0;						//p2 p3
u16 light;										//l1
u16 light_H=100,light_L=0;					//l2 l3
u16 co2;											//c1
u16 co2_H=100,co2_L=0;							//c2 c3
u16 tvoc;											//o1
u16 tvoc_H=100,tvoc_L=0;						//o2 o3

int main(void)
{	
	unsigned short timeCount = 0;	//发送间隔变量
	unsigned char *dataPtr = NULL;
	
	delay_init();	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	   //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	OLED_Init();		//OLED屏幕初始化
	OLED_Clear();		//OLED清屏
	DHT11_Init();		//温湿度传感器初始化
	Sensor_Init();	//光敏传感器 mq4 甲醛传感器初始化
	BEEP_Init();		//蜂鸣器初始化
	Usart2_Init(9600);	//pm25传感器串口初始化
	
OLED_Clear();
OLED_ShowPrintf(0,0,16,"WIFI Connecting");  // 保留原提示文字
OLED_Refresh_Gram();

Usart1_Init(115200);       //WIFI通信串口初始化
ESP8266_Init();            //连接WIFI

// WIFI连接阶段：打点仅在下方，拉长加载延时（从500ms→1000ms）
u8 load_dot = 0;
while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))    
{
    load_dot = (load_dot + 1) % 4;
    OLED_ShowPrintf(0, 16, 16, "%.*s", load_dot, "...");  // 打点仅显示在下方（0,16位置）
    OLED_Refresh_Gram();
    delay_ms(1000);  // 拉长WIFI连接阶段的加载显示时间（1秒/次）
}

// OneNET接入阶段：保留原提示，打点仅在下方，拉长延时
OLED_Clear();
OLED_ShowPrintf(0,0,16,"OneNET Linking");
OLED_Refresh_Gram();
load_dot = 3;
while(OneNet_DevLink())                         
{
    ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT");
    load_dot = (load_dot + 1) % 4;
    OLED_ShowPrintf(0, 16, 16, "%.*s", load_dot, "...");  // 打点仅显示在下方
    OLED_Refresh_Gram();
    delay_ms(1000);  // 拉长OneNET连接阶段的加载显示时间（1秒/次）
}
OneNET_Subscribe();        //订阅云端 接收数据

// 连接成功提示：拉长停留时间（从800ms→3000ms），仅保留原提示布局
OLED_Clear();
OLED_ShowPrintf(0,0,16,"WIFI OK");
OLED_ShowPrintf(0,16,16,"OneNET OK");       
OLED_ShowPrintf(0,32,16,"System Ready");    
OLED_Refresh_Gram();
delay_ms(3000);  // 成功提示停留3秒，让用户有足够时间看清
	
	
	while(1)
	{
		DHT11_Read_Data(&temp,&hum);		//读取温湿度
		PM25_ReadData();								//读取PM25
		pm25=PM25_Value;								//赋值PM25
		light=LightSensor_GetValue();		//读取光照强度
		co2=MQ4Sensor_GetValue();				//读取二氧化碳
		tvoc=HCHOSensor_GetPPM();				//读取甲醛
		
		//OLED显示
		OLED_Clear();
// 第一行
OLED_ShowString(0, 0, (u8*)"TEM:", 16);
OLED_ShowNum(32, 0, temp, 3, 16);   // 假设temp不超过999，用3位
OLED_ShowString(64, 0, (u8*)"HUM:", 16);
OLED_ShowNum(96, 0, hum, 3, 16);

// 第二行
OLED_ShowString(0, 16, (u8*)"LIG:", 16);
OLED_ShowNum(32, 16, light, 3, 16); // 假设light可能4位
OLED_ShowString(64, 16, (u8*)"ALC:", 16);
OLED_ShowNum(90, 16, co2, 3, 16);   // co2可能是酒精，用4位

// 第三行
OLED_ShowString(0, 32, (u8*)"CO2:", 16); // 注意这里标签是CO2，但变量pm25可能应该是PM2.5？根据原代码，这里应该是PM2.5的值，可能标签应为"PM2.5"但长度5，宽度40，数字起始40，结束72，还行。但PM2.5可能三位数。如果标签改为"PM2.5"，宽度5*8=40，数字起始40，结束72，右半部分可再放一个？但原代码第三行只有这一个数据，可能是单独一行。也可以改为"PM2.5:"，然后数字放在后面。但屏幕还有空间，可以在右边再放一个数据，比如TVOC？但第四行已经有FIRE AIR了。根据原代码，第三行是"CO2:"和pm25，第四行是"FIRE AIR:"和tvoc，所以第三行只有左列一个数据，右列空白。可以保留这样。
OLED_ShowNum(32, 32, pm25, 4, 16); // 假设pm25三位数

// 第四行
OLED_ShowString(0, 48, (u8*)"FIRE AIR:", 16); // 宽度72
OLED_ShowNum(72, 48, tvoc, 4, 16); // 数字起始72，结束104
		OLED_Refresh_Gram();
		
		//报警判断
		if(temp>temp_H||hum>hum_H||light>light_H||co2>co2_H||tvoc>tvoc_H||pm25>pm25_H)
			BEEP=1;
		else
			BEEP=0;
		
		
		if(++timeCount >= 10)									//发送间隔5s   发送数据到云平台
		{
			OneNet_SendData();									//发送数据
			timeCount = 0;
			ESP8266_Clear();             //清除ESP8266缓存
		}
		
		//解析接收来自云平台的数据
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
		{
			OneNet_RevPro(dataPtr);
		}
		
		delay_ms(5);
	}
}



