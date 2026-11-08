#ifndef __INFRARED_H
#define __INFRARED_H
#include "stm32f10x.h"
#include "sys.h"
//检测是否有人结经过 1有人 0无人
#define PEOPLE PAin (2)

void INFRARED_Init(void);

#endif
