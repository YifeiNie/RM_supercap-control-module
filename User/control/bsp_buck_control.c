#include "bsp_buck_control.h"
#include "bsp_can.h"
#include "stdio.h"
float ADC_I_SCAP_SampleValue;    //超级电容输入输出电流ADC采样值,	偏置2.25V			I=(3.3/4096*读取的无符号数-2.25)/50/0.0015，也等于3.3/4096*读取的无符号数/50/0.0015-30
float ADC_I_BAT_SampleValue;     //电池输入电流ADC采样值,												I=3.3/4096/50/0.005*读取的无符号数
float ADC_V_BAT_SampleValue;     //电池输入电压ADC采样值,衰减16倍,							V=3.3/4096*16*读取的无符号数
float ADC_V_SCAP_SampleValue;    //超级电容电压ADC采样值,衰减16倍,							V=3.3/4096*16*读取的无符号数

float IoRefApp,VoRefApp;//电流环设定值
float Input_Voltage,Input_Current,Input_Power,Available_Power,SCAP_Voltage,SCAP_Current;//电气参数转换值

uint16_t 	PWM_DUTY	= 0;	//PWM时钟计数比较值(占空比)
uint16_t	Vref 		= 1850;	//默认输出电压采样值,对应22.36V,即充电到22.36V认为电容充满
float Limit_Power;			//限制充电的功率

FilterBuf_STRUCT ADC_I_SCAP_FilterBuf;  //超级电容输入输出电流样本,用于数字滤波
FilterBuf_STRUCT ADC_I_BAT_FilterBuf;   //电池输入电流样本,用于数字滤波
FilterBuf_STRUCT ADC_V_BAT_FilterBuf;   //电池输入电压样本,用于数字滤波
FilterBuf_STRUCT ADC_V_SCAP_FilterBuf;  //超级电容电压样本,用于数字滤波


PID_STRUCT  gPID_AllIoutLoop;	    //放电功率环PID数据
PID_STRUCT  gPID_OutPowerCurr;	  //放电电流环PID数据

PID_STRUCT	gPID_VoltOutLoop;     //充电电压环PID数据
PID_STRUCT	gPID_CurrOutLoop;     //充电电流环PID数据
PID_STRUCT  gPID_PowerOutLoop;	  //充电功率环PID数据	

/*    外部函数声明     */
extern void PWM_Update(uint16_t CompareValue);

/**
 **********************************************************
 * @函数：void dp_cntl_init(void)
 * @描述：初始化PID参数
 * @输入：无
 * @返回：无 
 **********************************************************
**/
void bsp_PIDCtrlInit(void)
{	
	//放电功率环
	pid_func.reset(&gPID_AllIoutLoop);
	gPID_AllIoutLoop.Kp       = 2.0f;//PID控制周期，单位100us
	gPID_AllIoutLoop.Ki       = 0.1f;
	gPID_AllIoutLoop.Kd       = 0.0f;
	gPID_AllIoutLoop.max_iout = 2000.0f;	//位置式积分限幅大小
	gPID_AllIoutLoop.min_out  = 0;//最小电流输出值（最大电流值）.对应-30A（因为是放电所以是负数）
	gPID_AllIoutLoop.max_out  = 2730;//最大电流输出值,对应-0.67A
	pid_func.init(&gPID_AllIoutLoop);
	
	//放电电流环
	pid_func.reset(&gPID_OutPowerCurr);
	gPID_OutPowerCurr.Kp       = 2.0f;//PID控制周期，单位100us
	gPID_OutPowerCurr.Ki       = 0.15f;	//1.9
	gPID_OutPowerCurr.Kd       = 0.0f;
	gPID_OutPowerCurr.max_iout = 25000.0f;	//位置式积分限幅大小，即PI的最大值
	gPID_OutPowerCurr.min_out  = 0.01f * PERIOD;//最小占空比
	gPID_OutPowerCurr.max_out  = 1.61f * PERIOD;//最大占空比
	pid_func.init(&gPID_OutPowerCurr);
	
	//充电电压环
	pid_func.reset(&gPID_VoltOutLoop);
	gPID_VoltOutLoop.Kp       = 2.00f;	
	gPID_VoltOutLoop.Ki       = 0.1f;
	gPID_VoltOutLoop.Kd       = 0.00f;
	gPID_VoltOutLoop.max_iout = 20000.0f;
	gPID_VoltOutLoop.min_out  = 0.01f * PERIOD;//最小占空比
	gPID_VoltOutLoop.max_out  = 1.13f * PERIOD;//最大占空比
	pid_func.init(&gPID_VoltOutLoop);

	//充电电流环
	pid_func.reset(&gPID_CurrOutLoop);
	gPID_CurrOutLoop.Kp       = 2.0f;//PID控制周期，单位100us
	gPID_CurrOutLoop.Ki       = 0.11f;
	gPID_CurrOutLoop.Kd       = 0.0f;
	gPID_CurrOutLoop.max_iout = 19000.0f;	//位置式积分限幅大小，即PI的最大值
	gPID_CurrOutLoop.min_out  = 0.01f * PERIOD;//最小占空比
	gPID_CurrOutLoop.max_out  = 1.13f * PERIOD;//最大占空比
	pid_func.init(&gPID_CurrOutLoop);
	
	//充电功率环
	pid_func.reset(&gPID_PowerOutLoop);
	gPID_PowerOutLoop.Kp       = 2.0f;//PID控制周期，单位100us
	gPID_PowerOutLoop.Ki       = 0.1f;
	gPID_PowerOutLoop.Kd       = 0.0f;
	gPID_PowerOutLoop.max_iout = 3300.0f;	//位置式积分限幅大小，即PI的最大值
	gPID_PowerOutLoop.min_out  = 2790;//最小电流值,对应-0.035A
	gPID_PowerOutLoop.max_out  = 3900;//最大电流值,对应11.88A
	pid_func.init(&gPID_PowerOutLoop);

}

uint8_t	  Direction=0;//正向或反向,1表示电容供电，0表示电容充电
uint8_t	  Mode=0;//0表示在充电，1表示在放电
uint8_t	  Last_Direction_Mode=0;
uint8_t	  loop_mode = 0x00;
uint8_t   Transition_Mode=0;//0:正常状态;1:异常状态;2:过渡状态
uint32_t  Transition_Time_Out=0; //充电状态计数器
uint8_t   Mediate_State_Out=0;
uint32_t  Transition_Time_In=0;
uint8_t   Mediate_State_In=0;
uint32_t  Clear_Time=0;
uint8_t   Clear_State=0;
uint16_t	  Turn_VoltOutLoop_Time=0;


void InputPower_Control(void)
{	
	/*  将ADC采样值存入滤波数组中  */
	FilterSample( &ADC_I_SCAP_FilterBuf, ADC_SampleBuf[0]);//超级电容输入输出电流
	FilterSample( &ADC_I_BAT_FilterBuf , ADC_SampleBuf[1]);//电池输入电流
	FilterSample( &ADC_V_BAT_FilterBuf , ADC_SampleBuf[2]);//电池输入电压
	FilterSample( &ADC_V_SCAP_FilterBuf, ADC_SampleBuf[3]);//超级电容电压
	
	/*  将ADC值进行数组滤波  */
	ADC_I_SCAP_SampleValue  = FilterAverage( &ADC_I_SCAP_FilterBuf );//超级电容输入输出电流
	ADC_I_BAT_SampleValue   = FilterAverage( &ADC_I_BAT_FilterBuf  );//电池输入电流
	ADC_V_BAT_SampleValue   = FilterAverage( &ADC_V_BAT_FilterBuf  );//电池输入电压
	ADC_V_SCAP_SampleValue  = FilterAverage( &ADC_V_SCAP_FilterBuf );//超级电容电压
	
	/*  并联输出功率环控制  */
	//电气参数转换
	Input_Voltage=ADC_V_BAT_SampleValue*Voltage_Factor;
	SCAP_Voltage=ADC_V_SCAP_SampleValue*Voltage_Factor;
	Input_Current=ADC_I_BAT_SampleValue*Current_Factor_Pow;
	SCAP_Current=ADC_I_SCAP_SampleValue*Current_Factor_Cap-30;
	
	
	Input_Power=Input_Voltage*Input_Current;
	Limit_Power=MAX_POW-4;   //MAX_POW由CAN通过裁判系统给出
	
	//异常状态延迟时间 
	
	//过渡状态计数(每过一段时间一次轮询，此状态更新只要满足比PID计算更新慢即可满足要求)
	//判断是此时超级电容是供电状态还是充电状态，超功率就是供电状态，剩余功率就是充电状态
	Transition_Time_Out = Input_Power>=(Limit_Power+5)  ? ++Transition_Time_Out : 0;
	Transition_Time_In = Input_Power<=(Limit_Power-5)  ?  ++Transition_Time_In : 0;
	Clear_Time = Last_Direction_Mode!=Direction ? ++Clear_Time : 0;
	if(Transition_Time_Out>90)	// 计数器超过一定数量，判断进入某个状态
	{
		Mediate_State_Out=1;			//放电
		Transition_Time_Out=0;
		Mediate_State_In=0;
	}
	if(Transition_Time_In>90)	
	{
		Mediate_State_In=1;				//充电
		Transition_Time_In=0;
		Mediate_State_Out=0;
	}
	if(Clear_Time>25)
	{
		Clear_State=1;
		Clear_Time=0;
	}
	
	if(ADC_SampleBuf[2]>3300 || ADC_SampleBuf[3]>3300)//异常状态PWM刹车
	{
		Transition_Mode=1;
	}
	
	//环路状态检测用瞬时值做检测，滞后切换	
	if(Mediate_State_Out==1 && ADC_SampleBuf[3]<3000 && ADC_SampleBuf[2]<3000)//此时电容放电
	{
		Direction=1;//超级电容反向并联输出 	
		//超级电容放电最小值判断（因为有虚电做两次判断，这里应该有问题，ADC没那么快转化完成，（因为有虚电做两次判断，这里虚电的参数要调参！！！！！！！））
		if(ADC_SampleBuf[3]<550) //6.65V
		{
			Transition_Mode=1;
		}
		if(Transition_Mode==1 && ADC_SampleBuf[3]<1200) 
		{
			Transition_Mode=1;
			PWM_Update(PWM_DUTY);	//更新PWM占空比	
			return;
		}
		Transition_Mode=0;
	}
	else if(Mediate_State_In==1 && ADC_SampleBuf[2]<3000 && ADC_SampleBuf[3]<3000)
	{
		Direction=0;//超级电容正向充电
		Transition_Mode=0;
	}
	
	//模式切换PID数据清零,PWM刹车
	if(Clear_State==1)
	{
		pid_func.clc(&gPID_AllIoutLoop);
		pid_func.clc(&gPID_OutPowerCurr);
		pid_func.clc(&gPID_VoltOutLoop);
		pid_func.clc(&gPID_CurrOutLoop);
		pid_func.clc(&gPID_PowerOutLoop);
		Clear_State=0;
	}
	Last_Direction_Mode=Direction;
	
	
	/**  PID计算  **/

	if(Direction==1 && Transition_Mode==0)//超级电容放电模式，双环串联，内环电流外环功率
	{	
		Mode = 1;
		gPID_OutPowerCurr.max_out  = (-6.5002f)*ADC_V_SCAP_SampleValue + 34070;//最大占空比
		gPID_OutPowerCurr.max_iout = gPID_OutPowerCurr.max_out - 3000 ;
		
		
		/* power_cal和函数void pid_power_calc (PID_STRUCT *pid, float fdb, float ref)一样
		误差输入 = ref-fdb, 功能是如果超功率了，就尽可能完全由超级电容供电而电源不供电，
		故输入功率环的值为裁判系统的输入功率Input_Power，这里可以改进！！！！！！！！！！*/
		pid_func.power_calc(&gPID_AllIoutLoop,Limit_Power,Input_Power);	// 该函数会修改gPID_AllIoutLoop.out
		
		
		
		//将功率环输出输入到电流环中
		gPID_AllIoutLoop.out = (2770-gPID_AllIoutLoop.out);//电源输出端正反馈过程
		
		IoRefApp=gPID_AllIoutLoop.out;
		pid_func.calc(&gPID_OutPowerCurr, IoRefApp, ADC_I_SCAP_SampleValue);//电流环PID计算gAdcIoSampleValue
		gPID_OutPowerCurr.out=gPID_OutPowerCurr.max_out-gPID_OutPowerCurr.out;
		PWM_DUTY=gPID_OutPowerCurr.out;
	}	
	else if(Direction==0 && Transition_Mode==0)	//超级电容充电模式: 单环，在
	{
		Mode = 0;
		switch(loop_mode)
		{
			case 0x00: //电压环
				//动态限幅
				gPID_VoltOutLoop.min_out  = 6.56f*ADC_V_SCAP_SampleValue - 27;//最大占空比
			
				pid_func.calc( &gPID_VoltOutLoop, Vref, ADC_V_SCAP_SampleValue);//超级电容充电电压环PID计算			
				PWM_DUTY = gPID_VoltOutLoop.out;
				gPID_CurrOutLoop.out = PWM_DUTY;
  			loop_mode = (ADC_I_SCAP_SampleValue > 3100 ) ? 0x01 : 0x00;//如果超级电容电流大于3.3A，说明此时充电充的很猛，切换到电流环
				break;
			case 0x01:   //功率环+电流环
				//动态限幅
				gPID_CurrOutLoop.min_out  = 6.56f*ADC_V_SCAP_SampleValue - 27;//最小占空比
			
				//功率环
				//如果超过Limit_Power-5则以最大限定值作为功率限制值
				if(Input_Power>Limit_Power-5) // 超功率时，先减到功率限制，再减到功率限制-5
				{
					pid_func.calc(&gPID_PowerOutLoop,Limit_Power,Input_Power);
				}
				else
				{
					pid_func.calc(&gPID_PowerOutLoop,Limit_Power-5,Input_Power);
				}
				//电流环
				pid_func.calc( &gPID_CurrOutLoop, gPID_PowerOutLoop.out , ADC_I_SCAP_SampleValue);	//输出电流环PID计算	
				PWM_DUTY = gPID_CurrOutLoop.out;
				gPID_VoltOutLoop.out = PWM_DUTY;
				loop_mode = (ADC_V_SCAP_SampleValue > Vref) ? 0x00 : 0x01;//如果超级电容电压大于22.36V，说明此时接近充满，切换到电压环
				break;
			default :
				loop_mode = 0x01;
				break;
		}
	}
	PWM_Update(PWM_DUTY);	//更新PWM占空比	
}


