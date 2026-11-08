
#ifndef __PWM_H
#define __PWM_H

void Pwm_Init(void);
void Car_Left_Tire(uint8_t Compare,uint8_t mode); //小车左电机PWM  mode:1正转,0反转
void Car_Right_Tire(uint8_t Compare,uint8_t mode);//小车右电机PWM
void car_io_init(void);

#include "sys.h"

#define AN1 PCout(0)
#define AN2 PCout(1)
#define BN1 PCout(2)
#define BN2 PCout(3)

#endif 
