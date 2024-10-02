#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f30x.h"

//Òý½Å¶¨Òå
/*******************************************************/
#define LED1_PIN                         GPIO_Pin_3
#define LED1_GPIO_PORT                   GPIOB
#define LED1_GPIO_CLK                    RCC_AHBPeriph_GPIOB

#define LED2_PIN                         GPIO_Pin_4
#define LED2_GPIO_PORT                   GPIOB
#define LED2_GPIO_CLK                    RCC_AHBPeriph_GPIOB

#define ON  0
#define OFF 1

#define LED1(a) if (a)  \
	                GPIO_SetBits(LED1_GPIO_PORT,LED1_PIN);\
				    else      \
				    GPIO_ResetBits(LED1_GPIO_PORT,LED1_PIN)

#define LED2(a) if (a)  \
	                GPIO_SetBits(LED2_GPIO_PORT,LED2_PIN);\
				    else      \
				    GPIO_ResetBits(LED2_GPIO_PORT,LED2_PIN)					

void LED_GPIO_Config(void);

#endif
