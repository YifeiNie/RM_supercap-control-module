#ifndef __INIT_H__
#define __INIT_H__

#include "stm32f30x.h"
#include "stm32f30x_conf.h"
#include "delay.h"
#include "stm32f30x_tim.h"


#define   Disable   0
#define   Enable    1
void Init_TIM16(void);				//初始化计时器

void GPIO_InitConfig(void);       //初始化IO口

void Initial_prepheral_(void);    //初始化外设，具体参考函数内部

void USART1_Config(void);				//初始化串口1

void Print_state(void);					//打印当前各个ADC状态			

extern float Input_Voltage,Input_Current,Input_Power,Available_Power,SCAP_Voltage,SCAP_Current,Limit_Power,Input_Power;//电气参数转换值
extern uint8_t Mode,loop_mode,Transition_Mode;
extern uint16_t PWM_DUTY;
#endif

