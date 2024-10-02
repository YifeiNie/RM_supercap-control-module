#include "bsp_adc.h"
#include "stdio.h"

uint16_t ADC_SampleBuf[4];      //ADC采样值,通过DMA接收
__IO uint16_t calibration_value = 0;

void ADC_Config(void)
{
  ADC1_DMA_Config();
	ADC1_Config();         //初始化ADC1，启用DMA：
}

void ADC1_Config(void)
{
	GPIO_InitTypeDef			GPIO_InitStructure;
	ADC_InitTypeDef 			ADC_InitStructure;	
	
	ADC_CommonInitTypeDef 		ADC_CommonInitStructure;
	ADC_DeInit(ADC1);
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                              //不起作用
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	ADC_StructInit(&ADC_InitStructure);
	ADC_VoltageRegulatorCmd(ADC1, ENABLE);
	
	ADC_SelectCalibrationMode( ADC1, ADC_CalibrationMode_Single);
	ADC_StartCalibration( ADC1);
	
	while(ADC_GetCalibrationStatus(ADC1) != RESET );
	calibration_value = ADC_GetCalibrationValue(ADC1);
	

	ADC_CommonInitStructure.ADC_Mode             	= ADC_Mode_Independent;    //ADC独立模式，本应用中只用到ADC1                                                               
	ADC_CommonInitStructure.ADC_Clock            	= ADC_Clock_AsynClkMode;   //时钟异步模式                
	ADC_CommonInitStructure.ADC_DMAAccessMode    	= ADC_DMAAccessMode_1;     //DMA 12bit 分辨率使能        
	ADC_CommonInitStructure.ADC_DMAMode          	= ADC_DMAMode_Circular;    //DMA为连续模式            
	ADC_CommonInitStructure.ADC_TwoSamplingDelay 	= 10;          			   //采样时间间隔
	ADC_CommonInit(ADC1, &ADC_CommonInitStructure);

	ADC_InitStructure.ADC_ContinuousConvMode     	= ADC_ContinuousConvMode_Disable;
	ADC_InitStructure.ADC_Resolution             	= ADC_Resolution_12b;  					//12bit 分辨率
	ADC_InitStructure.ADC_ExternalTrigConvEvent  	= ADC_ExternalTrigConvEvent_7;           //配置HRtim的ADCTRIGGER_1为触发信号
	ADC_InitStructure.ADC_ExternalTrigEventEdge  	= ADC_ExternalTrigEventEdge_FallingEdge; //触发信号上升沿启动
	ADC_InitStructure.ADC_DataAlign              	= ADC_DataAlign_Right;					//右对齐
	ADC_InitStructure.ADC_OverrunMode            	= ADC_OverrunMode_Disable;   
	ADC_InitStructure.ADC_AutoInjMode            	= ADC_AutoInjec_Disable;  
	ADC_InitStructure.ADC_NbrOfRegChannel        	= 4;
	ADC_Init( ADC1, &ADC_InitStructure);
		

	ADC_RegularChannelConfig(ADC1, ADC_Channel_1,  1, ADC_SampleTime_7Cycles5);/* to PA0 (I_VSCAP)  */			//四个通道对应PA0，1，2，3四个引脚
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2,  2, ADC_SampleTime_7Cycles5);/* to PA1 (I_BAT) */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3,  3, ADC_SampleTime_7Cycles5);/* to PA2 (VBAT) */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4,  4, ADC_SampleTime_7Cycles5);/* to PA3 (VSCAP) */	

	ADC_Cmd(ADC1, ENABLE);
	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY));//wait for ADC1 ADRDY
	ADC_DMACmd(ADC1, ENABLE);
	ADC_DMAConfig(ADC1, ADC_DMAMode_Circular);		//ADC1与DMA	
	ADC_StartConversion(ADC1);		
	

}



void ADC1_DMA_Config(void)
{
	DMA_InitTypeDef 			DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	
	DMA_DeInit(DMA1_Channel1);
	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr 		= (uint32_t)(0x50000040);
	DMA_InitStructure.DMA_MemoryBaseAddr 			= (uint32_t)&ADC_SampleBuf;
	DMA_InitStructure.DMA_DIR 						= DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize 				= 4;
	DMA_InitStructure.DMA_PeripheralInc 			= DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc 				= DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize 		= DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize 			= DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode 						= DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority 					= DMA_Priority_High;
	DMA_InitStructure.DMA_M2M 						= DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);							
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	Delay_ms(1);
}









