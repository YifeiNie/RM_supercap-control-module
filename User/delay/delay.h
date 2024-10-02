#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f30x.h"

void Delay_Init(uint8_t sysclk);
void Delay_us(uint16_t time);
void Delay_ms(__IO uint32_t nCount);
void Init_TIM16(void);
#endif
