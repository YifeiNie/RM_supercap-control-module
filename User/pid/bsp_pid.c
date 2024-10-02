/**
  ******************************************************************************
  * @author  何。小P
  * @version V1.1
  * @date    2017-08-22
  * @brief   桂林安合科技有限公司 版权所有（C）All rights reserved.
  ******************************************************************************
	* 
	*	文件名：dp_pid_lib.c
	*	功  能：PID位置式算法
  *
  ******************************************************************************
**/ 

#include "bsp_pid.h"


#define LimitMax(input, max) \
    {                         \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }
 
#define LimitMin(input,min) {if (input < min){ input = min;}}		

extern void pid_reset( PID_STRUCT *p);
extern void pid_init ( PID_STRUCT *pid);
extern void pid_clc  ( PID_STRUCT *p);
extern void pid_calc ( PID_STRUCT *p, float fb, float ref);
extern void pid_power_calc (PID_STRUCT *pid, float fb, float ref);

PID_FUNC pid_func = {
	.reset	= pid_reset,
	.init 	= pid_init ,
	.clc  	= pid_clc  ,
	.calc 	= pid_calc ,
	.power_calc	= pid_power_calc
};


/**
 **********************************************************
 * @描述：PID数据复位
 * @输入：*p：定义的PID数据
 * @返回：无 
 **********************************************************
 */
void pid_reset( PID_STRUCT	*p)
{
	p->Kp			= 0;
	p->Ki			= 0;
	p->Kd			= 0;

	p->max_out 	= 0;     //  Error[k-0]
	p->min_out		= 0;     //  Error[k-1]
	p->max_iout		= 0;     //  Error[k-2]

	p->out			= 0;		 //周期
	p->Pout			= 0;     //比例系数P
	p->Iout			= 0;     //积分时间I
	p->Dout			= 0;     //微分时间D
	

}
/**
 **********************************************************
 * @描述：PID参数计算
 * @输入：*p：定义的PID数据
 * @返回：无 
 **********************************************************
 */
void pid_init ( PID_STRUCT *pid)
{   
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->error[0] = pid->error[1] = pid->error[2] = pid->Pout = pid->Iout = pid->Dout = pid->out = 0.0f;

}
/**
 **********************************************************
 * @描述：清除PID计算中的缓存
 * @输入：*p：定义的PID数据
 * @返回：无 
 **********************************************************
 */
void pid_clc( PID_STRUCT	*pid)
{
	pid->error[0] = pid->error[1] = pid->error[2] = 0.0f;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->out = pid->Pout = pid->Iout = pid->Dout = 0.0f;
    pid->fdb = pid->set = 0.0f;
}
/**
 **********************************************************
 * @描述：PID增量式计算
 * @输入：*p：PID结构体
 * @返回：PID运算结果	 
 **********************************************************
 */
void pid_calc( PID_STRUCT *pid, float ref, float fdb)
{
	pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->set = ref;
    pid->fdb = fdb;
    pid->error[0] = ref - fdb;
	
	  pid->Pout = pid->Kp * pid->error[0];
    pid->Iout += pid->Ki * pid->error[0];
    pid->Dbuf[2] = pid->Dbuf[1];
    pid->Dbuf[1] = pid->Dbuf[0];
    pid->Dbuf[0] = (pid->error[0] - pid->error[1]);
    pid->Dout = pid->Kd * pid->Dbuf[0];
    LimitMax(pid->Iout,pid->max_iout);
    pid->out = pid->Pout + pid->Iout + pid->Dout;
    LimitMax(pid->out,pid->max_out);
 	  LimitMin(pid->out,pid->min_out);
}

void pid_power_calc (PID_STRUCT *pid, float fdb, float ref)
{	
	pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->set = fdb;
    pid->fdb = ref;
    pid->error[0] = ref - fdb;
	
	 pid->Pout = pid->Kp * pid->error[0];
     pid->Iout += pid->Ki * pid->error[0];
     pid->Dbuf[2] = pid->Dbuf[1];
     pid->Dbuf[1] = pid->Dbuf[0];
     pid->Dbuf[0] = (pid->error[0] - pid->error[1]);
     pid->Dout = pid->Kd * pid->Dbuf[0];
     LimitMax(pid->Iout,pid->max_iout);
     pid->out = pid->Pout + pid->Iout + pid->Dout;
     LimitMax(pid->out,pid->max_out);
	 LimitMin(pid->out,pid->min_out);	
}

/**
 **********************************************************
 *[] END OF FILE 
 **********************************************************
**/



