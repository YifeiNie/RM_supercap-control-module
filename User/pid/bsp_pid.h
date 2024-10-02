/**
  ******************************************************************************
  * @author  何。小P
  * @version V1.0
  * @date    2017-08-22
  * @brief   桂林安合科技有限公司 版权所有（C）All rights reserved.
  ******************************************************************************
	* 
	*	文件名：dp_pid.h
	*	功  能：PID算法头文件
  *
  ******************************************************************************
**/

#ifndef __BSP_PID_H
#define __BSP_PID_H

typedef struct  _PID{ 
	
	  //PID 三参数
    float Kp;
    float Ki;
    float Kd;

    float max_out;  //最大输出
	  float min_out;	//最小输出
    float max_iout; //最大积分输出

    float set;			//目标值
    float fdb;			//当前值

    float out;
    float Pout;     //比例控制量
    float Iout;			//积分控制量
    float Dout;			//微分控制量
    float Dbuf[3];  //微分项 0最新 1上一次 2上上次
    float error[3]; //误差项 0最新 1上一次 2上上次
}PID_STRUCT;


typedef struct _PID_FUNC{

	void (*reset) ( PID_STRUCT *pid);
	void (*init ) ( PID_STRUCT *pid);
	void (*clc  ) ( PID_STRUCT *pid);
	void (*calc ) ( PID_STRUCT *pid, float ref, float fdb);
	void (*power_calc)	(PID_STRUCT *pid, float ref, float fdb);

}PID_FUNC;


extern PID_FUNC pid_func;

#endif


