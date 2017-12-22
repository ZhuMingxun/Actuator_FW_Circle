#include "math.h"
#include "IORedefine.h"
#include "timer.h"
#include "delay.h"
#include "Exti.h"
#include "data_input.h"
#include "motor.h"
#include "LCD.h"
#include "control_new.h"
#include "data_save.h"
#include "current_output.h"
#include "phase_detect.h"
#include "ir.h"
#include "GPIO.h"
#include "local.h"

#define TIMER2_20MS_COUNT       (20/T2_MS)
#define TIMER2_60MS_COUNT       (60/T2_MS)
#define BLINK_500MS_CNT         (500 / T2_MS)
#define TIMER2_100MS_COUNT      (100 / T2_MS)
#define TIMER2_200MS_COUNT      (200 / T2_MS)
#define TIMER2_400MS_COUNT      (400 / T2_MS)
#define TIMER_1S_COUNT          (1000/ T2_MS)


volatile Mode_Type mode;
u8 bdata Timer2_Flag;
sbit  timer2_60ms_flag =  Timer2_Flag^0;
sbit  timer2_100ms_flag = Timer2_Flag^1;
sbit  timer2_200ms_flag = Timer2_Flag^2;
sbit  timer2_400ms_flag = Timer2_Flag^3;
sbit  timer2_500ms_flag = Timer2_Flag^4;
sbit  timer_1s_flag = Timer2_Flag^5;
sbit  timer2_20ms_flag = Timer2_Flag^6; 


void Timer2_Config()
{
    TIM_InitTypeDef		TIM_InitStructure;
    TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;
    TIM_InitStructure.TIM_Interrupt = ENABLE;
    TIM_InitStructure.TIM_ClkOut    = DISABLE;
    TIM_InitStructure.TIM_Value     = T2_CNT;
    TIM_InitStructure.TIM_Run       = ENABLE;
    Timer_Inilize(Timer2,&TIM_InitStructure);
    EA = 1;
}

/********************* Timer2_Handler************************/
void timer2_int (void) interrupt TIMER2_VECTOR//Timer2 50ms
{
    static u8 timer2_20ms_cnt = 0;//50Hz
    static u8 timer2_60ms_cnt = 0;//20Hz
    static u8 timer2_100ms_cnt = 0;//10Hz
    static u8 timer2_200ms_cnt = 0;//5Hz
    static u8 timer2_400ms_cnt = 0;//2.5Hz
    static u8 timer2_500ms_cnt = 0;//2Hz 
    static u16 timer_1s_cnt = 0;
    
    timer2_20ms_cnt++;
    timer2_60ms_cnt++;
    timer2_100ms_cnt++;
    timer2_200ms_cnt++;
    timer2_400ms_cnt++;
    timer2_500ms_cnt++;
    timer_1s_cnt++;
    
    if(timer2_20ms_cnt>=TIMER2_20MS_COUNT)
    {
        timer2_20ms_flag = 1;
        timer2_20ms_cnt=0;  
    }
    
    if(timer2_60ms_cnt>=TIMER2_60MS_COUNT)
    {
        timer2_60ms_flag = 1;
        timer2_60ms_cnt = 0;
    }
    if(timer2_100ms_cnt>=TIMER2_100MS_COUNT)
    {
        timer2_100ms_flag = 1;
        timer2_100ms_cnt = 0;
    }
    if(timer2_200ms_cnt>=TIMER2_200MS_COUNT)
    {
        timer2_200ms_flag = 1;
        timer2_200ms_cnt = 0;
    }
    if(timer2_400ms_cnt>=TIMER2_400MS_COUNT)
    {
        timer2_400ms_cnt = 0;
        timer2_400ms_flag = 1;
    }
    if(timer2_500ms_cnt >= BLINK_500MS_CNT)//500ms
    {
        timer2_500ms_flag = 1;
        timer2_500ms_cnt = 0;
    }
    
    if(timer_1s_cnt>= TIMER_1S_COUNT)
    {
        timer_1s_flag = 1;
        timer_1s_cnt = 0;
    }
        
    
            
}


