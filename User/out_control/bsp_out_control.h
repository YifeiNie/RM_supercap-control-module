#ifndef __BSP_OUT_CONTROL_H
#define __BSP_OUT_CONTROL_H

#include "stm32f30x.h"

//Òý½Å¶¨Òå
/*******************************************************/
#define OUT_CONTROL_PIN                  GPIO_Pin_14
#define OUT_CONTROL_GPIO_PORT            GPIOC
#define OUT_CONTROL_GPIO_CLK             RCC_AHBPeriph_GPIOC

#define OPEN  1
#define CLOSE 0

#define Out_Control(a) if (a)  \
	                GPIO_SetBits(OUT_CONTROL_GPIO_PORT,OUT_CONTROL_PIN);\
				    else      \
				    GPIO_ResetBits(OUT_CONTROL_GPIO_PORT,OUT_CONTROL_PIN)

void Out_Control_GPIO_Config(void);

#endif
