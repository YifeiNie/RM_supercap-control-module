#ifndef __CAN_H
#define __CAN_H	 

#include "stm32f30x.h"
extern uint16_t MAX_POW;

void CAN1_Init(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp);
void CAN_SEND_DATA(int16_t  data);
int16_t float2int16(float V, float W);

#endif

