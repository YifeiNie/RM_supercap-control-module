#ifndef __BSP_HRTIM_H
#define __BSP_HRTIM_H

#include "stm32f30x.h"
#include "bsp_buck_control.h"

#define FSW 	         	250E3	//开关频率250K
#define PERIOD 	         	(uint16_t)(4608E6/FSW)  //4608E6是主时钟HSI(72MHz)先2倍频，再32倍频得到的HRTIM时钟(4.608GHz)

#define DeadTime_Value		200	//死区时间

#define MAX_CompareValue1 	(uint16_t)(1.13f*PERIOD)
#define MAX_CompareValue2 	(uint16_t)(1.65f*PERIOD)
#define MIN_CompareValue 	(uint16_t)(0.01f*PERIOD)
#define PERIOD_Half      	(uint16_t)(0.5f*PERIOD)
#define MAX_Duty         	(0.9f*PERIOD)
#define MIN_Duty         	(uint16_t)(0.1f*PERIOD)


extern uint16_t ADC_SampleBuf[4];//ADC采样值,通过DMA接收

void HRTIM_Config(void);
void PWM_Update(uint16_t CompareValue);

#endif
