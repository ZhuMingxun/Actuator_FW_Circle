#include "math.h"
#include "IORedefine.h"
#include "timer.h"
#include "delay.h"
#include "Exti.h"
#include "data_input.h"
#include "motor.h"
#include "LCD.h"
#include "control_new.h"
#include "mode_detect.h"
#include "data_save.h"
#include "current_output.h"
#include "phase_detect.h"
#include "ir.h"
#include "distant_analog.h"
#include "local.h"
#include "ringbuffer.h"
#include "USART.h"
#include "uart_debug.h"
#include <stdio.h>
#include "actuator_config.h"
#define LOCAL_LOCK_DELAY    40//50ms*40=2s
#define LOCAL_OPEN_ON       0
#define LOCAL_CLOSE_ON      0

volatile LocalCmd_Type local_cmd = LOCAL_CMD_STOP;
static u8 data local_lock_cnt = 0;
static u8 data local_lock_flag = 0;
static u8 data local_lock_startflag = 0;

u16  code sen_table[10] = {SEN0_05MA,SEN0_1MA,SEN0_15MA,SEN0_2MA,SEN0_25MA,SEN0_3MA,SEN0_35MA,SEN0_4MA,SEN0_45MA,SEN0_5MA};
const u16* p_sen = &sen_table[0];

void LocalCmdLock_Detect();
void LocalDef_Detect();


void Exti23_Config()
{
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Interrupt = ENABLE;
    Ext_Inilize(EXT_INT2,&EXTI_InitStructure);
    Ext_Inilize(EXT_INT3,&EXTI_InitStructure);
     
}

void Exti23_Disable()
{
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Interrupt = DISABLE;
    Ext_Inilize(EXT_INT2,&EXTI_InitStructure);
    Ext_Inilize(EXT_INT3,&EXTI_InitStructure);   
    
}

void LocalMode_Init()
{
    local_cmd = LOCAL_CMD_STOP;
    Motor_Stop();
    ERR_OFF
    LED_LSG_OFF 
    LCD_YELLOW_OFF    
    Exti23_Config();
    IR_PCA_Config();
    IR_RingBuffer_Init();
    ReadSystemPara();
    Acquire_Data();
    LCD_DIS();
    PWM_Update();
}


u8 KeyScan(u8 key)
{
	u8 reval = 0;
	
	switch(key)
	{
		case KEY_LOCAL:
		{
			if(LOCAL == LOCAL_ON && LOCAL_STOP != LOCAL_STOP_ON)
			{
				delay_ms(SCAN_DELAY_MS);
				if(LOCAL == LOCAL_ON && LOCAL_STOP != LOCAL_STOP_ON)
					reval = 1;
				
			}
			break;
		}
		
		case KEY_STOP:
		{
			if(LOCAL_STOP == LOCAL_STOP_ON && LOCAL != LOCAL_ON)
			{
				delay_ms(SCAN_DELAY_MS);
				if(LOCAL_STOP == LOCAL_STOP_ON && LOCAL != LOCAL_ON)
					reval = 1;
				
			}
			break;
		}
		
		case KEY_REMOTE:
		{
			if(LOCAL != LOCAL_ON && LOCAL_STOP != LOCAL_STOP_ON)
			{
				delay_ms(SCAN_RMTDELAY_MS);
				if(LOCAL != LOCAL_ON && LOCAL_STOP != LOCAL_STOP_ON)
					reval = 1;
				
			}
			
			break;
		}
		
		case KEY_OPEN:
		{
			if(LOCAL_OPEN == LOCAL_OPEN_ON && LOCAL_CLOSE != LOCAL_CLOSE_ON)
			{
				delay_ms(SCAN_DELAY_MS);
				if(LOCAL_OPEN == LOCAL_OPEN_ON && LOCAL_CLOSE != LOCAL_CLOSE_ON)
					reval = 1;
				
			}
			break;
		}
		
		case KEY_CLOSE:
		{
			if(LOCAL_CLOSE == LOCAL_CLOSE_ON && LOCAL_OPEN != LOCAL_OPEN_ON)
			{
				delay_ms(SCAN_DELAY_MS);
				if(LOCAL_CLOSE == LOCAL_CLOSE_ON && LOCAL_OPEN != LOCAL_OPEN_ON)
					reval = 1;
				
			}
			break;	
		}	
		
		default:break;

	}
	return reval;
	
	
}




void Local_Mode()
{ 
    LocalMode_Init();
    
    while( mode == MODE_LOCAL )
    { 
        #ifdef TRAVEL_PROTECT_MACHINE
            TravelProtect_Machine();
        #else
            TravelProtect();
        #endif
        
        if( IRisReadyToRead())
        {
           IR_Update();
           IRCmd_To_LocalCmd();
						Local_Control();					
        }
        
       
        if(timer2_20ms_flag == 1)
        {
            timer2_20ms_flag = 0;
            Acquire_Data();
					  CloseDir_Protect();
					  Local_Control();
				  printf(" -Local- \r\n");
        }
       
        
        if(timer2_60ms_flag == 1)
        {
            timer2_60ms_flag = 0;
            MotorStop_Delay_60ms();
            LocalCmdLock_Detect();
        }
        
        if(timer2_100ms_flag == 1)
        {
            timer2_100ms_flag = 0; 
            Update_InputCurrent();
            Torque_Detect();
            LimitPosition_Output();
            
            LCD_DIS();
            PWM_Update();  
        }

        if(timer2_200ms_flag == 1)
        {
            timer2_200ms_flag = 0;
            SystemMode_Detect();
            if(mode == MODE_DISTANT_ANALOG || mode == MODE_DISTANT_DIGITAL) break;
            
            LocalDef_Detect();
            if(mode == MODE_DEF_HUNDRED || mode == MODE_DEF_ZERO)   break;
            
            #ifdef PHASE_SEQ
            PhaseSeq_Update();//相序，缺相检测
            if(mode == MODE_LACK_PHASE) break;
            #endif 
      
        }
        
        if(timer2_500ms_flag == 1)
        {
            timer2_500ms_flag = 0;
            MotorVel_Detect();
            IntegAlarm();
            LED_MotorDIS_500ms();
            LCD_TorqueDIS_500ms();
            LCD_MotorErrDIS();
        }

        if(timer_1s_flag == 1)
        {
            timer_1s_flag = 0;
            MotorErr_Detect();
        }
  
    }
  
}

/**************现场控制函数******************/
void Local_Control()
{
    
    if(LOCAL_STOP == LOCAL_STOP_ON )
    {
        delay_ms(10);
        if(LOCAL_STOP == LOCAL_STOP_ON )
        {
            local_cmd = LOCAL_CMD_STOP;
        }
    }   
    switch(local_cmd)
    {
        case LOCAL_CMD_STOP:
        {
            Motor_Stop();
            break;
        }
        
        case LOCAL_CMD_OPEN:
        {
            MotorOpen_Control();
            break;
        }
        
        case LOCAL_CMD_CLOSE:
        {
            MotorClose_Control(); 
            break;
        }
        default:break;
    }
    
                 
}
//===========================================================================================
//===================================标定函数================================================
//===========================================================================================

signed char zero_flag = -1;//编码器过零标志,-1不过零，+1过零
//signed char close_dir = -1;//关方向指示，-1递减，+1递增


void CloseDir_Zero_Detect()
{
	static u16 value_pre;
	static int delta1,delta2,delta3;
	
	//判断关方向
	switch(local_cmd)
	{
		case LOCAL_CMD_STOP:
		{
			delta1 = 0;
			delta2 = 0;
			delta3 = 0;
			break;
		}
		
		case LOCAL_CMD_CLOSE:
		{
			delta1 = delta2;
			delta2 = delta3;
			delta3 = pData_ADC->adcvalue_encoder - value_pre;
			
			if(delta1>0 && delta2>0 && delta3>0)
			{
				pSystemParam->close_dir = 1;//关向递增
			}
			if(delta1<0 && delta2<0 && delta3<0)
			{
				pSystemParam->close_dir = -1;//关向递减
			}	
			
			break;
		}
		
		case LOCAL_CMD_OPEN:
		{
			delta1 = delta2;
			delta2 = delta3;
			delta3 = pData_ADC->adcvalue_encoder - value_pre;
			
			if(delta1>0 && delta2>0 && delta3>0)
			{
				pSystemParam->close_dir = -1;//关向递增
			}
			if(delta1<0 && delta2<0 && delta3<0)
			{
				pSystemParam->close_dir = 1;//关向递减
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
	
	//判断行程过零
	if((int)(pSystemParam->adcvalue_valvelow - pSystemParam->adcvalue_valvehigh)*(pSystemParam->close_dir)>0)
		zero_flag = -1;
	if((int)(pSystemParam->adcvalue_valvelow - pSystemParam->adcvalue_valvehigh)*(pSystemParam->close_dir)<0)
		zero_flag = 1;
	
	
	value_pre = pData_ADC->adcvalue_encoder;
		
	
}




void DefZero_Mode()
{
	  u8 zeroflag = 0;
    u16 u16_data_tmp = 0;
    static u8 exit_cnt = 15;
    u8 stopcnt_enable_flag = 0;//退出使能，检测到现场时，使能之后的停止档计数
    u8 localexit_enable_flag = 0;//检测现场退出使能，置1后现场可退出
    Def0_DIS();
    Motor_CR = 0;
    delay_ms(500);
    delay_ms(500);
    
    while(mode == MODE_DEF_ZERO)
    {
        
        if(IRisReadyToRead())
        {
           IR_Update(); 
        }
        if(timer2_20ms_flag == 1)
        {
            timer2_20ms_flag = 0;
            Acquire_Data();
			CloseDir_Zero_Detect();

        }
        
        if(timer2_60ms_flag == 1)
        {
            timer2_60ms_flag = 0;
            MotorStop_Delay_60ms();
            LocalCmdLock_Detect();
        }
        
        switch(ircmd)
        {
            case IR_COMMAND_OPEN:   local_cmd = LOCAL_CMD_OPEN;ircmd = 0;break;
            case IR_COMMAND_CLOSE:  local_cmd = LOCAL_CMD_CLOSE;ircmd = 0;break;  
            case IR_COMMAND_STOP:   local_cmd = LOCAL_CMD_STOP;ircmd = 0;break;
            case IR_COMMAND_4:
            {
                ircmd = 0;
                local_cmd = LOCAL_CMD_STOP;
                mode = MODE_LOCAL;
                break;
            }                        
            case IR_COMMAND_0:
            {
                if(local_cmd == LOCAL_CMD_STOP)
                {
                    if(use_encoder_flag)
                    {
                        pSystemParam->adcvalue_valvelow = pData_ADC->adcvalue_encoder;
                    }
                    else
                    {
                        pSystemParam->adcvalue_valvelow = (u16)(pData_ADC->adcvalue_valve_filtered+0.5);
                    }
                    IapWrite_Opening0_adc(pSystemParam->adcvalue_valvelow);
					IapWrite_CloseDir(pSystemParam->close_dir);
                    mode = MODE_LOCAL;
                    LCD_ClearDisplay();
                    delay_ms(500);
                    Def0_DIS();
                    delay_ms(500);
                    LCD_ClearDisplay();
                    delay_ms(500);
                    Def0_DIS();
                    delay_ms(500);
                }
                ircmd = 0;
                break;
            }
            default:break;  
        }
        
        if(LOCAL == LOCAL_ON)   stopcnt_enable_flag = 1;
        if(LOCAL_STOP == LOCAL_STOP_ON && stopcnt_enable_flag==1) localexit_enable_flag = 1;
        
        if(timer2_200ms_flag == 1)
        {
            timer2_200ms_flag = 0;
            
            #ifdef PHASE_SEQ
            PhaseSeq_Update();//相序，缺相检测
            if(mode == MODE_LACK_PHASE) break;
            #endif 
            
            if(LOCAL_STOP == LOCAL_STOP_ON && stopcnt_enable_flag == 1)
            {
                if(exit_cnt >0) exit_cnt--;
            }    
            if(LOCAL == LOCAL_ON && localexit_enable_flag == 1)
            {
                if(exit_cnt == 0)//超过3s 不保存退出
                {
                    exit_cnt = 15;
                    mode = MODE_LOCAL;
                    break;   
                }
                else
                {
                    exit_cnt = 15;
                    if(use_encoder_flag)
                    {
                        pSystemParam->adcvalue_valvelow = pData_ADC->adcvalue_encoder;
                    }
                    else
                    {
                        pSystemParam->adcvalue_valvelow = (u16)(pData_ADC->adcvalue_valve_filtered+0.5);
                    }
                    IapWrite_Opening0_adc(pSystemParam->adcvalue_valvelow);
										IapWrite_CloseDir(pSystemParam->close_dir);
                    mode = MODE_LOCAL;                                
                    LCD_ClearDisplay();
                    delay_ms(500);
                    Def0_DIS();
                    delay_ms(500);
                    LCD_ClearDisplay();
                    delay_ms(500);
                    Def0_DIS();
                    delay_ms(500);
                    break;

                }    
            }          
        }
        
        Local_Control();
        LCD_SetDIS();
    }
    
}

void DefHundred_Mode()
{
    u8 zeroflag = 0;
	  u16 u16_data_tmp = 0;
    static u8 exit_cnt = 15;
    u8 stopcnt_enable_flag = 0;//退出使能，检测到现场时，使能之后的停止档计数
    u8 localexit_enable_flag = 0;//检测现场退出使能，置1后现场可退出
    Motor_CR = 0;
    Def100_DIS();
    delay_ms(500);
    delay_ms(500);
    
    while(mode == MODE_DEF_HUNDRED)
    {
        Acquire_Data();
        
        if(IRisReadyToRead())
        {
           IR_Update(); 
        }
        
        if(timer2_20ms_flag == 1)
        {
            timer2_20ms_flag = 0;
            Acquire_Data();
						CloseDir_Zero_Detect();
            DebugPrintf();
        }
        
        if(timer2_60ms_flag == 1)
        {
            timer2_60ms_flag = 0;
            MotorStop_Delay_60ms();
            LocalCmdLock_Detect();
        }

        switch(ircmd)
        {
            case IR_COMMAND_OPEN:   local_cmd = LOCAL_CMD_OPEN;ircmd = 0;break;
            case IR_COMMAND_CLOSE:  local_cmd = LOCAL_CMD_CLOSE;ircmd = 0;break;  
            case IR_COMMAND_STOP:   local_cmd = LOCAL_CMD_STOP;ircmd = 0;break;
            case IR_COMMAND_4:
            {
                ircmd = 0;
                local_cmd = LOCAL_CMD_STOP;
                mode = MODE_LOCAL;
                break;
            } 
            case IR_COMMAND_100:
            {
                if(local_cmd == LOCAL_CMD_STOP)
                {
                    if(use_encoder_flag)
                    {
                        pSystemParam->adcvalue_valvehigh = pData_ADC->adcvalue_encoder;

                    }
                    else
                    {
                        pSystemParam->adcvalue_valvehigh = (u16)(pData_ADC->adcvalue_valve_filtered+0.5);
                    }
                    IapWrite_Opening100_adc(pSystemParam->adcvalue_valvehigh);
										IapWrite_CloseDir(pSystemParam->close_dir);
                    mode = MODE_LOCAL;
                }
                ircmd = 0;
                break;
            }
            default:break;  
        }
        
        if(LOCAL == LOCAL_ON)   stopcnt_enable_flag = 1;
        if(LOCAL_STOP == LOCAL_STOP_ON && stopcnt_enable_flag==1) localexit_enable_flag = 1;
        if(timer2_200ms_flag == 1)
        {
            timer2_200ms_flag = 0;
            #ifdef PHASE_SEQ
            PhaseSeq_Update();//相序，缺相检测
            if(mode == MODE_LACK_PHASE) break;
            #endif 
            
            if(LOCAL_STOP == LOCAL_STOP_ON && stopcnt_enable_flag == 1)
            {
                if(exit_cnt >0) exit_cnt--;
            }    
            if(LOCAL == LOCAL_ON && localexit_enable_flag == 1)
            {
                if(exit_cnt == 0)//超过3s 不保存退出
                {
                    exit_cnt = 15;
                    mode = MODE_LOCAL;
                    break;   
                }
                else
                {
                    exit_cnt = 15;
                    if(use_encoder_flag)
                    {
                        pSystemParam->adcvalue_valvehigh = pData_ADC->adcvalue_encoder;
                    }
                    else
                    {
                        pSystemParam->adcvalue_valvehigh = (u16)(pData_ADC->adcvalue_valve_filtered+0.5);
                    }
                    IapWrite_Opening100_adc(pSystemParam->adcvalue_valvehigh);
										IapWrite_CloseDir(pSystemParam->close_dir);
                    mode = MODE_LOCAL;                                
                    LCD_ClearDisplay();
                    delay_ms(500);
                    Def100_DIS();
                    delay_ms(500);
                    LCD_ClearDisplay();
                    delay_ms(500);
                    Def100_DIS();
                    delay_ms(500);
                    break;

                }    
            }          
        }
        Local_Control();
        LCD_SetDIS();
    }    
}

void SetSen_Mode()
{
    u8 i;

    LCD_ClearDisplay();
    delay_ms(500);
    delay_ms(500);
    
    pSystemParam->sen = IapRead_Sen();
    if(pSystemParam->sen == 0xff) pSystemParam->sen = SEN0_5MA;

    p_sen = &sen_table[0];
    for(i=0;i<10;i++)
    {
        if(pSystemParam->sen == *p_sen)   break;
        else
        {
            if(i == 9)
            {
                p_sen = &sen_table[0];
            }
            else    p_sen++;
        }
        
    }
    
    while(mode == MODE_SET_SEN)
    {
        
        if( IRisReadyToRead() )
        {
           IR_Update(); 
        }
            
        switch(ircmd)
        {
            case IR_COMMAND_OPEN:
            {
                if(*p_sen == sen_table[9])  p_sen = &sen_table[0];
                else    p_sen++;
                
                pSystemParam->sen = *p_sen;
                IapWrite_Sen(pSystemParam->sen);
                ircmd = 0;
                break;
            }
            case IR_COMMAND_CLOSE:
            {
                if(*p_sen == sen_table[0])  p_sen = sen_table+9;
                else    p_sen--;

                pSystemParam->sen = *p_sen;
                IapWrite_Sen(pSystemParam->sen);
                ircmd = 0;
                break;
            }
            case IR_COMMAND_4:
            {
                mode = MODE_LOCAL;
                ircmd = 0;
                break; 
            }
                
            default:break;    

        }
        Sen_DIS(pSystemParam->sen);
        LCD_SetDIS();

    }
      
}

void SetInputLow_Mode()
{
	u8 samp_cnt = 0;
	u16 read_value;
	while(mode==MODE_SETINPUT_LOW)
	{
//        #ifdef WATCH_DOG
//        WDT_CONTR = WATCH_DOG_RSTVAL;
//        #endif
        
        if(timer2_20ms_flag==1)
        {
            timer2_20ms_flag = 0;
			samp_cnt++;
            Update_InputCurrent();
			
			if(samp_cnt>50)
			{
				samp_cnt = samp_cnt;
				pSystemParam->setinput_low = (u16)pData_ADC->adcvalue_input_filtered;
				IapWrite_SetInputLow(pSystemParam->setinput_low);
				read_value = IapRead_SetInputLow();
				mode = MODE_DISTANT_ANALOG;
				printf("\r\n setinput_low:%d",pSystemParam->setinput_low);
				printf("\r\n read_value_low:%d",read_value);
				break;
				
			}
        }
		
        if(timer2_100ms_flag == 1)
        {
            timer2_100ms_flag = 0;
			Set_InputLow_DIS();
        } 
	
	}
	
}

void SetInputHigh_Mode()
{
	u8 samp_cnt = 0;
	u16 read_value;
	
	while(mode==MODE_SETINPUT_HIGH)
	{
//        #ifdef WATCH_DOG
//        WDT_CONTR = WATCH_DOG_RSTVAL;
//        #endif
        
        
        if(timer2_20ms_flag==1)
        {
            timer2_20ms_flag = 0;
			samp_cnt++;
            Update_InputCurrent();
			
			if(samp_cnt>50)
			{
				samp_cnt = samp_cnt;
				pSystemParam->setinput_high = (u16)pData_ADC->adcvalue_input_filtered;
				IapWrite_SetInputHigh(pSystemParam->setinput_high);
				mode = MODE_DISTANT_ANALOG;	
				read_value = IapRead_SetInputHigh();
				printf("\r\n setinput_high:%d",pSystemParam->setinput_high);
				printf("\r\n read_value_high:%d",read_value);
				break;
			}
        }
		
        if(timer2_100ms_flag == 1)
        {
            timer2_100ms_flag = 0; 
			Set_InputHigh_DIS();			
            
        } 

		
	}	
}

void CalOut_Mode()
{
    u16 pwm_cal_cnt;

    LCD_FullDisplay();
   delay_ms(500);
   delay_ms(500);
    
    Acquire_Data();
    pwm_cal_cnt = (u16)(0.02 * PWM_DUTY * 199.5 / pData_Voltage->voltage_vcc);  //
    PWM_Cal(pwm_cal_cnt);
    
    while(mode == MODE_OUT_CAL)
    {
        Acquire_Data();
        
        if(IRisReadyToRead())
        {
           IR_Update(); 
        }
        if(timer2_20ms_flag == 1)
        {
            timer2_20ms_flag = 0;
            Acquire_Data();
            DebugPrintf();
        }

        switch(ircmd)
        {
            case IR_COMMAND_OPEN:
            {
                pwm_cal_cnt++;
                PWM_Cal(pwm_cal_cnt);
                ircmd = 0;
                break;
            }
            case IR_COMMAND_CLOSE:
            {
                pwm_cal_cnt--;
                PWM_Cal(pwm_cal_cnt);
                ircmd = 0;
                break;  
            }
            case IR_COMMAND_4:
            {
                pSystemParam->res_output = (pData_Voltage->voltage_vcc * pwm_cal_cnt) / (0.02 * PWM_DUTY);
                IapWrite_R_out(pSystemParam->res_output); 
                mode = MODE_LOCAL;
                ircmd = 0;
                break;
            }
            default:break; 
        }

    }
  
}

void CalIn_Mode()
{
    LCD_ClearDisplay();
    delay_ms(500);
    delay_ms(500);
    
    while(mode == MODE_IN_CAL)
    {
        if(IRisReadyToRead())
        {
           IR_Update(); 
        }
        
        if(timer2_20ms_flag == 1)
        {
            timer2_20ms_flag = 0;
            Acquire_Data();
            Update_InputCurrent();
            DebugPrintf();
        }
        
        switch(ircmd)
        {
            case IR_COMMAND_4:
            {
                Acquire_Data();
                pSystemParam->res_input = pData_Voltage->voltage_input / 0.02;
                IapWrite_R_in(pSystemParam->res_input);
                mode = MODE_LOCAL;
                break;
            }
            case IR_COMMAND_CLOSE:
            {
                mode = MODE_LOCAL;
                break;                  
            }
            default:break;               
            
        }
    }  
}



static void LocalCmdLock_Detect()//执行周期 50ms
{
    if(local_lock_startflag == 1)
    {
        if(LOCAL_OPEN == LOCAL_OPEN_ON || LOCAL_CLOSE == LOCAL_CLOSE_ON)
        {
            if(local_lock_cnt<LOCAL_LOCK_DELAY) local_lock_cnt++;
            else
            {
                if(local_lock_flag != 1)    local_lock_flag = 1;
            }
        }
        else
        {
            local_lock_startflag = 0;
            if(local_lock_flag !=1) local_cmd = LOCAL_CMD_STOP;//点动
        }

    }        
}


static void LocalDef_Detect()//200ms 执行一次
{
    static u8 localdef_cnt = 15;//3s
    
    if(LOCAL_STOP == LOCAL_STOP_ON && mode == MODE_LOCAL)
    {
        if(LOCAL_OPEN == LOCAL_OPEN_ON)
        {
            if(localdef_cnt >0) localdef_cnt--;
            else
            {
                mode = MODE_DEF_HUNDRED;
                localdef_cnt = 15;
            }
        }
        else 
        {
            if(LOCAL_CLOSE == LOCAL_CLOSE_ON)
            {
                if(localdef_cnt>0)  localdef_cnt--;
                else
                {
                    mode = MODE_DEF_ZERO;
                    localdef_cnt = 15;
                }
            }
            else
            {
                if(localdef_cnt !=15)   localdef_cnt = 15;
            }
        }
        
    }
    
}

/********************* INT2中断服务*************************/
void Ext_INT2 (void) interrupt INT2_VECTOR	// LOCAL_OPEN
{
    delay_ms(1); 
    if(LOCAL_OPEN == LOCAL_OPEN_ON)
    {
        if(LOCAL == LOCAL_ON || mode == MODE_DEF_HUNDRED || mode == MODE_DEF_ZERO)
        {
            local_cmd = LOCAL_CMD_OPEN;
            local_lock_cnt = 0;
            local_lock_flag = 0;
            local_lock_startflag = 1;
        }
          
    }

}

/********************* INT3中断服务*************************/
void Ext_INT3 (void) interrupt INT3_VECTOR   // LOCAL_CLOSE
{
    delay_ms(1);
    if(LOCAL_CLOSE == LOCAL_CLOSE_ON)
    {  
        if(LOCAL == LOCAL_ON || mode == MODE_DEF_HUNDRED || mode == MODE_DEF_ZERO)
        {
            local_cmd = LOCAL_CMD_CLOSE;
            local_lock_cnt = 0;
            local_lock_flag = 0;
            local_lock_startflag = 1;
        }
    }

}

