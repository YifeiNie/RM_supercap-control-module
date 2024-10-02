#include "bsp_led.h"

void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	RCC_AHBPeriphClockCmd ( LED1_GPIO_CLK|LED1_GPIO_CLK, ENABLE);  
	GPIO_InitStructure.GPIO_Pin =LED1_PIN|LED2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	LED1(OFF);
	LED2(OFF);
}
