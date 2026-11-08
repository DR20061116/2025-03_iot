#ifndef __STEPPER_MOTOR_H
#define __STEPPER_MOTOR_H

#include "stm32f10x.h"

// 引脚定义
#define MOTOR_PORT GPIOB
#define MOTOR_PIN1 GPIO_Pin_6
#define MOTOR_PIN2 GPIO_Pin_7
#define MOTOR_PIN3 GPIO_Pin_8
#define MOTOR_PIN4 GPIO_Pin_9

// 旋转方向
typedef enum {
    CLOCKWISE = 0,					//顺时针
    COUNTER_CLOCKWISE = 1		//逆时针
} MotorDirection;

// 速度范围 (us)
#define MIN_SPEED 500   // 500us = 0.5ms
#define MAX_SPEED 5000  // 5000us = 5ms

// 函数声明
void StepperMotor_Init(void);															//初始化
void StepperMotor_SetSpeed(uint16_t speed_us); 						// 参数改为微秒
void StepperMotor_SetDirection(MotorDirection direction);	// 设置方向
void StepperMotor_Step(uint16_t steps);										// 移动指定步数
void StepperMotor_RotateContinuous(void);									// 连续旋转
void StepperMotor_Stop(void);															// 停止电机
uint8_t StepperMotor_IsRunning(void);											// 检查是否运行

void OpenCurtain(void);
void CloseCurtain(void);

#endif /* __STEPPER_MOTOR_H */
