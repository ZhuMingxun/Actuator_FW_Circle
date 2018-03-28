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

 u8 local_stat;
 u8 local_stat_cnt;
volatile LocalCmd_Type local_cmd = LOCAL_CMD_STOP;


u16  code sen_table[10] = {SEN0_05MA,SEN0_1MA,SEN0_15MA,SEN0_2MA,SEN0_25MA,SEN0_3MA,SEN0_35MA,SEN0_4MA,SEN0_45MA,SEN0_5MA};
const u16* p_sen = &sen_table[0];

void LocalDef_Detect();
static void IRCmd_To_LocalCmd();
static void Local_Stat_Detect();
static void Local_Stat_Count_per60ms();

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
    local_stat = LOCAL_STAT_STOP;
    local_stat_cnt=0;
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
			if(mode != MODE_LOCAL)
				break;					
        }
		
		Local_Stat_Detect();
		Local_Control();
		
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
            
            Update_InputCurrent();
            LimitPosition_Output();
            Torque_Detect();
            PWM_Update(); 
            LCD_DIS();
             
        }

        if(timer2_200ms_flag == 1)
        {
            timer2_200ms_flag = 0;
            SystemMode_Detect();
            if(mode == MODE_DISTANT_ANALOG || mode == MODE_DISTANT_DIGITAL) 
                break;
            
            LocalDef_Detect();
            if(mode == MODE_DEF_HUNDRED || mode == MODE_DEF_ZERO || mode==MODE_CAL_LOW || mode==MODE_CAL_HIGH)   
                break;
            
            #ifdef PHASE_SEQ
            PhaseSeq_Update();//相序，缺相检测
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
			Local_Stat_Count_per60ms();
            Motor_PosRev();
            printf(" -Local- \r\n");
        }
  
    }
  
}

static void IRCmd_To_LocalCmd()
{
    switch(ircmd)
    {
        case IR_COMMAND_100:    break;   
        case IR_COMMAND_0:      break;       
        case IR_COMMAND_OPEN:   {local_cmd = LOCAL_CMD_OPEN; break;}    
        case IR_COMMAND_CLOSE:  {local_cmd = LOCAL_CMD_CLOSE;break;} 
        case IR_COMMAND_STOP:   {local_cmd = LOCAL_CMD_STOP; break;}
        case IR_COMMAND_4:      break;
        case IR_COMMAND_0_100:
        {
            if(LOCAL_STOP == LOCAL_STOP_ON) 
            mode =  MODE_SET_SEN; 
            break;
        }
        case IR_COMMAND_4_0:    
        {
            if(LOCAL_STOP == LOCAL_STOP_ON)
            mode = MODE_DEF_ZERO;
            break;
        }
        case IR_COMMAND_4_100:  
        {
            if(LOCAL_STOP == LOCAL_STOP_ON)
            mode = MODE_DEF_HUNDRED;
            break;
        }
        case IR_COMMAND_4_OPEN:
        {
            if(LOCAL_STOP == LOCAL_STOP_ON)
            mode = MODE_OUT_CAL;
            break;
        }
        case IR_COMMAND_4_CLOSE:
        {
            if(LOCAL_STOP == LOCAL_STOP_ON)
            mode = MODE_IN_CAL;
            break;
        }
        default:break;  
    }
    ircmd = 0;

}

/**************现场控制函数******************/
void Local_Control()
{
    
    if(KeyScan(KEY_STOP) )
    {
        local_cmd = LOCAL_CMD_STOP;
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





#define DEF_STEP0_STOP	0
#define DEF_STEP1_LOCAL	1
#define DEF_STEP2_STOP	2
#define DEF_STEP3_LOCAL	3

void DefZero_Mode()
{
	u8 def_step = 0;
    u16 u16_data_tmp = 0;
    u8 exit_cnt = 15;
	u8 cal_low_cnt = 0;
    u8 ir_4_cnt=0;
    u8 ir_0_cnt=0;
	
    Motor_CR &= ~(0x03);

    Def0_DIS();
    delay_ms(500);
    delay_ms(500);
    
    while(mode == MODE_DEF_ZERO)
    {
         printf("\r\n enter while(1) !");
        
        if(IRisReadyToRead())
        {
           IR_Update(); 
        }

        
        switch(ircmd)
        {
            case IR_COMMAND_OPEN:   local_cmd = LOCAL_CMD_OPEN;ircmd = 0;break;
            case IR_COMMAND_CLOSE:  local_cmd = LOCAL_CMD_CLOSE;ircmd = 0;break;  
            case IR_COMMAND_STOP:   local_cmd = LOCAL_CMD_STOP;ircmd = 0;break;
            case IR_COMMAND_4:
            {
                ir_4_cnt++;
                if(ir_4_cnt>4)
                {
                    ir_4_cnt=0;
                    local_cmd = LOCAL_CMD_STOP;
                    mode = MODE_LOCAL;
                    printf("\r\n IR_COMMAND_4 Exit!");
                }
                
                ircmd = 0;

                break;
            }                        
            case IR_COMMAND_0:
            {
                ir_0_cnt++;
                if(ir_0_cnt>4)
                {
                    ir_0_cnt=0;
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
                         printf("\r\n IR_COMMAND_0 Exit!");
                    }
                }
                ircmd = 0;

                break;
            }
            default:break;  
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
			Local_Stat_Count_per60ms();
        }
		
        if(timer2_100ms_flag == 1)
        {
            timer2_100ms_flag = 0;
            Torque_Detect();
            TravelProtect_Def(); 
            
        }
		
        if(timer2_200ms_flag == 1)
        {
            timer2_200ms_flag = 0;
            
			if(def_step==DEF_STEP2_STOP)
			{
				if(exit_cnt >0) exit_cnt--;
			}
			
			if(def_step==DEF_STEP0_STOP)
			{
				if(KeyScan(KEY_CLOSE))
				{
					cal_low_cnt++;
					if(cal_low_cnt>30)
					{
						mode = MODE_CAL_LOW;
                        printf("\r\n enter MODE_CAL_LOW");
						cal_low_cnt = 0;
						return;
					}
				}
				else
				{
					cal_low_cnt = 0;
				}
			}
  
            #ifdef PHASE_SEQ
            PhaseSeq_Update();//相序，缺相检测
            if(mode == MODE_LACK_PHASE) break;
            #endif 
            
        }
		
		switch(def_step)
		{
			case DEF_STEP0_STOP:
			{
				if(KeyScan(KEY_LOCAL))
				{
					def_step = DEF_STEP1_LOCAL;
					printf("\r\n Switch to DEF_STEP1_STOP");
				}
				break;
			}
			
			case DEF_STEP1_LOCAL:
			{
				if(KeyScan(KEY_STOP))
				{

					def_step = DEF_STEP2_STOP;
					exit_cnt = 15;
					printf("\r\n Switch to DEF_STEP2_STOP");
				}
				break;
			}
			
			case DEF_STEP2_STOP:
			{
				
				if(KeyScan(KEY_LOCAL))
				{
					def_step = DEF_STEP3_LOCAL;
					printf("\r\n Switch to DEF_STEP3_STOP");
				}
				
				break;
			}
			
			case DEF_STEP3_LOCAL:
			{
				if(exit_cnt>0)
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
                    LCD_ClearDisplay();
                    delay_ms(500);
                    Def0_DIS();
                    delay_ms(500);
                    LCD_ClearDisplay();
                    delay_ms(500);
                    Def0_DIS();
                    delay_ms(500);
                    printf("\r\n DEF_STEP3_LOCAL Exit Save!");                    
 
				}
                mode = MODE_LOCAL; 
			    printf("\r\n DEF_STEP3_LOCAL Exit!");
				break;
			}
			default:break;
			
		}
        
		Local_Stat_Detect();
        Local_Control();
        LCD_SetDIS();
        printf("\r\nwhile(1) end");
    }
    
}

void DefHundred_Mode()
{
    //u8 zeroflag = 0;
    u8 def_step = 0;
    u16 u16_data_tmp = 0;
    static u8 exit_cnt = 15;
    u8 cal_high_mode_cnt = 0;
    u8 ir_4_cnt = 0;
    u8 ir_100_cnt = 0;

    Motor_CR &= ~(0x03);
    
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
        
        switch(ircmd)
        {
            case IR_COMMAND_OPEN:   local_cmd = LOCAL_CMD_OPEN;ircmd = 0;break;
            case IR_COMMAND_CLOSE:  local_cmd = LOCAL_CMD_CLOSE;ircmd = 0;break;  
            case IR_COMMAND_STOP:   local_cmd = LOCAL_CMD_STOP;ircmd = 0;break;
            case IR_COMMAND_4:
            {
                ir_4_cnt++;
                if(ir_4_cnt>4)
                {
                    ir_4_cnt=0;
                    
                    local_cmd = LOCAL_CMD_STOP;
                    mode = MODE_LOCAL;
                }
                ircmd = 0;
                break;
            } 
            case IR_COMMAND_100:
            {
                ir_100_cnt++;
                if(ir_100_cnt>4)
                {
                    ir_100_cnt=0;
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
                        LCD_ClearDisplay();
                        delay_ms(500);
                        Def100_DIS();
                        delay_ms(500);
                        LCD_ClearDisplay();
                        delay_ms(500);
                        Def100_DIS();
                        delay_ms(500);
                    }
                }
                ircmd = 0;
                break;
            }
            default:break;  
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
            Local_Stat_Count_per60ms();
        }
        
        if(timer2_100ms_flag == 1)
        {
            timer2_100ms_flag = 0;
            Torque_Detect();
            TravelProtect_Def(); 
        }
        
        if(timer2_200ms_flag == 1)
        {
            timer2_200ms_flag = 0;
            
			if(def_step==DEF_STEP2_STOP)
			{
				if(exit_cnt >0) exit_cnt--;
			}
			
			if(def_step==DEF_STEP0_STOP)
			{
				if(KeyScan(KEY_OPEN))
				{
					cal_high_mode_cnt++;
					if(cal_high_mode_cnt>30)
					{
						mode = MODE_CAL_HIGH;
						cal_high_mode_cnt = 0;
						return;
					}
	//			printf("\r\ncal_hight_cnt:%d ",(int)cal_high_mode_cnt);
				}
				else
				{
					cal_high_mode_cnt = 0;
				}
			}
            
            #ifdef PHASE_SEQ
            PhaseSeq_Update();//相序，缺相检测
            if(mode == MODE_LACK_PHASE) break;
            #endif 
         
        }
        
		switch(def_step)
		{
			
			case DEF_STEP0_STOP:
			{
				if(KeyScan(KEY_LOCAL))
				{
					def_step = DEF_STEP1_LOCAL;
				}
				break;
			}
			
			case DEF_STEP1_LOCAL:
			{
				if(KeyScan(KEY_STOP))
				{
				    def_step = DEF_STEP2_STOP;
					exit_cnt = 15;
				}
				break;
			}
			
			case DEF_STEP2_STOP:
			{
				
				if(KeyScan(KEY_LOCAL))
				{
					def_step = DEF_STEP3_LOCAL;
					//delay_ms(1000);
				}
				
				break;
			}
			
			case DEF_STEP3_LOCAL:
			{
				if(exit_cnt>0)
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

				}
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
			default:break;
			
		}
		Local_Stat_Detect();
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


//低端校准4mA
void CalLow_Mode()
{

    u16 pwm_cal_cnt;
	u8 ir_op_cnt=0;
	u8 ir_cl_cnt=0;
	u8 ir_4_cnt=0;
	u8 ir_0_cnt=0;
	//pSystemParam->cal_low = IapRead_CalLow();
	pwm_cal_cnt = pSystemParam->cal_low;
	
    Acquire_Data();
    PWM_Cal(pwm_cal_cnt);
    CalLow_DIS();
 
    while(KeyScan(KEY_CLOSE))
	{
//        #ifdef WATCH_DOG
//        WDT_CONTR = WATCH_DOG_RSTVAL;
//        #endif  
		delay_ms(50);
	}
    while(mode == MODE_CAL_LOW)
    {
//        #ifdef WATCH_DOG
//        WDT_CONTR = WATCH_DOG_RSTVAL;
//        #endif    
        if( IRisReadyToRead() )
        {
           IR_Update(); 
        }
		
        if(timer2_20ms_flag==1)
        {
            timer2_20ms_flag = 0;
            Acquire_Data();
        }
        
        if(timer2_100ms_flag == 1)
        {
            timer2_100ms_flag = 0;
			//旋钮检测
			if(KeyScan(KEY_OPEN))
			{
				pwm_cal_cnt++;
			}
			if(KeyScan(KEY_CLOSE))
			{
				pwm_cal_cnt--;
			}			
        }
        
        if(timer_1s_flag)
        {
            timer_1s_flag = 0;
            if(KeyScan(KEY_LOCAL))//确定
            {
                pSystemParam->cal_low = pwm_cal_cnt;
                IapWrite_CalLow(pwm_cal_cnt);
                mode = MODE_LOCAL;
				LCD_ClearDisplay();
				delay_ms(500);
				CalLow_DIS();
				delay_ms(500);
				LCD_ClearDisplay();
				delay_ms(500);
				CalLow_DIS();
				delay_ms(500);
				break;
            }
            if(KeyScan(KEY_REMOTE))//退出
            {
                mode = MODE_DISTANT_ANALOG;
                break;	
            }
        }

		PWM_Cal(pwm_cal_cnt);

        switch(ircmd)
        {
            case IR_COMMAND_OPEN:
            {
                ir_op_cnt++;
                if(ir_op_cnt>2)
                {
                    ir_op_cnt=0;
                    pwm_cal_cnt += 1;
                    PWM_Cal(pwm_cal_cnt);
                    
                }
                ircmd = 0;
                break;
            }
            case IR_COMMAND_CLOSE:
            {
                ir_cl_cnt++;
                if(ir_cl_cnt>2)
                {
                    ir_cl_cnt=0;
                    pwm_cal_cnt -= 1;
                    PWM_Cal(pwm_cal_cnt);
                }
                ircmd = 0;
                break;  
            }
            case IR_COMMAND_4:
            {
                ir_4_cnt++;
                if(ir_4_cnt>4)
                {
                    ir_4_cnt=0;
                    mode = MODE_LOCAL;
                }
                ircmd = 0;
                break;
            }
            case IR_COMMAND_0:
            {
                ir_0_cnt++;
                if(ir_0_cnt>4)
                {
                    ir_0_cnt = 0;
                    pSystemParam->cal_low = pwm_cal_cnt;
                    IapWrite_CalLow(pwm_cal_cnt);
                    mode = MODE_LOCAL;
					LCD_ClearDisplay();
					delay_ms(500);
					CalLow_DIS();
					delay_ms(500);
					LCD_ClearDisplay();
					delay_ms(500);
					CalLow_DIS();
					delay_ms(500);
                }
                ircmd = 0;
                break;
            }
            
            default:break; 
        }

    }	
}



//高端校准20mA
void CalHigh_Mode()
{
    u16 pwm_cal_cnt;
	u16 rd_cnt;
	u8 ir_op_cnt=0;
	u8 ir_cl_cnt=0;
	u8 ir_4_cnt=0;
	u8 ir_100_cnt=0;
	
	pSystemParam->cal_high = IapRead_CalHigh();
	pwm_cal_cnt = pSystemParam->cal_high;
	
	CalHigh_DIS();
    Acquire_Data();
	PWM_Cal(pwm_cal_cnt);

	while(KeyScan(KEY_OPEN))//等待按键释放
	{
		delay_ms(50);
	}
	
	//printf("\r\npwm_cal_cnt:%d ",(int)pwm_cal_cnt);
	
    while(mode == MODE_CAL_HIGH)
    {

        if( IRisReadyToRead() )
        {
           IR_Update(); 
        }
		
        if(timer2_20ms_flag==1)
        {
            timer2_20ms_flag = 0;
            Acquire_Data();
        }
        
        if(timer2_100ms_flag == 1)
        {
            timer2_100ms_flag = 0;

			if(KeyScan(KEY_OPEN))
			{
				pwm_cal_cnt++;
			}
			if(KeyScan(KEY_CLOSE))
			{
				pwm_cal_cnt--;
			}				
        }

        if(timer_1s_flag)
        {
            timer_1s_flag = 0;
            if(KeyScan(KEY_LOCAL))//确定
            {
                pSystemParam->cal_high = pwm_cal_cnt;
                IapWrite_CalHigh(pwm_cal_cnt);
                rd_cnt = IapRead_CalHigh();
                printf("\r\nset_cnt:%d ",(int)pwm_cal_cnt);
                printf("\r\nread_cnt:%d ",(int)rd_cnt);
                printf("\r\ncal_high:%d ",(int)pSystemParam->cal_high);
                
                mode = MODE_LOCAL;//确定退出
				LCD_ClearDisplay();
				delay_ms(500);
				CalHigh_DIS();
				delay_ms(500);
				LCD_ClearDisplay();
				delay_ms(500);
				CalHigh_DIS();
				delay_ms(500);
				break;
            }
            if(KeyScan(KEY_REMOTE))//退出
            {
                mode = MODE_DISTANT_ANALOG;
                break;	
            }
        }            
		
		PWM_Cal(pwm_cal_cnt);

        switch(ircmd)
        {
            case IR_COMMAND_OPEN:
            {
                ir_op_cnt++;
                if(ir_op_cnt>2)
                {
                    ir_op_cnt=0;
                    pwm_cal_cnt += 1;
                    PWM_Cal(pwm_cal_cnt);
                }
                ircmd = 0;
                break;
            }
            case IR_COMMAND_CLOSE:
            {
                ir_cl_cnt++;
                if(ir_cl_cnt>2)
                {
                    ir_cl_cnt=0;
                    pwm_cal_cnt -= 1;
                    PWM_Cal(pwm_cal_cnt);
                }
                ircmd = 0;
                break;  
            }
            case IR_COMMAND_4:
            {
                ir_4_cnt++;
                if(ir_4_cnt>4)
                {
                    ir_4_cnt=0;
                    mode = MODE_LOCAL;//退出
                }
                ircmd = 0;
                break;
            }
            case IR_COMMAND_100:
            {
                ir_100_cnt++;
                if(ir_100_cnt>4)
                {
                    ir_100_cnt = 0;
                    pSystemParam->cal_high = pwm_cal_cnt;
                    IapWrite_CalHigh(pwm_cal_cnt);
                    mode = MODE_LOCAL;
					LCD_ClearDisplay();
					delay_ms(500);
					CalHigh_DIS();
					delay_ms(500);
					LCD_ClearDisplay();
					delay_ms(500);
					CalHigh_DIS();
					delay_ms(500); 
                }
                ircmd = 0; 
                break;                
            } 
            
            default:break; 
        }

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
				LCD_ClearDisplay();
				delay_ms(500);
				Set_InputLow_DIS();
				delay_ms(500);
				LCD_ClearDisplay();
				delay_ms(500);
				Set_InputLow_DIS();
				delay_ms(500); 
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
				delay_ms(500);
				Set_InputHigh_DIS();
				delay_ms(500);
				LCD_ClearDisplay();
				delay_ms(500);
				Set_InputHigh_DIS();
				delay_ms(500); 
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



//static void LocalCmdLock_Detect()//执行周期 50ms
//{
//    if(local_lock_startflag == 1)
//    {
//        if(LOCAL_OPEN == LOCAL_OPEN_ON || LOCAL_CLOSE == LOCAL_CLOSE_ON)
//        {
//            if(local_lock_cnt<LOCAL_LOCK_DELAY) local_lock_cnt++;
//            else
//            {
//                if(local_lock_flag != 1)    local_lock_flag = 1;
//            }
//        }
//        else
//        {
//            local_lock_startflag = 0;
//            if(local_lock_flag !=1) local_cmd = LOCAL_CMD_STOP;//点动
//        }

//    }        
//}


static void LocalDef_Detect()//200ms 执行一次
{
	static u8 open_cnt;//6s
	static u8 close_cnt;
    
    if(KeyScan(KEY_STOP) && mode == MODE_LOCAL)
    {
        if(KeyScan(KEY_OPEN))
        {
			open_cnt++;
			if(open_cnt>15)
			{
				mode = MODE_DEF_HUNDRED;
				open_cnt = 0;
				
			}
				
			
        }
        else if(KeyScan(KEY_CLOSE))
        {
			close_cnt++;
			if(close_cnt>15)
			{
				mode = MODE_DEF_ZERO;
				close_cnt = 0;
			}
        }
		else
		{
			open_cnt = 0;
			close_cnt = 0;
		}

        
    }
    
}

extern bit motordelay_flag;
static void Local_Stat_Detect()
{

	switch(local_stat)
	{
		case LOCAL_STAT_STOP://现场停止状态
		{
			local_stat_cnt = 0;
			
			if(KeyScan(KEY_LOCAL))
			{     
				
                if(KeyScan(KEY_OPEN) && motordelay_flag)
				{
					local_stat = LOCAL_STAT_OPEN_INCHING;
				}
				else if(KeyScan(KEY_CLOSE) && motordelay_flag)
				{
					local_stat = LOCAL_STAT_CLOSE_INCHING;
					
				}
			}
			
			break;
		}
		
		case LOCAL_STAT_OPEN_INCHING://开-点动区
		{
			if(KeyScan(KEY_STOP) || !KeyScan(KEY_OPEN))
			{
				local_stat = LOCAL_STAT_STOP;
				break;
			}
			
			if(local_stat_cnt>2)//持续超3秒
			{
				//local_stat_cnt = 0;
				local_stat = LOCAL_STAT_OPEN_KEEP;
			}
			break;

		}
		
		case LOCAL_STAT_CLOSE_INCHING://关-点动区
		{
			if(KeyScan(KEY_STOP) || !KeyScan(KEY_CLOSE))
			{
				local_stat = LOCAL_STAT_STOP;
				break;
			}
			
			if(local_stat_cnt>2)//持续超3秒
			{
				//local_stat_cnt = 0;
				local_stat = LOCAL_STAT_CLOSE_KEEP;
			}
			break;

		}
		
		case LOCAL_STAT_OPEN_KEEP:
		{
			if(KeyScan(KEY_STOP) || KeyScan(KEY_CLOSE))
			{
				local_stat = LOCAL_STAT_STOP;
			}
			break;

		}
		
		case LOCAL_STAT_CLOSE_KEEP:
		{
			if(KeyScan(KEY_STOP) || KeyScan(KEY_OPEN))
			{
				local_stat = LOCAL_STAT_STOP;
			}
			break;

		}
		default:
		{
			local_stat = LOCAL_STAT_STOP;
			break;
		}
		
	}
	
	switch(local_stat)
	{
		case LOCAL_STAT_STOP:
		{
			local_cmd = LOCAL_CMD_STOP;
			break;
		}
		case LOCAL_STAT_OPEN_INCHING:case LOCAL_STAT_OPEN_KEEP:
		{
			local_cmd = LOCAL_CMD_OPEN;
			break;
		}
		case LOCAL_STAT_CLOSE_INCHING:case LOCAL_STAT_CLOSE_KEEP:
		{
			local_cmd = LOCAL_CMD_CLOSE;
			break;
		}
		default:
		{
			local_cmd = LOCAL_CMD_STOP;
			break;
		}
		
	}	
}

static void Local_Stat_Count_per60ms()
{
	if(local_stat==LOCAL_STAT_OPEN_INCHING || local_stat==LOCAL_STAT_CLOSE_INCHING)
	{
		local_stat_cnt++;
	}	
}

/********************* INT2中断服务*************************/
void Ext_INT2 (void) interrupt INT2_VECTOR	// LOCAL_OPEN
{
//    delay_ms(1); 
//    if(LOCAL_OPEN == LOCAL_OPEN_ON)
//    {
//        if(LOCAL == LOCAL_ON || mode == MODE_DEF_HUNDRED || mode == MODE_DEF_ZERO)
//        {
//            local_cmd = LOCAL_CMD_OPEN;
//            local_lock_cnt = 0;
//            local_lock_flag = 0;
//            local_lock_startflag = 1;
//        }
//          
//    }

}

/********************* INT3中断服务*************************/
void Ext_INT3 (void) interrupt INT3_VECTOR   // LOCAL_CLOSE
{
//    delay_ms(1);
//    if(LOCAL_CLOSE == LOCAL_CLOSE_ON)
//    {  
//        if(LOCAL == LOCAL_ON || mode == MODE_DEF_HUNDRED || mode == MODE_DEF_ZERO)
//        {
//            local_cmd = LOCAL_CMD_CLOSE;
//            local_lock_cnt = 0;
//            local_lock_flag = 0;
//            local_lock_startflag = 1;
//        }
//    }

}

