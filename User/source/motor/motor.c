/*********************************************************
ÎÄ¼þÃû³Æ£ºMotor.c
×÷Õß£º    ÖìÃ÷Ñ«
ÃèÊö£º    ÓÃÓÚµç»ú¿ØÖÆ £¬Ö÷Òª¿ØÖÆÁ½¸ö½Ó´¥Æ÷µÄÍ¨¶Ï
Ö÷Òªº¯Êý£º 
ÆäËûËµÃ÷£º
********************************************************/

#include "IORedefine.h"
#include "GPIO.h"
#include "delay.h"
#include "motor.h"
#include "phase_detect.h"
#include "control_new.h"
#include "LCD.h"
#include "data_input.h"
#include "local.h"
#include "distant_digital.h"
#include <math.h>
#include <stdio.h>
#include "uart_debug.h"

static bit m_pos_rev = 1;

#define RELAY_ON            0
#define RELAY_OFF           1
#define M_OPEN              {RELAY_A = (RELAY_ON ^ phase_seq ^ m_pos_rev);RELAY_B = !RELAY_A;}
#define M_CLOSE             {RELAY_B = (RELAY_ON ^ phase_seq ^ m_pos_rev);RELAY_A = !RELAY_B;}
#define M_STOP              {RELAY_A = RELAY_OFF;RELAY_B = RELAY_OFF;}
#define MOTOR_DELAY_MS      2000 //2s
#define TORQUE_DELAY_MS     3000 //3s
#define MOTOR_DELAY_CNT     (MOTOR_DELAY_MS / T2_MS)
#define TORQUE_DELAY_CNT    (TORQUE_DELAY_MS / T2_MS)
#define ALARM_TIME_MS       10000 //10s
#define ALARM_TIME_CNT      (ALARM_TIME_MS / T2_MS)

#define MOTORERR_MASK       (1<<6)
#define TORQUE_OPEN_MASK    (1<<3)
#define TORQUE_CLOSE_MASK   (1<<2)

volatile  MotorStatus_Type  motor_status = MOTOR_STATUS_STOP;
static u16 data stop_cnt;
//================================================================
//=========================Motor_CR===============================
//		Bit7		Bit6		Bit5		Bit4		Bit3		Bit2		Bit1		Bit0
//		delay		err			almop		almcl		torop		torcl		op100		cl0
//================================================================
u8 bdata Motor_CR;
sbit  motordelay_flag   = Motor_CR^7;
sbit  motorerr_flag     = Motor_CR^6;
sbit  alarm_opnflag     = Motor_CR^5;
sbit  alarm_cleflag     = Motor_CR^4;
sbit  torquelock_open_flag  = Motor_CR^3;
sbit  torquelock_close_flag = Motor_CR^2;
sbit  open100_flag  = Motor_CR^1;
sbit  close0_flag   = Motor_CR^0;  
#define _SET_CLOSE_IN_PLACE_FLAG	{Motor_CR=Motor_CR|(1<<0);Motor_CR=Motor_CR&(~(1<<1));}
#define _SET_OPEN_IN_PLACE_FLAG		{Motor_CR=Motor_CR|(1<<1);Motor_CR=Motor_CR&(~(1<<0));}
#define _CLEAR_IN_PLACE_FLAG			{Motor_CR=Motor_CR&(~(0x03));}
static void MotorIO_Config()
{
    GPIO_InitTypeDef	GPIO_InitStructure;
    GPIO_InitStructure.Pin  = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.Mode = GPIO_OUT_PP;
    GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);
    
}

/*******************åˆ°ä½è¾“å‡ºç«¯å£é…ç½®*********************/
static void LimitIO_Config()
{
    GPIO_InitTypeDef	GPIO_InitStructure;
    
    GPIO_InitStructure.Pin  = GPIO_Pin_7 | GPIO_Pin_6;
    GPIO_InitStructure.Mode = GPIO_OUT_PP;
    GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);
    PIN_FLAG_LIMCLE_OFF
    PIN_FLAG_LIMOPN_OFF
    
}

void Motor_Config()
{
    MotorIO_Config();
    LimitIO_Config();
}

extern u8 distant_open_trigger_flag;//
extern u8 distant_close_trigger_flag;//
void Motor_Stop()
{
    if(motor_status != MOTOR_STATUS_STOP)
    {
        motor_status = MOTOR_STATUS_STOP;
        M_STOP
        stop_cnt = 0;
        motordelay_flag = 0;
		distant_open_trigger_flag = 0;
		distant_close_trigger_flag = 0;	
    }  
}

static void Motor_Open()
{
    if(motor_status != MOTOR_STATUS_OPEN)
    {   
        motor_status = MOTOR_STATUS_OPEN;
        M_OPEN

    }
}

static void Motor_Close()
{
    if(motor_status != MOTOR_STATUS_CLOSE)
    {
        motor_status = MOTOR_STATUS_CLOSE;
        M_CLOSE
    }
    
}

void MotorOpen_Control()
{
    switch(motor_status)
    {
        case MOTOR_STATUS_CLOSE:    {Motor_Stop();break;}
        case MOTOR_STATUS_OPEN:     break;
        case MOTOR_STATUS_STOP:
        {
            if(torquelock_open_flag != 1 && open100_flag !=1 && PIN_LIM_OPN != LIM_ON && motorerr_flag !=1 )
            { 
                if(motordelay_flag == 1)  
                {           
                    Motor_Open();
                    if(torquelock_close_flag == 1)  torquelock_close_flag = 0;
                    if(close0_flag==1)              close0_flag = 0;
                }
            }
            break;             
        }
        default:break; 
    }        
}

void MotorClose_Control()
{
    switch(motor_status)
    {
        case MOTOR_STATUS_CLOSE:    break;
        case MOTOR_STATUS_OPEN:     {Motor_Stop();break;}
        case MOTOR_STATUS_STOP:
        {
            if(torquelock_close_flag != 1 && close0_flag !=1 && PIN_LIM_CLE != LIM_ON && motorerr_flag !=1)//
            {
                if(motordelay_flag == 1)    
                {
                    Motor_Close(); 
                    if(torquelock_open_flag==1)     torquelock_open_flag = 0;
                    if(open100_flag==1)             open100_flag = 0;
                }
            }
            break;              
        }
        default:break;
 
    }    
}

//ç”µæœºåœæ­¢ä¿æŠ¤å»¶æ—¶ æ‰§è¡Œå‘¨æœŸï¼š50ms/20Hz
void MotorStop_Delay_60ms()
{   
    if(motor_status == MOTOR_STATUS_STOP)
    {
        stop_cnt++;
        if(stop_cnt >= 34)
        {
            if(torquelock_open_flag == 1 || torquelock_close_flag == 1)
            {
                if(stop_cnt >= 50)
                {
                    motordelay_flag = 1;
                    stop_cnt = 0;   
                }
                    
            }
            else
            {
                motordelay_flag = 1;
                stop_cnt = 0;
            }
        }
 
    }   
    
}

/* è¡Œç¨‹ä¿æŠ¤å‡½æ•° */
void TravelProtect()
{
    
    float opening = pData_Acquire->opening;
    
    switch(motor_status)
    {
        case MOTOR_STATUS_CLOSE:
        {
            if(PIN_LIM_CLE == LIM_ON || opening<=0.0)
            { 
				local_cmd = LOCAL_CMD_STOP;
				distant_cmd = DISTANT_CMD_STOP;
                
                if(opening==0.0)
                {
                    //printf("open travel!! opening=0.0 \r\n");
					_SET_CLOSE_IN_PLACE_FLAG
                }
                Motor_Stop();
                
                if(opening<=0.0)
                {
                    local_stat =  LOCAL_STAT_STOP;
                    distant_open_trigger_flag = 0;
                    distant_close_trigger_flag = 0;
                    
                }
            }
            break;
        }            
        case MOTOR_STATUS_OPEN:
        {
            if(PIN_LIM_OPN == LIM_ON || opening>=1.0)
            {
				local_cmd = LOCAL_CMD_STOP;
				distant_cmd = DISTANT_CMD_STOP;
                //printf("close travel!! \r\n");
                if(opening==1.0)
                {
					_SET_OPEN_IN_PLACE_FLAG
                    //printf("close travel!! opening=1.0 \r\n");
                }
                Motor_Stop();
                
                if(opening>=1.0)
                {
                    local_stat =  LOCAL_STAT_STOP;
                    distant_open_trigger_flag = 0;
                    distant_close_trigger_flag = 0;
                }
            }
            break;
        }
                
//        case MOTOR_STATUS_STOP:
//        {
//            if( opening<=0.0)
//            {

//								_SET_CLOSE_IN_PLACE_FLAG							
//                //printf("close travel!! -- Motor_CR |= (u8)(1<<0) \r\n");
//            }
//            else if( opening>=1.0)
//            {
//                _SET_OPEN_IN_PLACE_FLAG
//               // printf("open travel!! -- Motor_CR |= (u8)(1<<1) \r\n");
//            }
//            else
//            {
//                _CLEAR_IN_PLACE_FLAG	
//            }
//            break;
//        }
        default:break;
    }
            
}
void TravelProtect_Machine()
{
    float opening = pData_Acquire->opening;
    
    switch(motor_status)
    {
        case MOTOR_STATUS_CLOSE:
        {
            if(PIN_LIM_CLE == LIM_ON)
            { 
                //local_stat =  LOCAL_STAT_STOP;
				local_cmd = LOCAL_CMD_STOP;
				distant_cmd = DISTANT_CMD_STOP;
//                distant_open_trigger_flag = 0;
//                distant_close_trigger_flag = 0;
                if(opening==0.0)
                {
                    //printf("open travel!! opening=0.0 \r\n");
					_SET_CLOSE_IN_PLACE_FLAG
                }
                Motor_Stop();//printf("open travel!! \r\n");
            }
            break;
        }            
        case MOTOR_STATUS_OPEN:
        {
            if(PIN_LIM_OPN == LIM_ON)
            {
                //local_stat =  LOCAL_STAT_STOP;
				local_cmd = LOCAL_CMD_STOP;
				distant_cmd = DISTANT_CMD_STOP;
//                distant_open_trigger_flag = 0;
//                distant_close_trigger_flag = 0;
                if(opening==1.0)
                {
					_SET_OPEN_IN_PLACE_FLAG
                    //printf("close travel!! opening=1.0 \r\n");
                }
                Motor_Stop();//printf("close travel!! \r\n");
            }
            break;
        }
                
//        case MOTOR_STATUS_STOP:
//        {
//            if( opening<=0.0)
//            {
//				_SET_CLOSE_IN_PLACE_FLAG							
//                //printf("close travel!! -- Motor_CR |= (u8)(1<<0) \r\n");
//            }
//            else if( opening>=1.0)
//            {
//                _SET_OPEN_IN_PLACE_FLAG
//               // printf("open travel!! -- Motor_CR |= (u8)(1<<1) \r\n");
//            }
//            else
//            {
//                _CLEAR_IN_PLACE_FLAG
//                
//            }
//            break;
//        }
        default:break;
    }
    
}

void TravelProtect_Def()
{
      
    switch(motor_status)
    {
        case MOTOR_STATUS_CLOSE:
        {
            if(PIN_LIM_CLE == LIM_ON)
            { 
                Motor_Stop();//printf("open travel!! \r\n");
            }
            break;
        }            
        case MOTOR_STATUS_OPEN:
        {
            if(PIN_LIM_OPN == LIM_ON)
            {
                Motor_Stop();//printf("close travel!! \r\n");
            }
            break;
        }
        default:break;
    }
            
}



/* åˆ°ä½è¾“å‡ºå‡½æ•° */
u8 LimitPosition_Output()
{
    if(pData_Acquire->opening >=1.0)  { PIN_FLAG_LIMOPN_ON return 1;}
    else if(pData_Acquire->opening <=0.0)  { PIN_FLAG_LIMCLE_ON return 0;}
        else { 
				if(pData_Acquire->opening<=0.975 && pData_Acquire->opening>=0.015)
				{
					PIN_FLAG_LIMOPN_OFF
					PIN_FLAG_LIMCLE_OFF
					return 2;
				}
				else
				{
					if(pData_Acquire->opening >=0.975)
						return 1;
						else
						return 0;

				}
                
             }   
}

/* åŠ›çŸ©ä¿æŠ¤æ£€æµ‹å‡½æ•° */
void Torque_Detect()
{
    switch(motor_status)
    {
        case MOTOR_STATUS_CLOSE:
        {
            if(PIN_TORQUE_CLOSE == TORQUE_ON && torquelock_close_flag == 0)
            {
                    torquelock_close_flag = 1;
                    Motor_Stop();
            }
            break;              
        }
        
        case MOTOR_STATUS_OPEN:
        {
            if(PIN_TORQUE_OPEN == TORQUE_ON && torquelock_open_flag == 0)
            {
                torquelock_open_flag = 1;
                Motor_Stop();
         
            }
            break;            
        }
		
        case MOTOR_STATUS_STOP:
        {
            if(PIN_TORQUE_CLOSE == TORQUE_ON && torquelock_close_flag == 0)
            {
                torquelock_close_flag = 1;
            }
            if(PIN_TORQUE_OPEN == TORQUE_ON && torquelock_open_flag == 0)
            {
                torquelock_open_flag = 1;
            }
            break;


        }            
        default:break;
    }

}

void CloseDir_Protect()
{
	static u16 value_pre;
	static int delta1,delta2,delta3;
	signed char closedir = pSystemParam->close_dir;
	

	switch(motor_status)
	{
		case MOTOR_STATUS_STOP:
		{
			delta1 = 0;
			delta2 = 0;
			delta3 = 0;

			break;
		}
		
		case MOTOR_STATUS_CLOSE:
		{

			delta1 = delta2;
			delta2 = delta3;
			delta3 = pData_ADC->adcvalue_encoder - value_pre;
			
			if(delta1>0 && delta2>0 && delta3>0)
			{
				closedir = 1;//
			}
			if(delta1<0 && delta2<0 && delta3<0)
			{
				closedir = -1;//
			}	

			break;
		}
		
		case MOTOR_STATUS_OPEN:
		{

			delta1 = delta2;
			delta2 = delta3;
			delta3 = pData_ADC->adcvalue_encoder - value_pre;
			
			if(delta1>0 && delta2>0 && delta3>0)
			{
				closedir = -1;//
			}
			if(delta1<0 && delta2<0 && delta3<0)
			{
				closedir = 1;//
			}

			break;
		}
		default:
		{
			delta1 = 0;
			delta2 = 0;
			delta3 = 0;
			break;
		}
	}
	
	if(closedir != pSystemParam->close_dir)
	{
		torquelock_close_flag = 1;
		Motor_Stop();
	}

	value_pre = pData_ADC->adcvalue_encoder;
	
}

/* ç”µä½å™¨æŠ¥è­¦æ£€æµ‹å‡½æ•° */
void MotorErr_Detect()
{
    static u8 pre_opening;
    static u8 motorerr_cnt = 0;
    u8 opening = (u8)(pData_Acquire->opening * 100 + 0.5);
    u16 current_out;
    u16 current_in;
    u16 err_tmp;
    u16 sen_tmp;
    
    switch(mode)
    {
        case MODE_LOCAL:
        {
            if(local_cmd != LOCAL_CMD_STOP  && PIN_ALARM == ALARM_ON)
            {
                if(opening == pre_opening)
                {
                    motorerr_cnt++;
                    if(motorerr_cnt>5) 
                    {
                        motorerr_cnt = 0;
                        Motor_Stop();
                        motorerr_flag = 1;
                    }
                }
                else    
                {
                    motorerr_cnt = 0;
                    motorerr_flag = 0;
                }
                
            }
            else
            {
                motorerr_cnt = 0;
                motorerr_flag = 0;
            } 
            pre_opening = opening;
            break;
        }
        case MODE_DISTANT_DIGITAL:
        {
            if(distant_cmd != DISTANT_CMD_STOP  && PIN_ALARM == ALARM_ON)
            {
                if(opening == pre_opening)
                {
                    motorerr_cnt++;
                    if(motorerr_cnt>5) 
                    {
                        motorerr_cnt = 0;
                        Motor_Stop();
                        motorerr_flag = 1;
                    }
                }
                else    
                {
                    motorerr_cnt = 0;
                    motorerr_flag = 0;
                }
                
            }
            else
            {
                motorerr_cnt = 0;
                motorerr_flag = 0;
            } 
            pre_opening = opening;
            break;      
        }
        
        case MODE_DISTANT_ANALOG:
        {
            current_out = (u16)(pData_Acquire->current_output * 1000 + 0.5);
            current_in = (u16)(pData_Acquire->current_input * 1000 + 0.5);
            err_tmp = abs((int)current_out - (int)current_in);//è®¡ç®—åå·®
            sen_tmp = pSystemParam->sen; //è¯»å–çµæ•åº¦

            if((err_tmp>sen_tmp) && PIN_ALARM == ALARM_ON)
            {
                if(opening == pre_opening)
                {
                    motorerr_cnt++;
                    if(motorerr_cnt>5) 
                    {
                        motorerr_cnt = 0;
                        Motor_Stop();
                        motorerr_flag = 1;
                    }
                }
                else    
                {
                    motorerr_cnt = 0;
                    motorerr_flag = 0;
                }
                
            }
            else
            {
                motorerr_cnt = 0;
                motorerr_flag = 0;
            } 
            pre_opening = opening;
            break;   
            
        }
        default:break;
    }
    
}

void Motor_PosRev()
{
    if(POS_REV)
    {
        delay_ms(20);
        if(POS_REV)
        {
            m_pos_rev = 1;
        }
    }
    else
    {
        delay_ms(20);
        if(!POS_REV)
        {
             m_pos_rev = 0;
        }              
    }
    
}

void IntegAlarm()
{
    if((Motor_CR & (MOTORERR_MASK | TORQUE_OPEN_MASK | TORQUE_CLOSE_MASK)) != 0x00)
    {
        ERR_OUT 
    }
    else
    {
        ERR_OFF
    }
    
}

void MotorVel_Detect()
{
    static u16 preadc;
    u16 curadc;
    static u8 first_flag = 1;
    float motorvel;
    
    curadc = pData_ADC->adcvalue_valve_filtered;
    
    if(first_flag == 1)
    {
        preadc = curadc;     
        first_flag = 0;
    }
    
    motorvel = (float)(((float)curadc - (float)preadc)/0.5f);
    preadc = curadc;
    pData_Acquire->motor_vel = motorvel;   
}


















