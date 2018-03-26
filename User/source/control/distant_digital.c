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
#include "distant_digital.h"
#include "mode_detect.h"
#include "local.h"
#include "uart_debug.h"
#include <stdio.h>
#include "actuator_config.h"
volatile DistantCmd_Type distant_cmd = DISTANT_CMD_STOP;
u8 distant_open_trigger_flag = 0;//远方开触发标志
u8 distant_close_trigger_flag = 0;//远方关触发标志

void Distant_PCA_Config()
{
    PCA_InitTypeDef		PCA_InitStructure;

    PCA_InitStructure.PCA_IoUse    = PCA_P24_P25_P26_P27;	//PCA_P12_P11_P10_P37, PCA_P34_P35_P36_P37, PCA_P24_P25_P26_P27
    PCA_InitStructure.PCA_Clock    = PCA_Clock_12T;		//PCA_Clock_1T, PCA_Clock_2T, PCA_Clock_4T, PCA_Clock_6T, PCA_Clock_8T, PCA_Clock_12T, PCA_Clock_Timer0_OF, PCA_Clock_ECI
    PCA_InitStructure.PCA_Polity   = PolityLow;		//PolityHigh,PolityLow
    PCA_InitStructure.PCA_Interrupt_Mode = ENABLE;		//
    PCA_Init(PCA_Counter,&PCA_InitStructure);

    PCA_InitStructure.PCA_Interrupt_Mode = PCA_Fall_Active | PCA_Rise_Active| ENABLE;//上下降沿触发
    PCA_InitStructure.PCA_Mode     = PCA_Mode_Capture;		//PCA_Mode_PWM, PCA_Mode_Capture, PCA_Mode_SoftTimer, PCA_Mode_HighPulseOutput
    PCA_InitStructure.PCA_Value    = 0;			//
    PCA_Init(PCA1,&PCA_InitStructure);
    PCA_Init(PCA2,&PCA_InitStructure);

    CR = 1;
    EA = 1;    
    
}

void DistantDigital_Init()
{
    LED_LSG_ON
    Exti23_Disable();
    Motor_Stop();
    distant_cmd = DISTANT_CMD_STOP;
    Motor_CR = 0x00;
    ERR_OFF
//    lock_flag = 0;
//    lock_cnt = 0;
	distant_open_trigger_flag = 0;
	distant_close_trigger_flag = 0;
    Distant_PCA_Config(); 
    IR_Disable();
    LCD_YELLOW_ON
  
}
void DistantDigital_Mode()
{
    DistantDigital_Init();
    while(mode==MODE_DISTANT_DIGITAL)
    {
       
        if(timer2_20ms_flag == 1)
        {
            timer2_20ms_flag = 0;
            Acquire_Data();
            printf(" -Distant Digital - \r\n");
        }
        
        if(timer2_60ms_flag == 1)
        {
            timer2_60ms_flag = 0;
            MotorStop_Delay_60ms();
        }
        
        if(timer2_100ms_flag == 1)
        {
            timer2_100ms_flag = 0;
            Update_InputCurrent();
            Torque_Detect();//力矩检测
        #ifdef TRAVEL_PROTECT_MACHINE
            TravelProtect_Machine();
        #else
            TravelProtect();
        #endif
            LimitPosition_Output();
            DistantDigital_Control2();
            LCD_DIS();
            PWM_Update();    
        }
        
        if(timer2_200ms_flag == 1)
        {
            timer2_200ms_flag = 0; 
            SystemMode_Detect();//远方现场模式检测
            if(mode == MODE_LOCAL || mode == MODE_DISTANT_ANALOG)   break;

            #ifdef PHASE_SEQ
            PhaseSeq_Update();
            if(mode == MODE_LACK_PHASE)    break;      
            #endif
            
        }

        if(timer2_500ms_flag == 1)
        {
            timer2_500ms_flag = 0;
            IntegAlarm();
            LED_MotorDIS_500ms();
            LCD_TorqueDIS_500ms();
            LCD_MotorErrDIS();
        }
        if(timer_1s_flag == 1)
        {
            timer_1s_flag = 0;
            MotorErr_Detect();
            Set_InputLowHigh_Detect();
        }
        
        
    }
     
}



/**************远方数字控制函数******************/
//void DistantDigital_Control()
//{
//    
//    if(PIN_LOSE_OPEN & PIN_LOSE_CLOSE)
//    {
//        if(distant_trigger_flag == 0)
//        {
//            distant_cmd = DISTANT_CMD_STOP;
//        }
//        if(PIN_DISTANT_STOP == 0)
//        {
//            if(distant_trigger_flag == 0)   distant_trigger_flag = 1;
//            distant_cmd = DISTANT_CMD_STOP;
//        }            
//    }
//    else
//    {
//        if(distant_trigger_flag != 0) distant_trigger_flag = 0;  
//        if(PIN_LOSE_OPEN == 0)  distant_cmd = DISTANT_CMD_OPEN;
//        if(PIN_LOSE_CLOSE == 0) distant_cmd = DISTANT_CMD_CLOSE;    
//    }
//         
//    switch(distant_cmd)
//    {
//        case DISTANT_CMD_STOP:  Motor_Stop();break;
//        case DISTANT_CMD_OPEN:  MotorOpen_Control();break;
//        case DISTANT_CMD_CLOSE: MotorClose_Control();break;
//        default:break;
//    }    
//                    
//}

//远方 点动，保持控制
void DistantDigital_Control2()
{
	if(PIN_LOSE_OPEN & PIN_LOSE_CLOSE)//丢信保持
	{
		if(PIN_DISTANT_STOP==0)//自锁
		{
			if(distant_open_trigger_flag==1 && distant_close_trigger_flag==0)
			{
				distant_cmd = DISTANT_CMD_OPEN;
			}
			else if(distant_open_trigger_flag==0 && distant_close_trigger_flag==1)
			{
				distant_cmd = DISTANT_CMD_CLOSE;
			}
			else
			{
				distant_cmd = DISTANT_CMD_STOP;
			}		

		}
		else//点动
		{
			/* 清空自锁标志 */
			distant_open_trigger_flag = 0;
			distant_close_trigger_flag = 0;	
			
			if(PIN_DISTANT_OPEN==0 && PIN_DISTANT_CLOSE==1)
			{
				delay_ms(10);//延时抗干扰
				if(PIN_DISTANT_OPEN==0 && PIN_DISTANT_CLOSE==1)
					distant_cmd = DISTANT_CMD_OPEN;
			}
			else if(PIN_DISTANT_OPEN==1 && PIN_DISTANT_CLOSE==0)
			{
				delay_ms(10);//延时抗干扰
				if(PIN_DISTANT_OPEN==1 && PIN_DISTANT_CLOSE==0)
					distant_cmd = DISTANT_CMD_CLOSE;
			}
			else
			{
				distant_cmd = DISTANT_CMD_STOP;
			}
			
		}

	
	}
	else//丢信开，丢信关
	{
        if(PIN_LOSE_OPEN == 0)  distant_cmd = DISTANT_CMD_OPEN;
        if(PIN_LOSE_CLOSE == 0) distant_cmd = DISTANT_CMD_CLOSE;  
	}

		
	
	
	
	
	/* 执行控制命令 */
    switch(distant_cmd)
    {
        case DISTANT_CMD_STOP:  Motor_Stop();break;
        case DISTANT_CMD_OPEN:  MotorOpen_Control();break;
        case DISTANT_CMD_CLOSE: MotorClose_Control();break;
        default:break;
    }  
}







