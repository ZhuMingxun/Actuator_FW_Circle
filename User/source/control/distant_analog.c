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
#include "uart_debug.h"
#include <stdio.h>
#include "actuator_config.h"
#define CTR_READY           1
#define CTR_RUN_OPEN        2
#define CTR_RUN_CLOSE       3
#define CTR_STOP            4

PosControl_TypeDef PosCTR;
PosControl_TypeDef* pPosCTR = &PosCTR;
u8 ctr_stat = CTR_READY;
u8 ctr_stat_pre = CTR_READY;

void UpdateOffset_Open();
void UpdateOffset_Close();
void DistantAnalog_Control_Pos();
extern  u16 code  sen_table[10];

void DistantAnalogMode_Init()
{
    Exti23_Disable();
    IR_Disable();
    LED_LSG_OFF
    LCD_YELLOW_OFF
    Motor_Stop();
    ERR_OFF
    ReadSystemPara();
    Acquire_Data();
    delay_ms(500);
    delay_ms(500);
    SystemMode_Detect();
   
}

/*****************远方模拟模式********************/
/*date:2015.9.12                     */
/*description:   */
void DistantAnalog_Mode()
{
    DistantAnalogMode_Init();
    while( mode == MODE_DISTANT_ANALOG )
    {
 
        if(timer2_20ms_flag == 1)
        {
            timer2_20ms_flag = 0;
            
            Acquire_Data();
            //DebugPrintf();
        #ifdef TRAVEL_PROTECT_MACHINE
            TravelProtect_Machine();
        #else
            TravelProtect();
        #endif
            Update_InputCurrent();
            if(pSystemParam->sen != sen_table[9])
            {
                DistantAnalog_Control();
            }
            else
            {
                DistantAnalog_Control_Pos();
            }
//			printf("\r\n valve_filter:%f ",pData_ADC->adcvalue_valve_filtered);
//			printf("opening_in:%f ",pPosCTR->pos_input);
//			printf("opening:%f L:%d,H:%d ",pData_Acquire->opening,pSystemParam->adcvalue_valvelow,pSystemParam->adcvalue_valvehigh);
//			printf("offset_open_filter:%f ",pPosCTR->offset_open_filter);
//			printf("offset_close_filter:%f ",pPosCTR->offset_close_filter);
//			printf("open100_flag:%d close0_flag:%d",(int)open100_flag,(int)close0_flag);
//			printf(" -Distant- \r\n");
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

            //TravelProtect();
            LCD_DIS();
            PWM_Update();
        }
        
        if(timer2_200ms_flag == 1)
        {
            timer2_200ms_flag = 0; 
            SystemMode_Detect();//远方现场模式检测
            if(mode == MODE_LOCAL || mode == MODE_DISTANT_DIGITAL)  break;
            
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
        }        
    }
  
}
#define __CTRL_STAT_OPEN 1
#define __CTRL_STAT_CLOSE 2
#define __CTRL_STAT_FINISH 0
/********************远方模拟控制******************/
void DistantAnalog_Control()
{
     u16 current_out_cmp;
     u16 current_in_cmp;
     u16 err_tmp;
     u16 sen_tmp;
    
     current_out_cmp = (u16)(pData_Acquire->current_output * 1000);
     current_in_cmp = (u16)(pData_Acquire->current_input * 1000);

     err_tmp = (u16)fabs((float)current_out_cmp - (float)current_in_cmp);//计算偏差
     sen_tmp = pSystemParam->sen; //读取灵敏度
    
     if(err_tmp<=sen_tmp)
     {
         if(current_in_cmp>=4150 && current_in_cmp<19750) 
         {
             Motor_Stop(); 
         }
         else
         {
             
             if(current_in_cmp>=19750)
             {
                 MotorOpen_Control();
             }
             else
             {
                 if(current_in_cmp<4150)
                 {
                     MotorClose_Control(); 
                 }
                 else
                 {
                      Motor_Stop();
                 }
    
             } 
         }
     }
     else
     {
         if(current_out_cmp<current_in_cmp)
         {
             MotorOpen_Control();
         }
         else
         {
             MotorClose_Control();    
         }
     }
              	
}

void GetInputPos()
{
    float current_in = pData_Acquire->current_input;
    u8 curpos = (u8)((current_in - 4)/16*100+0.5);
    static u8 prepos;
    static u8 cnt;
    
    if(curpos==prepos)
    {
        cnt++;
        if(cnt>3)//400ms不变
        {
            pPosCTR->pos_input = (current_in - 4)/16.0f*100.0f;
						if(pPosCTR->pos_input>=98.5)
							pPosCTR->pos_input = 100.0;
						if(pPosCTR->pos_input<1.6)
							pPosCTR->pos_input = 0.0;
            cnt = 0;   
        }       
    }
    else
    {
        cnt = 0;
    }
    
    prepos = curpos;
    
}

void GetTargetPos()
{
    float pos_real = pData_Acquire->opening*100;
    float poserr;
    
    poserr = pPosCTR->pos_input-pos_real;
    
    if(poserr > (pPosCTR->offset_open_filter))//open
    {
        if(ctr_stat==CTR_RUN_CLOSE)
        {
            pPosCTR->pos_target = pos_real;
            Motor_Stop();
            ctr_stat = CTR_STOP;    
        }
        else
        {
            ctr_stat = CTR_RUN_OPEN;
            if(pPosCTR->pos_input < 98.5 )
            {
                pPosCTR->pos_target = pPosCTR->pos_input - pPosCTR->offset_open_filter;//计算目标值
            }
            else
            {
                pPosCTR->pos_target = 100.0;
            }
                
            
           // printf("OPEN!-poserr>openoffset openoffset= %d \r\n",pPosCTR->offset_open_filter);
        }
    }
    else
    {
        if(poserr < (pPosCTR->offset_close_filter))//close
        {
            if(ctr_stat==CTR_RUN_OPEN)
            {
                pPosCTR->pos_target = pos_real;
                Motor_Stop();
                ctr_stat = CTR_STOP;    
            }
            else            
            {
                ctr_stat = CTR_RUN_CLOSE;
                if(pPosCTR->pos_input >= 1.0)
                {
                    pPosCTR->pos_target = pPosCTR->pos_input - pPosCTR->offset_close_filter;//计算目标值
                }
                else
                {
                    pPosCTR->pos_target = 0.0;
                }
                    
              //  printf("CLOSE!-poserr<closeoffset closeoffset= %d\r\n",pPosCTR->offset_close_filter);
            }
        }
    }
    
}

void DistantAnalog_Control_Pos()
{
    float pos_real = pData_Acquire->opening*100;
    float poserr;
    
    GetInputPos(); 
    poserr = pPosCTR->pos_input-pos_real;
    
    switch(ctr_stat)
    {
        
        case CTR_READY:
        {
            if(poserr > (pPosCTR->offset_open_filter))//open
            {
                ctr_stat = CTR_RUN_OPEN;
                if(pPosCTR->pos_input < 98.5 )
                {
                    pPosCTR->pos_target = pPosCTR->pos_input - pPosCTR->offset_open_filter;//计算目标值
                }
                else
                {
                    pPosCTR->pos_target = 100.0;
                }

                printf("READY!-poserr>openoffset openoffset= %f \r\n",pPosCTR->offset_open_filter);
				
            }
            else
            {
                if(poserr < (pPosCTR->offset_close_filter))//close
                {
                    ctr_stat = CTR_RUN_CLOSE;
                    if(pPosCTR->pos_input >= 1.0)
                    {
                        pPosCTR->pos_target = pPosCTR->pos_input - pPosCTR->offset_close_filter;//计算目标值
                    }
                    else
                    {
                        pPosCTR->pos_target = 0.0;
                    }

                    printf("READY!-poserr<closeoffset closeoffset= %f\r\n",pPosCTR->offset_close_filter);
                }
                else
                {
                    if(pPosCTR->pos_input>=98.5 )//&& pos_real<100)
                    {
                        pPosCTR->pos_target = 100.0;
                        ctr_stat = CTR_RUN_OPEN;
                        //MotorOpen_Control();
                        //ctr_stat = CTR_READY;
                        printf("READY!-pos_target = 100.0 \r\n");
                    }
                    else if(pPosCTR->pos_input<1.0 )//&& pos_real>0)
                            {
                                pPosCTR->pos_target = 0.0;
                                ctr_stat = CTR_RUN_CLOSE;
                                //MotorClose_Control();
                                //ctr_stat = CTR_READY;
                                printf("READY!-pos_target = 0.0\r\n");
                            }
                          else 
                          {
                              Motor_Stop();
                              ctr_stat = CTR_READY;
                              printf("READY!- \r\n");
                          }
                }
            } 
            break;
        }
        
        case CTR_RUN_OPEN:
        { 
            if(pos_real < (pPosCTR->pos_target))
            {
                MotorOpen_Control();
                printf("OPEN! \r\n");
            }
            else
            {
                Motor_Stop();
                pPosCTR->pos_target = pos_real;
                ctr_stat = CTR_STOP;
                printf("OPEN Finish! \r\n");
            }
            GetTargetPos();
            
            ctr_stat_pre = CTR_RUN_OPEN;
            break; 
        }
        
        case CTR_RUN_CLOSE:
        { 
            if(pos_real > (pPosCTR->pos_target))
            {
                MotorClose_Control();
                printf("CLOSE! \r\n");
            }
            else
            {
                Motor_Stop();
                pPosCTR->pos_target = pos_real;
                ctr_stat = CTR_STOP;
                printf("CLOSE Finish! \r\n");
            }
            GetTargetPos();
            
            ctr_stat_pre = CTR_RUN_CLOSE;
            break;  
        }
        
        case CTR_STOP:
        {
            if(pPosCTR->pos_end_flag==1)
            {
                pPosCTR->pos_end_flag = 0;
                if(ctr_stat_pre==CTR_RUN_OPEN)
                {
									if(pPosCTR->pos_target != 100)
											UpdateOffset_Open(); 
                    printf("Update_OpenOffset OK! \r\n");                    
                }
                else 
                {
                    if(ctr_stat_pre==CTR_RUN_CLOSE)
                    {
											if(pPosCTR->pos_target != 0)
                        UpdateOffset_Close(); 
                        printf("Update_CloseOffset OK! \r\n"); 
                    }
                }
                
                ctr_stat = CTR_READY;
            }
            else
            {
                GetEndPos();
                
               printf("Wait EndPos ! \r\n");
            }
            break;
            
        }
        default:break;
    }

}


void UpdateOffset_Open()
{
    float offset;


    offset = pPosCTR->pos_end - pPosCTR->pos_target;
    pPosCTR->offset_open_filter = pPosCTR->offset_open_filter + K_OFFSET * (offset - pPosCTR->offset_open_filter);//0-100
	  if(pPosCTR->offset_open_filter<0 || pPosCTR->offset_open_filter>10.0)
			pPosCTR->offset_open_filter = 10.0;
	  IapWrite_OffsetOpen(pPosCTR->offset_open_filter);

    
}

void UpdateOffset_Close()
{
    float offset;

    offset = pPosCTR->pos_end - pPosCTR->pos_target;
    pPosCTR->offset_close_filter = pPosCTR->offset_close_filter + K_OFFSET * (offset - pPosCTR->offset_close_filter);
	  if(pPosCTR->offset_close_filter>0 || pPosCTR->offset_close_filter<-10.0)
			pPosCTR->offset_close_filter = -10;
	  IapWrite_OffsetClose(pPosCTR->offset_close_filter);
}

extern float min_opening;
void GetEndPos()//20ms 50Hz
{
    float pos = pData_Acquire->opening;//0-1
    //u8 curpos = (u8)(pos*100.0+0.5);//0-100
    static float prepos;
    static u8 count = 0;
	static u8 breakcnt = 0;
   
    if(fabs(prepos-pos)<0.005)
    {
        count++;
        if(count>50)// 
        {
            pPosCTR->pos_end = pos*100.0;//0-100
            pPosCTR->pos_end_flag = 1;
            count = 0;
        }
    }
    else
    {
        pPosCTR->pos_end_flag = 0;
        count = 0;
    }  

	ctr_stat = CTR_STOP;
	breakcnt++;	
    if(breakcnt>100 && pPosCTR->pos_end_flag == 0)
	{
		breakcnt = 0;
        pPosCTR->pos_end = pos*100.0;//0-100
        pPosCTR->pos_end_flag = 1;
//		ctr_stat = CTR_READY;
		
	}      
    prepos = pos;
 
}


void PosControl_Init()
{
    
    pPosCTR->pos_input = (pData_Acquire->current_input-4)*100/16;
//    pPosCTR->offset_open_filter = 10.0;
//    pPosCTR->offset_close_filter = -10.0;
//    pPosCTR->offset_close = -10.0;
//    pPosCTR->offset_open = 10.0;
    pPosCTR->pos_end_flag = 0;
    
}

















