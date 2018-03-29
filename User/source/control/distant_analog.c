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


#define CTR_STAT_STANDBY 	0
#define CTR_STAT_RUN     	1
#define CTR_END  			2
#define CTR_STAT_FULL_OPEN  3
#define CTR_STAT_FULL_CLOSE 4
#define CTR_STAT_OPEN       5
#define CTR_STAT_CLOSE      6
#define CTR_STAT_VIBRATE    7



PosControl_TypeDef PosCTR;
PosControl_TypeDef* pPosCTR = &PosCTR;
u8 ctr_stat = CTR_READY;
u8 ctr_stat_pre = CTR_READY;

extern  u16 code  sen_table[10];
#define MARGIN_SIZE 7
static float margin[MARGIN_SIZE]={6.4,3.2,1.6,0.8,0.4,0.2,0.1};//±5% ~ ±0.2%
static float *p_margin = &margin[2];
void SelectSortf(float*buf,u8 arrsize);
void RemoteAnalog_Control_Adaptive();
void FindMargin(float **p,float mar);

void UpdateOffset_Open();
void UpdateOffset_Close();
void DistantAnalog_Control_Pos();
extern  u16 code  sen_table[10];

void DistantAnalogMode_Init()
{
    Exti23_Disable();
    Motor_Stop();
    ReadSystemPara();
    Acquire_Data();
    
    ctr_stat = CTR_STAT_STANDBY;
    pPosCTR->pos_end_flag = 0;
    
    SystemMode_Detect();
    
    IR_Disable();
    LED_LSG_OFF
    LCD_YELLOW_OFF
    
    ERR_OFF
    
    delay_ms(500);
    
    FindMargin(&p_margin,pSystemParam->margin); 
    
   
}

void Set_InputLowHigh_Detect()//设置输入模式检测
{
	static u8 detectlow_cnt;
	static u8 detecthigh_cnt;
	
	if(KeyScan(KEY_CLOSE))
	{
		detectlow_cnt++;
		if(detectlow_cnt>10)
		{
			mode = MODE_SETINPUT_LOW;
			detectlow_cnt = 0;
		}
		
		
	}
	else if(KeyScan(KEY_OPEN))
	{
		detecthigh_cnt++;
		if(detecthigh_cnt>10)
		{
			mode = MODE_SETINPUT_HIGH;
			detecthigh_cnt = 0;
		}
		
		
	}
	else
	{	
		detectlow_cnt = 0;
		detecthigh_cnt = 0;
	}

}

/*****************远方模拟模式********************/
/*date:2015.9.12                     */
/*description:   */
void DistantAnalog_Mode()
{
    DistantAnalogMode_Init();
    while( mode == MODE_DISTANT_ANALOG )
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
            
			if(pSystemParam->sen != sen_table[9])
            {
                DistantAnalog_Control();
            }
            else
            {
                RemoteAnalog_Control_Adaptive();
				//printf(" -Distant_Control_Adaptive- \r\n");

            }
			//printf(" -Distant - \r\n");
        }

        if(timer2_60ms_flag == 1)
        {
            timer2_60ms_flag = 0;
            MotorStop_Delay_60ms();
			PWM_Update();
           
        }
        
        if(timer2_100ms_flag == 1)
        {
            timer2_100ms_flag = 0;
            Torque_Detect();//力矩检测
            LimitPosition_Output();
            LCD_DIS();
        }
        
        if(timer2_200ms_flag == 1)
        {
            timer2_200ms_flag = 0; 
            SystemMode_Detect();//远方现场模式检测
            if(mode == MODE_LOCAL || mode == MODE_DISTANT_DIGITAL)  
                break;
            
            #ifdef PHASE_SEQ
            PhaseSeq_Update();
            if(mode == MODE_LACK_PHASE)
			{
				Motor_Stop();
				break;  
			}
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
			Motor_PosRev();
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
	u16 low_val = pSystemParam->setinput_low;
	u16 high_val = pSystemParam->setinput_high;
	float in_val = pData_ADC->adcvalue_input_filtered;
	
	//float curpos = 100.0*(float)((in_val - (float)low_val )/(float)(high_val-low_val));
    
	float curpos = (pData_Acquire->current_input-4.0)*100.0/16.0;//static float prepos;
    //static u8 cnt;
	curpos = (float)((int)((curpos+0.05)*10))/10.0;

	if(curpos<0)
	{
		curpos = 0;
	}
	
	if(curpos>100)
		curpos = 100;
	
	pPosCTR->pos_input = curpos;
	
	if(pPosCTR->pos_input>=98.5)
		pPosCTR->pos_input = 100.0;
	if(pPosCTR->pos_input<1.6)
		pPosCTR->pos_input = 0.0;
	
    
}


void SeclectBestMargin(float **pp_curmargin,float iner)//选择最佳灵敏度
{
	float *p = margin;
	u8 i;
	
	if(iner<=margin[MARGIN_SIZE-1])
	{
		*pp_curmargin = &margin[MARGIN_SIZE-1];

		return;
	}
	
	if(iner>=margin[0])
	{
		*pp_curmargin = &margin[0];
		return;
	}
	
	for(i=0;i<MARGIN_SIZE;i++)
	{
		if(*p>iner)
			p++;
		else
		{		
			*pp_curmargin = p-1;
			break;
		}
		
	}

}

float GetInertiaAvr(float ine)
{
    static float inertia_buf[5];
    static u8 inertia_buf_cnt; 
    static float sortbuf[5];
    u8 i;
    float sum;
    float reval;
    static u8 first_flag=1;
    
    if(first_flag==1)
    {
        //memset(inertia_buf,ine,5);
        for(i=0;i<5;i++)
        {
            inertia_buf[i]=ine;
        }
        first_flag=0;
    }
    
    if(inertia_buf_cnt>4)
        inertia_buf_cnt = 0;
    
    inertia_buf[inertia_buf_cnt++] = ine;
    //UART1_SWToDebug();
    //printf("\r\ninertia_buf:%.2f, %.2f, %.2f, %.2f, %.2f, ",inertia_buf[0],inertia_buf[1],inertia_buf[2],inertia_buf[3],inertia_buf[4]);
    
    //memcpy(sortbuf,inertia_buf,5);
    for(i=0;i<5;i++)
    {
        sortbuf[i]=inertia_buf[i];
    }
    //printf("\r\nsortbuf_before:%.2f, %.2f, %.2f, %.2f, %.2f, ",sortbuf[0],sortbuf[1],sortbuf[2],sortbuf[3],sortbuf[4]);
    
    SelectSortf(sortbuf,5);
    //UART1_SWToDebug();
    //printf("\r\nsortbuf:%.2f, %.2f, %.2f, %.2f, %.2f, ",sortbuf[0],sortbuf[1],sortbuf[2],sortbuf[3],sortbuf[4]);
    
    sum = 0;
    for(i=1;i<4;i++)
    {
        sum += sortbuf[i];
    }
    
    reval = sum/3.0;
    return reval;  
    
}

void FindMargin(float **p,float mar)
{
    u8 i;
    for(i=0;i<MARGIN_SIZE;i++)
    {
        if(mar==margin[i])
        {
            *p = &margin[i];
            break;
        }
        
    }
    if(i>=MARGIN_SIZE)
    {
        *p = &margin[0];
    }
    
}

extern bit close0_flag;
extern bit open100_flag;
void RemoteAnalog_Control_Adaptive()
{
    
	static float interia = 1;
	static u8 ctr_end_cnt;
    static float start_pos;
    static float stop_pos;
    static u8 run_enable_cnt;
    static int vibrate_cnt;
    //static u8 vibrate_input_cnt;
    static float input_pos_pre;
    float pos_real;
    float poserr;
    
    float rd_mar;
	
	
	switch(ctr_stat)
	{
		case CTR_STAT_STANDBY:
		{
            Motor_Stop();
			GetInputPos();
			pos_real = (float)((int)((pData_Acquire->opening*100+0.05)*10))/10.0;
			poserr = pPosCTR->pos_input - pos_real;
			
            if(pPosCTR->pos_input<1.0)
            {
                ctr_stat = CTR_STAT_FULL_CLOSE;
                break;
            }
            if(pPosCTR->pos_input>99.0)
            {
                ctr_stat = CTR_STAT_FULL_OPEN;
                break;  
            }
            
			if(fabs(poserr) > (*p_margin))
			{
                run_enable_cnt++;
                if(run_enable_cnt>50)
                {
                    run_enable_cnt = 0;
                    
                    if(poserr>0)
                    {
                        ctr_stat = CTR_STAT_OPEN;
                        start_pos = pos_real;
                        //printf("\r\nOPEN poserr=%.2f",poserr);
                        
                    }
                    else
                    {
                        ctr_stat = CTR_STAT_CLOSE;
                        start_pos = pos_real;
                        //printf("\r\nCLOSE poserr=%.2f",poserr);
                    }
                    
                    //printf("\r\n----vibrate_cnt=%d",(int)vibrate_cnt);
                    
                    if(input_pos_pre == pPosCTR->pos_input)
                    {
                        vibrate_cnt++;
                        if(vibrate_cnt>2)
                        {
                            if(*p_margin<margin[0])
                            {
                                p_margin--;
                                // printf("\r\nCLOSE p_margin=%f",*p_margin);
                            }
                            vibrate_cnt = 0;
                        }

                    }
                    else
                    {
                        vibrate_cnt=0;
                    }
                    
                    input_pos_pre = pPosCTR->pos_input;
                    
                }
            
                
			}
            else
            {
                run_enable_cnt = 0;
            }
            
        //printf("\r\n STANDBY RELAY_ON=%d phase_seq = %d POS_REV = %d RELAY_A=%d RELAY_B=%d \r\n",(int)RELAY_ON,(int)phase_seq,(int)POS_REV,(int)RELAY_A,(int)RELAY_B);
			
			break;
		}
        
        case CTR_STAT_OPEN:
        {
			GetInputPos();
			pos_real = (float)((int)((pData_Acquire->opening*100+0.05)*10))/10.0;
			poserr = pPosCTR->pos_input - pos_real;
            
            if(pPosCTR->pos_input<1.0)
            {
                ctr_stat = CTR_STAT_FULL_CLOSE;
                break;
            }
            if(pPosCTR->pos_input>99.0)
            {
                ctr_stat = CTR_STAT_FULL_OPEN;
                break;  
            }
            
            if(poserr>(*p_margin))
            {
                MotorOpen_Control();
            }
            else
            {
				Motor_Stop();
				ctr_stat = CTR_END;
                stop_pos = pos_real;
            }
            //printf("\r\nctr_stat=OPEN  pos_input=%.2f  pos_real=%.2f  poserr=%.2f  p_margin=%.2f",pPosCTR->pos_input,pos_real,poserr,*p_margin);
            //printf("\r\n OPEN RELAY_ON=%d phase_seq = %d POS_REV = %d RELAY_A=%d RELAY_B=%d \r\n",(int)RELAY_ON,(int)phase_seq,(int)POS_REV,(int)RELAY_A,(int)RELAY_B);
            break;
        }
        
        case CTR_STAT_CLOSE:
        {
			GetInputPos();
			pos_real = (float)((int)((pData_Acquire->opening*100+0.05)*10))/10.0;
			poserr = pPosCTR->pos_input - pos_real;
            
            if(pPosCTR->pos_input<1.0)
            {
                ctr_stat = CTR_STAT_FULL_CLOSE;
                break;
            }
            if(pPosCTR->pos_input>99.0)
            {
                ctr_stat = CTR_STAT_FULL_OPEN;
                break;  
            }
            
            if((poserr)< (-1*(*p_margin)))
            {
                MotorClose_Control();
            }
            else
            {
				Motor_Stop();
				ctr_stat = CTR_END;
                stop_pos = pos_real;
            } 
            //printf("\r\nctr_stat=CLOSE  pos_input=%.2f  pos_real=%.2f  poserr=%.2f  p_margin=%.2f",pPosCTR->pos_input,pos_real,poserr,*p_margin);
            //printf("\r\n RELAY_ON=%d phase_seq = %d POS_REV = %d RELAY_A=%d RELAY_B=%d \r\n",(int)RELAY_ON,(int)phase_seq,(int)POS_REV,(int)RELAY_A,(int)RELAY_B);
            break;
        }
		
		case CTR_END:
		{
			Motor_Stop();
			pos_real = (float)((int)((pData_Acquire->opening*100+0.05)*10))/10.0;
			poserr = pPosCTR->pos_input - pos_real;
			ctr_end_cnt++;
			if(ctr_end_cnt>150)//停止3秒
			{
				ctr_end_cnt = 0;
                if(fabs(start_pos-stop_pos)>5)
                {
                    interia = fabs(stop_pos-pos_real);
                    //printf("\r\nctr_stat=END ,pos_input=%.2f, pos_real=%.2f, poserr=%.2f, p_margin=%.2f, interia=%.2f",pPosCTR->pos_input,pos_real,poserr,*p_margin,interia);

                    interia = GetInertiaAvr(interia);
                    //printf("\r\nNew interia=%.2f",interia);
                    
                    SeclectBestMargin(&p_margin,interia);
                    //printf("\r\nNew_Margin=%.2f\r\n",*p_margin);
                    
                    if(*p_margin != pSystemParam->margin)
                    {
                        pSystemParam->margin = *p_margin;
                        IapWrite_Margin(*p_margin);
                        rd_mar = IapRead_Margin();
                       // printf("\r\n Read Margin=%.2f",rd_mar);
                    }
                }
                ctr_stat = CTR_STAT_STANDBY;
                //printf("\r\nctr_stat=STANDBY ");

			}
        //printf("\r\n END RELAY_ON=%d phase_seq = %d POS_REV = %d RELAY_A=%d RELAY_B=%d \r\n",(int)RELAY_ON,(int)phase_seq,(int)POS_REV,(int)RELAY_A,(int)RELAY_B);
			break;
		}
        
        case CTR_STAT_FULL_CLOSE:
        {

			GetInputPos();
			pos_real = (float)((int)((pData_Acquire->opening*100+0.05)*10))/10.0;
			poserr = pPosCTR->pos_input - pos_real;

            #ifdef TRAVEL_PROTECT_MACHINE	
            if(!close0_flag && pPosCTR->pos_input<1.0)
            {
                MotorClose_Control();
               // printf("\r\n FULL CLOSE !");
                
            }
            #else
            if(pos_real>0 && !close0_flag && pPosCTR->pos_input<1.0)
            {
                MotorClose_Control();
               // printf("\r\n FULL CLOSE !");
            }
            #endif
            else
            {
                Motor_Stop();
                ctr_stat = CTR_STAT_STANDBY;
               // printf("\r\n FULL CLOSE To Standby !");
            }
      // printf("\r\n FULLCLOSE phase_seq = %d POS_REV = %d RELAY_A=%d RELAY_B=%d \r\n",(int)phase_seq,(int)POS_REV,(int)RELAY_A,(int)RELAY_B);
                
            break;
        }
		
        case CTR_STAT_FULL_OPEN:
        {
			GetInputPos();
			pos_real = (float)((int)((pData_Acquire->opening*100+0.05)*10))/10.0;
			poserr = pPosCTR->pos_input - pos_real;
            
            #ifdef TRAVEL_PROTECT_MACHINE	
            if(!open100_flag && pPosCTR->pos_input>99.0)
            {
                MotorOpen_Control();
             //  printf("\r\n FULL OPEN !");
            }
            #else
            if(pos_real<100 && !open100_flag && pPosCTR->pos_input>99.0)
            {
                MotorOpen_Control();
               //printf("\r\n FULL OPEN !");
            }
            #endif
            else
            {
                Motor_Stop();
                ctr_stat = CTR_STAT_STANDBY;
               // printf("\r\n FULL OPEN To Standby !");
            }
        //printf("\r\n FULLOPEN RELAY_ON=%d phase_seq = %d POS_REV = %d RELAY_A=%d RELAY_B=%d \r\n",(int)RELAY_ON,(int)phase_seq,(int)POS_REV,(int)RELAY_A,(int)RELAY_B);
            break;
        }  
        
        default:
		{
			ctr_stat = CTR_STAT_STANDBY;
           // printf("\r\n Default To Standby !");
			break;
		}
		
	}
		
}

//void GetInputPos()
//{
//    float current_in = pData_Acquire->current_input;
//    u8 curpos = (u8)((current_in - 4)/16*100+0.5);
//    static u8 prepos;
//    static u8 cnt;
//    
//    if(curpos==prepos)
//    {
//        cnt++;
//        if(cnt>3)//400ms不变
//        {
//            pPosCTR->pos_input = (current_in - 4)/16.0f*100.0f;
//						if(pPosCTR->pos_input>=98.5)
//							pPosCTR->pos_input = 100.0;
//						if(pPosCTR->pos_input<1.6)
//							pPosCTR->pos_input = 0.0;
//            cnt = 0;   
//        }       
//    }
//    else
//    {
//        cnt = 0;
//    }
//    
//    prepos = curpos;
//    
//}

//void GetTargetPos()
//{
//    float pos_real = pData_Acquire->opening*100;
//    float poserr;
//    
//    poserr = pPosCTR->pos_input-pos_real;
//    
//    if(poserr > (pPosCTR->offset_open_filter))//open
//    {
//        if(ctr_stat==CTR_RUN_CLOSE)
//        {
//            pPosCTR->pos_target = pos_real;
//            Motor_Stop();
//            ctr_stat = CTR_STOP;    
//        }
//        else
//        {
//            ctr_stat = CTR_RUN_OPEN;
//            if(pPosCTR->pos_input < 98.5 )
//            {
//                pPosCTR->pos_target = pPosCTR->pos_input - pPosCTR->offset_open_filter;//计算目标值
//            }
//            else
//            {
//                pPosCTR->pos_target = 100.0;
//            }
//                
//            
//           // printf("OPEN!-poserr>openoffset openoffset= %d \r\n",pPosCTR->offset_open_filter);
//        }
//    }
//    else
//    {
//        if(poserr < (pPosCTR->offset_close_filter))//close
//        {
//            if(ctr_stat==CTR_RUN_OPEN)
//            {
//                pPosCTR->pos_target = pos_real;
//                Motor_Stop();
//                ctr_stat = CTR_STOP;    
//            }
//            else            
//            {
//                ctr_stat = CTR_RUN_CLOSE;
//                if(pPosCTR->pos_input >= 1.0)
//                {
//                    pPosCTR->pos_target = pPosCTR->pos_input - pPosCTR->offset_close_filter;//计算目标值
//                }
//                else
//                {
//                    pPosCTR->pos_target = 0.0;
//                }
//                    
//              //  printf("CLOSE!-poserr<closeoffset closeoffset= %d\r\n",pPosCTR->offset_close_filter);
//            }
//        }
//    }
//    
//}

//void DistantAnalog_Control_Pos()
//{
//    float pos_real = pData_Acquire->opening*100;
//    float poserr;
//    
//    GetInputPos(); 
//    poserr = pPosCTR->pos_input-pos_real;
//    
//    switch(ctr_stat)
//    {
//        
//        case CTR_READY:
//        {
//            if(poserr > (pPosCTR->offset_open_filter))//open
//            {
//                ctr_stat = CTR_RUN_OPEN;
//                if(pPosCTR->pos_input < 98.5 )
//                {
//                    pPosCTR->pos_target = pPosCTR->pos_input - pPosCTR->offset_open_filter;//计算目标值
//                }
//                else
//                {
//                    pPosCTR->pos_target = 100.0;
//                }

//                printf("READY!-poserr>openoffset openoffset= %f \r\n",pPosCTR->offset_open_filter);
//				
//            }
//            else
//            {
//                if(poserr < (pPosCTR->offset_close_filter))//close
//                {
//                    ctr_stat = CTR_RUN_CLOSE;
//                    if(pPosCTR->pos_input >= 1.0)
//                    {
//                        pPosCTR->pos_target = pPosCTR->pos_input - pPosCTR->offset_close_filter;//计算目标值
//                    }
//                    else
//                    {
//                        pPosCTR->pos_target = 0.0;
//                    }

//                    printf("READY!-poserr<closeoffset closeoffset= %f\r\n",pPosCTR->offset_close_filter);
//                }
//                else
//                {
//                    if(pPosCTR->pos_input>=98.5 )//&& pos_real<100)
//                    {
//                        pPosCTR->pos_target = 100.0;
//                        ctr_stat = CTR_RUN_OPEN;
//                        //MotorOpen_Control();
//                        //ctr_stat = CTR_READY;
//                        printf("READY!-pos_target = 100.0 \r\n");
//                    }
//                    else if(pPosCTR->pos_input<1.0 )//&& pos_real>0)
//                            {
//                                pPosCTR->pos_target = 0.0;
//                                ctr_stat = CTR_RUN_CLOSE;
//                                //MotorClose_Control();
//                                //ctr_stat = CTR_READY;
//                                printf("READY!-pos_target = 0.0\r\n");
//                            }
//                          else 
//                          {
//                              Motor_Stop();
//                              ctr_stat = CTR_READY;
//                              printf("READY!- \r\n");
//                          }
//                }
//            } 
//            break;
//        }
//        
//        case CTR_RUN_OPEN:
//        { 
//            if(pos_real < (pPosCTR->pos_target))
//            {
//                MotorOpen_Control();
//                printf("OPEN! \r\n");
//            }
//            else
//            {
//                Motor_Stop();
//                pPosCTR->pos_target = pos_real;
//                ctr_stat = CTR_STOP;
//                printf("OPEN Finish! \r\n");
//            }
//            GetTargetPos();
//            
//            ctr_stat_pre = CTR_RUN_OPEN;
//            break; 
//        }
//        
//        case CTR_RUN_CLOSE:
//        { 
//            if(pos_real > (pPosCTR->pos_target))
//            {
//                MotorClose_Control();
//                printf("CLOSE! \r\n");
//            }
//            else
//            {
//                Motor_Stop();
//                pPosCTR->pos_target = pos_real;
//                ctr_stat = CTR_STOP;
//                printf("CLOSE Finish! \r\n");
//            }
//            GetTargetPos();
//            
//            ctr_stat_pre = CTR_RUN_CLOSE;
//            break;  
//        }
//        
//        case CTR_STOP:
//        {
//            if(pPosCTR->pos_end_flag==1)
//            {
//                pPosCTR->pos_end_flag = 0;
//                if(ctr_stat_pre==CTR_RUN_OPEN)
//                {
//									if(pPosCTR->pos_target != 100)
//											UpdateOffset_Open(); 
//                    printf("Update_OpenOffset OK! \r\n");                    
//                }
//                else 
//                {
//                    if(ctr_stat_pre==CTR_RUN_CLOSE)
//                    {
//											if(pPosCTR->pos_target != 0)
//                        UpdateOffset_Close(); 
//                        printf("Update_CloseOffset OK! \r\n"); 
//                    }
//                }
//                
//                ctr_stat = CTR_READY;
//            }
//            else
//            {
//                GetEndPos();
//                
//               printf("Wait EndPos ! \r\n");
//            }
//            break;
//            
//        }
//        default:break;
//    }

//}





















