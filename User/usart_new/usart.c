
#include "stm32f30x.h"
#include "delay.h"
#include "bsp_led.h"
#include "bsp_adc.h"
#include "bsp_hrtim.h"
#include "bsp_buck_control.h"
#include "bsp_pid.h"
#include "bsp_can.h"
#include "bsp_time.h"
#include "bsp_filter.h"
#include "bsp_out_control.h"
#include "stdio.h"
#include "usart.h"


void Initial_prepheral_(void)
{
	Init_TIM16();          //初始化定时器16
	USART1_Config();
}

//串口通信IO配置
//	PB6   USART1_TX  串口接发送
//	PB7   USART1_RX  串口发接收
void USART1_Config(void)
{
	GPIO_InitTypeDef		GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef  USART_InitStruct;
		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
		
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6,GPIO_AF_7);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7,GPIO_AF_7);
	
  //USART_DeInit(USART1);
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	
	USART_Init(USART1,&USART_InitStruct); 
	

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
	USART_ITConfig(USART1, USART_IT_ORE, ENABLE);//开启串口溢出中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
}
 __IO uint8_t    PC_command[8]={0};
 __IO uint8_t    flag_command_ok=0 ;
void USART1_IRQHandler(void)
{
	static uint8_t i=0;
	if( USART_GetFlagStatus(USART1, USART_FLAG_RXNE)==SET&&flag_command_ok==0)//MCU正在执行命令，此时PC重新发送命令，直接忽略
	{				 

				PC_command[i] = USART_ReceiveData(USART1); 
				if(PC_command[0]!=0xAA)i=0;            //PC命令格式无效，正确格式：AA 55 XX 
				else if(i==2){flag_command_ok=1;i=0;}        
				else i++;					
	}
		if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
    {
			  USART_ClearFlag(USART1,USART_FLAG_ORE);
			  //USART_ReceiveData(USART1);
		}
	USART_ClearITPendingBit(USART1,  USART_IT_RXNE); //清除中断标志位，退出中断 
}

void Print_state(void)
{
	printf("Input_Voltage = %f\r\n",Input_Voltage);
	printf("Input_Current = %f\r\n",Input_Current);
	printf("Duty = %d\r\n",PWM_DUTY);
	//打印输入功率信息，并判断充放电情况
	printf("Input_Power = %f\r\n",Input_Power);
	printf("Limit_Power = %f\r\n",Limit_Power);	
//	if (Input_Power<=(Limit_Power-5))
//	{
//		printf("Charge time++\r\n");
//	}
//	else if (Input_Power>=(Limit_Power+5))
//	{
//		printf("Output time++\r\n");
//	}
	if (Transition_Mode == 0)
	{
		printf("Working\r\n");
	}
	else
	{
		printf("Break\r\n");
	}
	// 打印超级电容信息
	printf("SCAP_Voltage = %f\r\n",SCAP_Voltage);
	printf("SCAP_Current = %f\r\n",SCAP_Current);
	if (Mode == 1)
	{
		printf("Cap output!\r\n\n");
	}
	else if (Mode == 0)
	{
		printf("Cap charge!\r\n");
		if (loop_mode == 0)
		{
			printf("Const Vortage!\r\n");
		}
		else 
		{
			printf("Const Current!\r\n");
		}
	}
	float tem=(SCAP_Voltage-6.65f)*6.365f;
	if (tem<0) tem=0;
	else if (tem>99) tem=100;
	else tem=tem;
	printf("Engergy = %f%%\r\n\n",tem);
}










