

#include "timer.h"
#include "GPIO.h"
#include "current_output.h"
#include "data_input.h"
#include "IORedefine.h"
#include "data_save.h"

u16 data pwm_high;
u16 data pwm_low;

void PWMIO_Config()
{
    GPIO_InitTypeDef	GPIO_InitStructure;
    GPIO_InitStructure.Pin  = GPIO_Pin_5;
    GPIO_InitStructure.Mode = GPIO_OUT_PP;//推挽输出
    GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);
    
}
void Timer0_Config()
{
    TIM_InitTypeDef TIM_InitStructure;
    TIM_InitStructure.TIM_Mode = TIM_16BitAutoReload;
    TIM_InitStructure.TIM_Polity = PolityHigh;
    TIM_InitStructure.TIM_Interrupt = ENABLE;
    TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;
    TIM_InitStructure.TIM_ClkOut =  ENABLE;
    TIM_InitStructure.TIM_Value  = 65536UL - PWM_HIGH_MIN;
    TIM_InitStructure.TIM_Run    = ENABLE;
    Timer_Inilize(Timer0,&TIM_InitStructure);
    EA = 1;

}



void PWM_Update()
{                        
    u16 pwm_cnt_tmp;
   
//    pwm_cnt_tmp = (u16)(((pData_Acquire->current_output) / 1000) * pSystemParam->res_output * PWM_DUTY / pData_Voltage->voltage_vcc);
    pwm_cnt_tmp = ((pData_Acquire->current_output/1.0) - 4.0)*(float)(pSystemParam->cal_high-pSystemParam->cal_low)/16.0 + pSystemParam->cal_low;
    
    if(pwm_cnt_tmp > PWM_HIGH_MAX)
    {
        pwm_cnt_tmp = PWM_HIGH_MAX;
    }
    if(pwm_cnt_tmp < PWM_HIGH_MIN)
    {
        pwm_cnt_tmp = PWM_HIGH_MIN;
    }
    
    EA = 0;
    pwm_low = 65536UL - (PWM_DUTY - pwm_cnt_tmp);
    pwm_high = 65536UL - pwm_cnt_tmp;                     
    EA = 1;

}

void PWM_Cal(u16 pwmcnt_cal)
{
    if(pwmcnt_cal > PWM_HIGH_MAX)
    {
        pwmcnt_cal = PWM_HIGH_MAX;
    }
    if(pwmcnt_cal < PWM_HIGH_MIN)
    {
        pwmcnt_cal = PWM_HIGH_MIN;
    }
    
    EA = 0;
    pwm_low = 65536UL - (PWM_DUTY - pwmcnt_cal);
    pwm_high = 65536UL - pwmcnt_cal;
    EA = 1;
}

/********************* Timer0中断函数************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
    if(P_PWM)
    {
        TH0 = (u8)(pwm_low>>8);
        TL0 = (u8)pwm_low;
    }
    else
    {
        TH0 = (u8)(pwm_high>>8);
        TL0 = (u8)pwm_high;
    }

}




