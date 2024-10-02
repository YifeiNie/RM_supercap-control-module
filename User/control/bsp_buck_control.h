#ifndef _DP_BUCK_H
#define _DP_BUCK_H
#include "stm32f30x.h"
#include "bsp_pid.h"
#include "bsp_filter.h"
#include "bsp_hrtim.h"
#include "math.h"

#define Voltage_Factor  0.01209f
#define Current_Factor_Pow  0.00322f
#define Current_Factor_Cap  0.01074f

extern uint8_t	Direction;
extern uint8_t  Transition_Mode;

void bsp_PIDCtrlInit(void);
void InputPower_Control(void);

#endif




