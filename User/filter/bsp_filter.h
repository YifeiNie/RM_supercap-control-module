/**
  ******************************************************************************
  * @author  何。小P
  * @version V1.0
  * @date    2018-08-22
  * @brief   桂林安合科技有限公司 版权所有（C）All rights reserved.
  ******************************************************************************
	* 
	*	文件名：dp_filter_lib.h
	*	功  能：数字滤波算法
    *
  ******************************************************************************
**/ 


#ifndef _DP_FILTER_H
#define _DP_FILTER_H

#include "stm32f30x.h"

#define FilterBuf_MaxLen 3u

typedef struct{

	float Base[FilterBuf_MaxLen];
	int16_t   Head;
	int16_t   Rear;

}FilterBuf_STRUCT;


void    FilterSample( FilterBuf_STRUCT* p, float NewValue);
float   FilterAverage( FilterBuf_STRUCT* p);

#endif










