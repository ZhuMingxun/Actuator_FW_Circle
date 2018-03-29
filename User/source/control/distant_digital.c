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


/* 远方开关控制器状态 停、开、关 */
#define RMTKEYCTR_STAT_STOP     0
#define RMTKEYCTR_STAT_OPEN     1
#define RMTKEYCTR_STAT_CLOSE    2

static u8 keyctr_stat = RMTKEYCTR_STAT_STOP;

volatile DistantCmd_Type distant_cmd = DISTANT_CMD_STOP;
//u8 distant_open_trigger_flag = 0;//远方开触发标志
//u8 distant_close_trigger_flag = 0;//远方关触发标志

u8 Remote_KeyScan();
void Remote_KeyControl();

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
    ERR_OFF
    keyctr_stat = RMTKEYCTR_STAT_STOP;
    Distant_PCA_Config(); 
    IR_Disable();
    LCD_YELLOW_ON
  
}
void DistantDigital_Mode()
{
    DistantDigital_Init();
    
    while(mode==MODE_DISTANT_DIGITAL)
    {
        
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif
        
        #ifdef TRAVEL_PROTECT_MACHINE
            TravelProtect_Machine();
        #else
            TravelProtect();
        #endif
        
        if(timer2_20ms_flag == 1)
        {
            timer2_20ms_flag = 0;
            Acquire_Data();
            Update_InputCurrent();
        }
        
        if(timer2_60ms_flag == 1)
        {
            timer2_60ms_flag = 0;
            MotorStop_Delay_60ms();
        }
        
        if(timer2_100ms_flag == 1)
        {
            timer2_100ms_flag = 0;

            Torque_Detect();//力矩检测
            LimitPosition_Output();
            Update_InputCurrent(); 
            Remote_KeyControl();
            LCD_DIS();
            PWM_Update();    
        }
        
        if(timer2_200ms_flag == 1)
        {
            timer2_200ms_flag = 0; 
            SystemMode_Detect();//远方现场模式检测
            if(mode == MODE_LOCAL || mode == MODE_DISTANT_ANALOG)   
                break;

            #ifdef PHASE_SEQ
            PhaseSeq_Update();
            if(mode == MODE_LACK_PHASE)    
                break;      
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
            //printf("  \r\n -Distant Digital- ");
        }
    } 
}

//===========================改进按键控制2018.3.28==============================
/* 按键值宏定义 - 保持 */
#define RMTKEY_KEEP_ERR     (0x00)//开关同时按下
#define RMTKEY_KEEP_OPEN    (0x01)
#define RMTKEY_KEEP_CLOSE   (0x02)
#define RMTKEY_KEEP_NONE    (0x03)
/* 按键值宏定义 - 点动 */
#define RMTKEY_INCHING_ERR      (0x04)//开关同时按下
#define RMTKEY_INCHING_OPEN     (0x05)
#define RMTKEY_INCHING_CLOSE    (0x06)
#define RMTKEY_INCHING_NONE     (0x07)

/*远方开关量扫描检测
*返回值：按键值
*/
u8 Remote_KeyScan()
{
    u8 keyval = 0xff;
    u8 keyval_delay = 0xff;
    u8 reval = 0xff;
    
    keyval = ((u8)PIN_DISTANT_STOP<<2)|((u8)PIN_DISTANT_OPEN<<1)|((u8)PIN_DISTANT_CLOSE);
    keyval &= 0x07; //保留低3位
    delay_ms(20);
    keyval_delay = ((u8)PIN_DISTANT_STOP<<2)|((u8)PIN_DISTANT_OPEN<<1)|((u8)PIN_DISTANT_CLOSE);
    keyval_delay &= 0x07; //保留低3位
    
    if( keyval == keyval_delay )
    {
        reval = keyval;
    }
    else
    {
        reval = 0xff;
    }

    return reval;    

}



/* 保持状态机
*参数1：按键值
*返回值：控制状态
*/
u8 RemoteKey_State_Keep(u8 value)
{
    u8 keyval;
    
    keyval = value;
    
    switch(keyctr_stat)
    {
        case RMTKEYCTR_STAT_STOP:
        {
            if( keyval == RMTKEY_KEEP_OPEN )
            {
                keyctr_stat = RMTKEYCTR_STAT_OPEN;
            }
            else if( keyval == RMTKEY_KEEP_CLOSE )
            {
                keyctr_stat = RMTKEYCTR_STAT_CLOSE;
            }
            else
            {
                keyctr_stat = RMTKEYCTR_STAT_STOP;
            }
            
            break;
        }
        
        case RMTKEYCTR_STAT_OPEN:
        {
            if( (keyval==RMTKEY_KEEP_CLOSE) || (keyval==RMTKEY_KEEP_ERR) )
            {
                keyctr_stat = RMTKEYCTR_STAT_STOP;
            }
            break;
        }
        
        case RMTKEYCTR_STAT_CLOSE:
        {
            if( (keyval==RMTKEY_KEEP_OPEN) || (keyval==RMTKEY_KEEP_ERR))
            {
                keyctr_stat = RMTKEYCTR_STAT_STOP;
            }
            break;
        }
        
        default:break;
    }
    
    return keyctr_stat; 
}

/* 点动状态机 
参数1：按键值
返回值：控制状态
*/
u8 RemoteKey_State_Inching(u8 value)
{
    u8 keyval;
    
    keyval = value;
    
    switch(keyctr_stat)
    {
        case RMTKEYCTR_STAT_STOP:
        {
            if( keyval == RMTKEY_INCHING_OPEN )
            {
                keyctr_stat = RMTKEYCTR_STAT_OPEN;
            }
            else if( keyval == RMTKEY_INCHING_CLOSE )
            {
                keyctr_stat = RMTKEYCTR_STAT_CLOSE;
            }
            else
            {
                keyctr_stat = RMTKEYCTR_STAT_STOP;
            }
            
            break;
        }
        
        case RMTKEYCTR_STAT_OPEN:
        {
            if( keyval != RMTKEY_INCHING_OPEN )
            {
                keyctr_stat = RMTKEYCTR_STAT_STOP;
            }
            break;
        }
        
        case RMTKEYCTR_STAT_CLOSE:
        {
            if( keyval != RMTKEY_INCHING_CLOSE )
            {
                keyctr_stat = RMTKEYCTR_STAT_STOP;
            }
            break;
        }
        
        default:break;
    }
    
    return keyctr_stat;
 
}


/* 远方开关量控制-驱动 
*参数1:控制器状态
*/
void Remote_KeyControl_Drive(u8 ctrstat)
{
    if(ctrstat==RMTKEYCTR_STAT_OPEN)
    {
        MotorOpen_Control();
    }
    else if(ctrstat==RMTKEYCTR_STAT_CLOSE)
    {
        MotorClose_Control();
    }
    else
    {
        Motor_Stop();
    }
    
}

/* 远方开关量控制 */
void Remote_KeyControl()
{
    u8 rmt_keyval;//按键检测值
    u8 rmt_sw_mod;///远方开关模式，点动/保持
    u8 ctr_stat = RMTKEYCTR_STAT_STOP;//控制器状态
    
    rmt_keyval = Remote_KeyScan();//扫描按键值
    
    //printf("\r\n rmt_keyval=%d",(int)rmt_keyval);
    
    if(rmt_keyval==0xff)//无效值
        return;
    
    /* 得到远方开关模式，点动/保持 */
    rmt_sw_mod = (rmt_keyval>>2)&0x01;
    if(rmt_sw_mod)
    /* 点动状态控制 */
    {  
        ctr_stat = RemoteKey_State_Inching(rmt_keyval);
        //printf("\r\n Inching ctr_stat=%d",(int)ctr_stat);        
    }
    else
    /* 保持状态控制 */
    {
        ctr_stat = RemoteKey_State_Keep(rmt_keyval);
        //printf("\r\n Keep ctr_stat=%d",(int)ctr_stat);
    }
    
    /* 底层驱动 */
    Remote_KeyControl_Drive(ctr_stat);
 
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
//void DistantDigital_Control2()
//{
//	if(PIN_LOSE_OPEN & PIN_LOSE_CLOSE)//丢信保持
//	{
//		if(PIN_DISTANT_STOP==0)//自锁
//		{
//			if(distant_open_trigger_flag==1 && distant_close_trigger_flag==0)
//			{
//				distant_cmd = DISTANT_CMD_OPEN;
//			}
//			else if(distant_open_trigger_flag==0 && distant_close_trigger_flag==1)
//			{
//				distant_cmd = DISTANT_CMD_CLOSE;
//			}
//			else
//			{
//				distant_cmd = DISTANT_CMD_STOP;
//			}		

//		}
//		else//点动
//		{
//			/* 清空自锁标志 */
//			distant_open_trigger_flag = 0;
//			distant_close_trigger_flag = 0;	
//			
//			if(PIN_DISTANT_OPEN==0 && PIN_DISTANT_CLOSE==1)
//			{
//				delay_ms(10);//延时抗干扰
//				if(PIN_DISTANT_OPEN==0 && PIN_DISTANT_CLOSE==1)
//					distant_cmd = DISTANT_CMD_OPEN;
//			}
//			else if(PIN_DISTANT_OPEN==1 && PIN_DISTANT_CLOSE==0)
//			{
//				delay_ms(10);//延时抗干扰
//				if(PIN_DISTANT_OPEN==1 && PIN_DISTANT_CLOSE==0)
//					distant_cmd = DISTANT_CMD_CLOSE;
//			}
//			else
//			{
//				distant_cmd = DISTANT_CMD_STOP;
//			}
//			
//		}

//	
//	}
//	else//丢信开，丢信关
//	{
//        if(PIN_LOSE_OPEN == 0)  distant_cmd = DISTANT_CMD_OPEN;
//        if(PIN_LOSE_CLOSE == 0) distant_cmd = DISTANT_CMD_CLOSE;  
//	}
//	
//	/* 执行控制命令 */
//    switch(distant_cmd)
//    {
//        case DISTANT_CMD_STOP:  Motor_Stop();break;
//        case DISTANT_CMD_OPEN:  MotorOpen_Control();break;
//        case DISTANT_CMD_CLOSE: MotorClose_Control();break;
//        default:break;
//    }  
//}




