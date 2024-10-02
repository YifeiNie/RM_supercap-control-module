#include "bsp_hrtim.h"

void HRTIM_Config(void)
{
	GPIO_InitTypeDef 			GPIO_InitStructure;
	HRTIM_OutputCfgTypeDef	 	HRTIM_TIM_OutputStructure;
	HRTIM_BaseInitTypeDef 		HRTIM_BaseInitStructure;
	HRTIM_TimerInitTypeDef 		HRTIM_TimerInitStructure;  
	HRTIM_TimerCfgTypeDef 		HRTIM_TimerWaveStructure;
	HRTIM_DeadTimeCfgTypeDef 	HRTIM_DeadtimeStructure;
	HRTIM_ADCTriggerCfgTypeDef	HRTIM_ADCTrigStructure;
	NVIC_InitTypeDef 			NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;  
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;  
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8 , GPIO_AF_13);//PA8复用
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9 , GPIO_AF_13);//PA9复用  
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_13);//PA10复用
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_13);//PA11复用
	
	RCC_HRTIM1CLKConfig(RCC_HRTIM1CLK_PLLCLK);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_HRTIM1,ENABLE);
 
	HRTIM_DLLCalibrationStart(HRTIM1, HRTIM_CALIBRATIONRATE_14);
	//while(HRTIM_GetCommonFlagStatus(HRTIM1, HRTIM_ISR_DLLRDY) == RESET);

	/* --------------------------------------------------- */
	/* HRTIM_TIMERINDEX_MASTER initialization: timer mode and PWM frequency */
	/* --------------------------------------------------- */
	HRTIM_BaseInitStructure.Period					= PERIOD;// HRTIM周期寄存器, Period = fHRCK / fsw
	HRTIM_BaseInitStructure.PrescalerRatio			= HRTIM_PRESCALERRATIO_MUL32;//HRTIM时钟倍频系数
	HRTIM_BaseInitStructure.Mode					= HRTIM_MODE_CONTINOUS;//HRTIM运行在连续模式
	HRTIM_BaseInitStructure.RepetitionCounter		= 7;	// 中断频率 = FSW/(7+1)
	
	HRTIM_TimerInitStructure.HalfModeEnable 		= HRTIM_HALFMODE_DISABLED;
	HRTIM_TimerInitStructure.StartOnSync 			= HRTIM_SYNCSTART_DISABLED;
	HRTIM_TimerInitStructure.ResetOnSync 			= HRTIM_SYNCRESET_DISABLED;
	HRTIM_TimerInitStructure.DACSynchro 			= HRTIM_DACSYNC_NONE;
	HRTIM_TimerInitStructure.PreloadEnable 			= HRTIM_PRELOAD_DISABLED;//直接加载
	HRTIM_TimerInitStructure.UpdateGating 			= HRTIM_UPDATEGATING_INDEPENDENT;
	HRTIM_TimerInitStructure.BurstMode 				= HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
	HRTIM_TimerInitStructure.RepetitionUpdate 		= HRTIM_UPDATEONREPETITION_ENABLED;

	HRTIM_Waveform_Init(HRTIM1,HRTIM_TIMERINDEX_MASTER,&HRTIM_BaseInitStructure,&HRTIM_TimerInitStructure);
	
	/* --------------------------------------------------- */
	/* HRTIMx initialization: timer mode and PWM frequency */
	/* --------------------------------------------------- */
	HRTIM_TimerInitStructure.RepetitionUpdate 		= HRTIM_UPDATEONREPETITION_DISABLED;	
	
	HRTIM_BaseInitStructure.Mode					= HRTIM_MODE_SINGLESHOT;//HRTIM运行在非连续模式，使用MASTER作为复位时钟
	
	HRTIM_Waveform_Init(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, &HRTIM_BaseInitStructure, &HRTIM_TimerInitStructure);    
	HRTIM_Waveform_Init(HRTIM1, HRTIM_TIMERINDEX_TIMER_B, &HRTIM_BaseInitStructure, &HRTIM_TimerInitStructure);	
	
	/* ------------------------------------------------ */
	/* TIMERx output and registers update configuration */
	/* ------------------------------------------------ */
	HRTIM_TimerWaveStructure.DeadTimeInsertion		= HRTIM_TIMDEADTIMEINSERTION_ENABLED;//死区时间：使能(对称互补模式)
	HRTIM_TimerWaveStructure.DelayedProtectionMode 	= HRTIM_TIMDELAYEDPROTECTION_DISABLED;//延时保护：无
	HRTIM_TimerWaveStructure.FaultEnable 			= HRTIM_TIMFAULTENABLE_NONE;//故障使能：无
	HRTIM_TimerWaveStructure.FaultLock 				= HRTIM_TIMFAULTLOCK_READWRITE;//故障位	：可读写
	HRTIM_TimerWaveStructure.PushPull 				= HRTIM_TIMPUSHPULLMODE_DISABLED;
	HRTIM_TimerWaveStructure.ResetTrigger 			= HRTIM_TIMRESETTRIGGER_MASTER_PER;
	HRTIM_TimerWaveStructure.ResetUpdate 			= HRTIM_TIMUPDATEONRESET_DISABLED;
	HRTIM_TimerWaveStructure.UpdateTrigger 			= HRTIM_TIMUPDATETRIGGER_MASTER;
	
	HRTIM_WaveformTimerConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_A,&HRTIM_TimerWaveStructure);
	HRTIM_WaveformTimerConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_B,&HRTIM_TimerWaveStructure);
	
	/* -------------------------------- */
	/* -------HRTIM TAx 输出配置 -------*/
	/* -------------------------------- */		
	HRTIM_TIM_OutputStructure.Polarity				= HRTIM_OUTPUTPOLARITY_LOW;//极性控制位：正极性
	HRTIM_TIM_OutputStructure.SetSource				= HRTIM_OUTPUTRESET_MASTERCMP1;
	HRTIM_TIM_OutputStructure.ResetSource			= HRTIM_OUTPUTRESET_MASTERCMP4;
	HRTIM_TIM_OutputStructure.IdleMode 				= HRTIM_OUTPUTIDLEMODE_NONE;//输出不受空闲状态影响  
	HRTIM_TIM_OutputStructure.IdleState 			= HRTIM_OUTPUTIDLESTATE_INACTIVE;//输出不受空闲状态影响          
	HRTIM_TIM_OutputStructure.FaultState  			= HRTIM_OUTPUTFAULTSTATE_NONE;//故障状态：PWM输出不受故障影响          
	HRTIM_TIM_OutputStructure.ChopperModeEnable 	= HRTIM_OUTPUTCHOPPERMODE_DISABLED;//不使用斩波器         
	HRTIM_TIM_OutputStructure.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;//死区时间不插入空闲状态
	
	HRTIM_WaveformOutputConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_A,HRTIM_OUTPUT_TA2,&HRTIM_TIM_OutputStructure);
	
	HRTIM_TIM_OutputStructure.Polarity				= HRTIM_OUTPUTPOLARITY_LOW;
	HRTIM_WaveformOutputConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_A,HRTIM_OUTPUT_TA1,&HRTIM_TIM_OutputStructure);

	/* -------------------------------- */
	/* -------HRTIM TBx 输出配置 -------*/
	/* -------------------------------- */		
	HRTIM_TIM_OutputStructure.Polarity				= HRTIM_OUTPUTPOLARITY_LOW;//极性控制位：正极性
	HRTIM_TIM_OutputStructure.SetSource				= HRTIM_OUTPUTRESET_MASTERCMP2;
	HRTIM_TIM_OutputStructure.ResetSource			= HRTIM_OUTPUTRESET_MASTERCMP3;
	HRTIM_TIM_OutputStructure.IdleMode 				= HRTIM_OUTPUTIDLEMODE_NONE;//输出不受空闲状态影响  
	HRTIM_TIM_OutputStructure.IdleState 			= HRTIM_OUTPUTIDLESTATE_INACTIVE;//输出不受空闲状态影响          
	HRTIM_TIM_OutputStructure.FaultState  			= HRTIM_OUTPUTFAULTSTATE_NONE;//故障状态：PWM输出不受故障影响          
	HRTIM_TIM_OutputStructure.ChopperModeEnable 	= HRTIM_OUTPUTCHOPPERMODE_DISABLED;//不使用斩波器         
	HRTIM_TIM_OutputStructure.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;//死区时间不插入空闲状态
	
	HRTIM_WaveformOutputConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_B,HRTIM_OUTPUT_TB2,&HRTIM_TIM_OutputStructure);
	
	HRTIM_TIM_OutputStructure.Polarity				= HRTIM_OUTPUTPOLARITY_LOW;
	HRTIM_WaveformOutputConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_B,HRTIM_OUTPUT_TB1,&HRTIM_TIM_OutputStructure);
	
	
	HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_1, PERIOD_Half);
	HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_4, PERIOD_Half);	

	HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_2, PERIOD_Half);
	HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_3, PERIOD_Half);
	
	/* -------------------------------- */
	/* -----HRTIM PWM 死区时间配置 ---- */
	/* -------------------------------- */
	HRTIM_DeadtimeStructure.Prescaler 				= 0x00;//死区时间预分频器
	HRTIM_DeadtimeStructure.FallingLock				= HRTIM_TIMDEADTIME_FALLINGLOCK_WRITE;
	HRTIM_DeadtimeStructure.FallingSign				= HRTIM_TIMDEADTIME_FALLINGSIGN_NEGATIVE;
	HRTIM_DeadtimeStructure.FallingSignLock			= HRTIM_TIMDEADTIME_FALLINGSIGNLOCK_WRITE;
	HRTIM_DeadtimeStructure.FallingValue			= DeadTime_Value; //死区时间
	HRTIM_DeadtimeStructure.RisingLock				= HRTIM_TIMDEADTIME_RISINGLOCK_WRITE;
	HRTIM_DeadtimeStructure.RisingSign				= HRTIM_TIMDEADTIME_RISINGSIGN_NEGATIVE;
	HRTIM_DeadtimeStructure.RisingSignLock			= HRTIM_TIMDEADTIME_RISINGSIGNLOCK_WRITE;
	HRTIM_DeadtimeStructure.RisingValue				= DeadTime_Value;
	
	HRTIM_DeadTimeConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_A,&HRTIM_DeadtimeStructure);
	HRTIM_DeadTimeConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_B,&HRTIM_DeadtimeStructure);
	
	/* --------------------------*/
	/* ADC trigger initialization */
	/* --------------------------*/
	HRTIM_ADCTrigStructure.Trigger      			= HRTIM_ADCTRIGGEREVENT13_MASTER_PERIOD;
	HRTIM_ADCTrigStructure.UpdateSource 			= HRTIM_ADCTRIGGERUPDATE_MASTER;
	HRTIM_ADCTriggerConfig(HRTIM1, HRTIM_ADCTRIGGER_1, &HRTIM_ADCTrigStructure);
	
	/* -------------------------*/
	/* Interrupt initialization */
	/* -------------------------*/
	/* Configure and enable HRTIM TIMERA interrupt channel in NVIC */
	NVIC_InitStructure.NVIC_IRQChannel 						= HRTIM1_TIMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* TIMER A issues an interrupt on each repetition event */
	HRTIM_ITConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_TIM_IT_REP, ENABLE);//开启HRTIM_TIMERA中断
	

//	HRTIM_WaveformOutputStart(HRTIM1,HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TA2);
//	HRTIM_WaveformOutputStart(HRTIM1,HRTIM_OUTPUT_TB1 | HRTIM_OUTPUT_TB2);

	HRTIM_WaveformCounterStart(HRTIM1,HRTIM_TIMERID_TIMER_A);
	HRTIM_WaveformCounterStart(HRTIM1,HRTIM_TIMERID_TIMER_B);
	HRTIM_WaveformCounterStart(HRTIM1,HRTIM_TIMERID_MASTER);
}


void PWM_Update(uint16_t CompareValue)
{
	uint16_t	buck_duty,boost_duty;
	
	//输出方向判断
	if(Direction==0)//正向输出限制
	{
		if(CompareValue > MAX_CompareValue1) CompareValue	= MAX_CompareValue1;	//限制最大占空比
		if(CompareValue < MIN_CompareValue ) CompareValue	= MIN_CompareValue;		//限制最小占空比
	}
	else//反向输出限制
	{
		if(CompareValue > MAX_CompareValue2) CompareValue	= MAX_CompareValue2;	//限制最大占空比
		if(CompareValue < MIN_CompareValue ) CompareValue	= MIN_CompareValue;		//限制最小占空比
	}

	if(CompareValue>0.9f*PERIOD)  							//当总占空比大于90%时，进入BOOST模式
		boost_duty	= CompareValue/2 - 0.85f*PERIOD_Half;	//设置boost的占空比，并且模拟中心对称的PWM。注意要减去buck_duty的占空比: 0.85*PERIOD_Half
	else
		boost_duty	= 0.1f * PERIOD_Half;					//当总占空比不大于90%时，进入BUCK模式,boost_duty给固定占空比10%

	if(CompareValue>0.9f*PERIOD)
		buck_duty	= 0.9f * PERIOD_Half;					//当总占空比大于90%时，进入BOOST模式,buck_duty给固定占空比90%
	else
		buck_duty	= CompareValue/2;						//设置buck的占空比，并且模拟中心对称的PWM。
	
	
//	if(CompareValue > MAX_Duty)  							//当总占空比大于90%时，进入BOOST模式
//	{
//		boost_duty	= CompareValue/2 - 0.85f*PERIOD_Half;	//设置boost的占空比，并且模拟中心对称的PWM。注意要减去buck_duty的占空比: 0.85*PERIOD_Half
//	}
//	else
//	{
//		boost_duty	= MIN_Duty;					//当总占空比不大于90%时，进入BUCK模式,boost_duty给固定占空比10%
//	}
//	
//	if(CompareValue > MAX_Duty)
//	{	
//		buck_duty	= MAX_Duty;					//当总占空比大于90%时，进入BOOST模式,buck_duty给固定占空比90%	
//	}
//	else
//	{
//		buck_duty	= CompareValue/2;			//设置buck的占空比，并且模拟中心对称的PWM。
//	}

	//模式判断
	if(Transition_Mode==1)//上桥臂关断，下桥臂导通
	{
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_1, PERIOD_Half);
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_4, PERIOD_Half);	

		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_2, PERIOD_Half);
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_3, PERIOD_Half);	
		return;
	}
	
	if(Direction==1 && Transition_Mode==0)//反向输出
	{
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_1, PERIOD_Half + boost_duty);//PWM置1比较器，模拟中心对称的PWM，以PWM_PER_0_5为中心
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_4, PERIOD_Half - boost_duty);//PWM置0比较器，比如（400-50）与（400+50）的中心点为400	

		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_2, PERIOD_Half - buck_duty);
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_3, PERIOD_Half + buck_duty);
	}
	if(Direction==0 && Transition_Mode==0)//正向输出
	{
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_1, PERIOD_Half - buck_duty);//PWM置1比较器，模拟中心对称的PWM，以PWM_PER_0_5为中心
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_4, PERIOD_Half + buck_duty);//PWM置0比较器，比如（400-50）与（400+50）的中心点为400	

		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_2, PERIOD_Half + boost_duty);
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_3, PERIOD_Half - boost_duty);	
	}
}



















