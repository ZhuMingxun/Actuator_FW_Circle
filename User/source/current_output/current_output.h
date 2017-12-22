

#ifndef _CURRENT_OUT_H_
#define _CURRENT_OUT_H_
#include "data_input.h"

#define PWM_DUTY      2500
#define PWM_HIGH_MIN  32
#define PWM_HIGH_MAX  (PWM_DUTY - PWM_HIGH_MIN)

void PWMIO_Config();
void    Timer0_Config();
void PWM_Update();
void PWM_Cal(u16 pwmcnt_cal);
#endif