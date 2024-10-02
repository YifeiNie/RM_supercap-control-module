#include "bsp_out_control.h"

void Out_Control_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	RCC_AHBPeriphClockCmd ( OUT_CONTROL_GPIO_CLK, ENABLE);  
	GPIO_InitStructure.GPIO_Pin =OUT_CONTROL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
		
	Out_Control(OPEN);

}

