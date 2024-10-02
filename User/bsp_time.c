#include  "bsp_time.h"


//定时器2初始化

void Bsp_TimeInit(uint32_t freq)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
	
	TIM_TimeBaseStructure.TIM_Period            = 1e6/freq;            //比较值,8e6 = SystemCoreClock/(Prescaler+1)
	TIM_TimeBaseStructure.TIM_Prescaler         = 63;                  //预分频,SystemCoreClock/(Prescaler+1)的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision     = 0;                   //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);                    //根据TIM_TimeBaseInitStruct中指定的参数
                                                                     //初始化TIMx的时间基数单位
 
	TIM_ITConfig(  
			TIM1,            //TIMx
			TIM_IT_Update,   //TIM 触发中断源 
			ENABLE           //使能
		);                 //使能或者失能指定的TIM中断
	
	NVIC_InitStructure.NVIC_IRQChannel                     = TIM1_UP_TIM16_IRQn; //TIM1中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority   = 1;             //抢占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority          = 3;             //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd                  = ENABLE;        //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd( TIM1, ENABLE ); //使能TIMx外设	
}

