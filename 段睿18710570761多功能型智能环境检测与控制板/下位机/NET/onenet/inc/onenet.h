#ifndef _ONENET_H_
#define _ONENET_H_

#include "stm32f10x.h"


_Bool OneNET_RegisterDevice(void);

_Bool OneNet_DevLink(void);

void OneNet_SendData(void);

void OneNET_Subscribe(void);

void OneNet_RevPro(unsigned char *cmd);

extern u8 temp,hum;                 //t1 h1
extern u8 temp_H,temp_L,hum_H,hum_L;//t2 t3 h2 h3
extern u16 pm25;										//p1
extern u16 pm25_H,pm25_L;						//p2 p3
extern u16 light;										//l1
extern u16 light_H,light_L;					//l2 l3
extern u16 co2;											//c1
extern u16 co2_H,co2_L;							//c2 c3
extern u16 tvoc;										//o1
extern u16 tvoc_H,tvoc_L;						//o2 o3

#endif
