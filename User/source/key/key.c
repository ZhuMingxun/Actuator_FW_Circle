/**********************************************************
�ļ����ƣ�key.c
����    ������ѫ
˵��    �������ֶ��궨 ���Զ��궨 0%~100%
***********************************************************/

#include "timer.h"
#include "GPIO.h"
#include "IORedefine.h"
#include "key.h"
#include "data_input.h"
#include "data_save.h"
#include "motor.h"
#include "control_new.h"
#include "current_output.h"
#include "LCD.h"
#include "delay.h"
/*************************************************************
                      ��������
**************************************************************/
u8 key_status = KEY_STATUS_IDLE;
u8 key_num = KEY_NUM_NONE;
u8 key_cmd = KEY_CMD_NONE;
u8 key_cmd_done = 1;
u8 set_status;
u16 key_cnt = 0;
//u16 sen = SEN0_1MA;
//u16 pwm_cnt_cal;
u8 def_ok = 0;
u16  alarm_cnt;
u8  opening_alarm_1st;
u8  alarm_start_flag = 0;
float opening_rt;

/*************************************************************
                         ��������
**************************************************************/
void KeyIO_Config()
{
    GPIO_InitTypeDef	GPIO_InitStructure;
    
    GPIO_InitStructure.Pin  = GPIO_Pin_7;
    GPIO_InitStructure.Mode = GPIO_OUT_PP;//�������
    GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);
    
    GPIO_InitStructure.Mode = GPIO_HighZ;
    GPIO_InitStructure.Pin  = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Inilize(GPIO_P5,&GPIO_InitStructure);
    
    GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_Inilize(GPIO_P4,&GPIO_InitStructure);

    GPIO_InitStructure.Pin  = GPIO_Pin_3 | GPIO_Pin_4;    GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);
   
}

/********************��ʱ��1����********************/
void Timer1_Config()
{
    TIM_InitTypeDef		TIM_InitStructure;
    TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;
    TIM_InitStructure.TIM_Polity    = PolityLow;
    TIM_InitStructure.TIM_Interrupt = ENABLE;
    TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;
    TIM_InitStructure.TIM_ClkOut    = DISABLE;
    TIM_InitStructure.TIM_Value     = T1_CNT;
    TIM_InitStructure.TIM_Run       = ENABLE;
    Timer_Inilize(Timer1,&TIM_InitStructure);
    EA = 1;
}
/******************************��ֵ��⺯��************************************/
u8 Get_KeyNumber()
{
    //u8 tmp;
    u8 keynum_tmp;
    
//    keynum_tmp = (P4 & 0x01);//P40Ϊ100%
//    tmp = (P5 & 0x30);       //P55ΪSET_SEN,P54Ϊ0%
//    tmp >>= 3;
//    keynum_tmp |= tmp;
    
    keynum_tmp = 4 * (u8)SET_SEN + 2 * (u8)OPENING_0_DEF + 1 * (u8)OPENING_100_DEF; 
    
    switch(keynum_tmp)
    {
        case KEY_NUM_100:   break;  
        case KEY_NUM_0:     break;    
        case KEY_NUM_SEN:   break;
        default:keynum_tmp = KEY_NUM_NONE;break;
    }        
    
    return keynum_tmp;
       
}
/****************************����ɨ�躯��************************************/
u8 Key_Scan()
{
    u8 keynum_tmp;  
    u8 key_cmd_tmp = KEY_CMD_NONE;  //��������
    
    keynum_tmp = Get_KeyNumber();  //��ȡ���µļ�ֵ
    
    switch(key_status)
    {
        case KEY_STATUS_IDLE:
        {
            if(keynum_tmp != KEY_NUM_NONE)//�м�����
            {
                key_num = keynum_tmp; //���¼�ֵ
                key_status = KEY_STATUS_DOWN; //��ת����һ״̬ 
            }
           
            break;
        }
        
        case KEY_STATUS_DOWN:
        {
            if((key_num | keynum_tmp) == key_num) //ͬһ����û�з�
            {
                key_cnt = 0;
                key_status = KEY_STATUS_ACK;  //����ȷ��״̬
            }
            else //������Ϊ���ţ��ص�����״̬����ֵΪ0
            {
                key_num = KEY_NUM_NONE;
                key_status = KEY_STATUS_IDLE;
            }
            
            break;
            
        }
        
        case KEY_STATUS_ACK:
        {
            if((key_num | keynum_tmp) == key_num)
            {
                key_cnt++;
                if(key_cnt >= KEY_CNT_LONG)//����3s��Ϊ����
                {
                    switch(key_num)
                    {
                        case KEY_NUM_100:key_cmd_tmp = KEY_CMD_100_LONG;break;
                        case KEY_NUM_0:  {key_cmd_tmp = KEY_CMD_0_LONG;break;}
                        case KEY_NUM_SEN:key_cmd_tmp = KEY_CMD_SEN_LONG;break;
                        default:break;
                    }
                    key_status = KEY_STATUS_ACK; 
                }
                else
                {
                    key_status = KEY_STATUS_ACK;//����δ�ţ�����ԭ״̬
                }                    
            }
            else
            {
                key_status = KEY_STATUS_UP;  //��⵽�ſ���������һ״̬                      
            }
            break;
        }
                        
        case KEY_STATUS_UP:
        {
            if((key_num | keynum_tmp) != key_num)//�ߵ�ƽ ȷ���ɿ�����
            {
                if(key_cnt >= KEY_CNT_SHORT && key_cnt < KEY_CNT_LONG ) //300ms-3s����Ϊ�̰�
                {
                    switch(key_num)
                    {
                        case KEY_NUM_100:key_cmd_tmp = KEY_CMD_100_SHORT;   break;
                        case KEY_NUM_0:  key_cmd_tmp = KEY_CMD_0_SHORT;     break;
                        case KEY_NUM_SEN:key_cmd_tmp = KEY_CMD_SEN_SHORT;   break;
                        default:break;
                    }
                }
                else
                { 
                    if(key_cnt >= KEY_CNT_LONG)  //�����ɿ�
                    {
                        switch(key_num)
                        {
                            case KEY_NUM_100:key_cmd_tmp = KEY_CMD_100_UP;   break;
                            case KEY_NUM_0:  key_cmd_tmp = KEY_CMD_0_UP;     break;
                            case KEY_NUM_SEN:key_cmd_tmp = KEY_CMD_SEN_UP;   break;
                            default:break;
                        }     
                    }
                }
                key_cnt = 0;
                key_num = KEY_NUM_NONE;
                key_status = KEY_STATUS_IDLE;   //״̬�ص�����״̬����ֵ�ͼ���ֵ����

            }
            else
            {
                if(key_cnt < KEY_CNT_LONG)   //��Ϊ���Żص�ǰһ״̬
                {
                    key_status = KEY_STATUS_ACK;
                }
                else
                {
                    switch(key_num)  //����δ��
                    {
                        case KEY_NUM_100:key_cmd_tmp = KEY_CMD_100_LONG;break;
                        case KEY_NUM_0:  key_cmd_tmp = KEY_CMD_0_LONG;break;
                        case KEY_NUM_SEN:key_cmd_tmp = KEY_CMD_SEN_LONG;break;
                        default:break;
                    }
                }                    
            }                
            break;
        }
        default:break;
    }
    return key_cmd_tmp; 
  
}
/*********************���������⺯��*********************/
//void KeyCmd_Detect()
//{
//    if(key_cmd_done == 0)
//    {
//        switch(key_cmd)
//        {
//            case KEY_CMD_0_LONG:    mode = MODE_DEF_ZERO;break; 
//            case KEY_CMD_100_LONG:  mode = MODE_DEF_HUNDRED;break;
//            case KEY_CMD_SEN_SHORT: mode = MODE_SET_SEN;break;
//            case KEY_CMD_SEN_LONG:  mode = MODE_OUT_CAL;break;
//            default:break;      
//        }
//        key_cmd_done = 1;
//    }
//}

/********0%�궨������*********/
//void Def0_Proces()
//{
//    u16 u16_data_tmp;

//    u16_data_tmp = Get_Vx_adc(VA_CH);
//    IapWrite_Opening0_adc(u16_data_tmp);  
//}

/**********100%�궨������****************/
//void Def100_Proces()
//{
//    u16 u16_data_tmp;
//            
//    u16_data_tmp = Get_Vx_adc(VA_CH);
//    IapWrite_Opening100_adc(u16_data_tmp);
// 
//}

/******************���������ú���***************************/
//void Sen_Proces()
//{
//    u8 i,sen_ok = 0;
//    u16 u16_sen_tmp;
//    u16_sen_tmp = IapRead_Sen();
//    if(u16_sen_tmp == 0xff) u16_sen_tmp = 10;
//    
//    p_sen = &sen_table[0];
//    for(i=0;i<6;i++)
//    {
//        if(u16_sen_tmp == *p_sen)   break;
//        else
//        {
//            if(i == 5)
//            {
//                p_sen = &sen_table[0];
//            }
//            else    p_sen++;
//        }
//                
//    }     
//    
//    while(sen_ok == 0)
//    {
//        if(key_cmd_done == 0)
//        {
//            switch(key_cmd)
//            {
//                case KEY_CMD_0_SHORT:
//                {
//                    if(*p_sen == sen_table[0])  p_sen = sen_table+5;
//                    else    p_sen--;
//                    
//                    u16_sen_tmp = *p_sen;
//                    IapWrite_Sen(u16_sen_tmp);
//                    break;
//                }
//                
//                case KEY_CMD_100_SHORT:
//                {
//                    if(*p_sen == sen_table[5])  p_sen = &sen_table[0];
//                    else    p_sen++;
//                    
//                    u16_sen_tmp = *p_sen;
//                    IapWrite_Sen(u16_sen_tmp);  
//                    break;
//                }

//                case KEY_CMD_SEN_SHORT:case KEY_CMD_SEN_UP:
//                {
//                    sen_ok = 1;
//                    break;
//                }
//                default:break;
//            }                
//            key_cmd_done = 1;
//        }
//        Sen_DIS(u16_sen_tmp);
//    }
//}    
        

        
    
  
  


/*****************У׼����ȴ�����***************************/
//void Cal_Lock_Wait()
//{
//     while(SET_SEN == 0 );
//     
//}

/*****************************************У׼����*******************************************/

//void Cal_Res()
//{
//    float vcc_tmp,vi_tmp;
//    u16 pwm_cal_cnt;
//    float res_out_tmp,res_in_tmp;
//    u8 cal_ok = 0;
//    u8 sen_up = 0;
//    
//    while(SET_SEN == 0 );
//    
//    vcc_tmp = Get_Vcc();
//    pwm_cal_cnt = (u16)(0.02 * PWM_DUTY * 199.5 / vcc_tmp);  //������ۼ���ֵ
//    PWM_Cal(pwm_cal_cnt);
//    
//    while(cal_ok == 0)
//    {    
//        if(key_cmd_done == 0)
//        {
//            switch(key_cmd)
//            {
//                case KEY_CMD_0_SHORT:
//                {
//                    pwm_cal_cnt--;
//                    PWM_Cal(pwm_cal_cnt);
//                    break;
//                }
//                
//                case KEY_CMD_100_SHORT:
//                {
//                    pwm_cal_cnt++;
//                    PWM_Cal(pwm_cal_cnt);
//                    break;
//                }
//                
//                case KEY_CMD_SEN_SHORT:
//                {
//                    vcc_tmp = Get_Vcc();
//                    res_out_tmp = (vcc_tmp * pwm_cal_cnt) / (0.02 * PWM_DUTY);
//                    IapWrite_R_out(res_out_tmp);
//                    cal_ok = 1;
//                    break;
//                }
//                
//                case KEY_CMD_SEN_UP:
//                {
//                    
//                    vcc_tmp = Get_Vcc();
//                    res_out_tmp = (vcc_tmp * pwm_cal_cnt) / (0.02 * PWM_DUTY);
//                    IapWrite_R_out(res_out_tmp);
//                    
//                    vi_tmp = Get_Vi();
//                    res_in_tmp = vi_tmp/0.02;
//                    IapWrite_R_in(res_in_tmp);
//                    if(sen_up == 0) {sen_up++;Delay500ms();} //�������ʱ�ĳ������˳�ʱ�ĳ���
//                    else    cal_ok = 1;
//                    break;
//                }
//                default:break;
//            }
//            key_cmd_done = 1;
//        }
//    }
//}


/*********************10s������⺯��*********************/
//void Alarm_Detect()
//{
//    u8 u8_opening_tmp;

//    if(motor_status != MOTOR_STATUS_STOP)
//    {
//        if(alarm_start_flag == 0)
//        {
//            
//            opening_alarm_1st = (u8)(100 * opening_rt + 0.5);
//            alarm_start_flag = 1;
//            alarm_cnt = 0;
//        }
//        else
//        { 
//            u8_opening_tmp =  (u8)(100 * opening_rt + 0.5);
//            
//            if(u8_opening_tmp != opening_alarm_1st) //��⵽�仯�����¼��
//            {
//                alarm_start_flag = 0;
//                alarm_cnt = 0;
//            }
//            else    alarm_cnt++;

//        }
//        
//        if(alarm_cnt >= ALARM_TIME_CNT) //�������״̬��������10s���ޱ仯������
//        {
//            alarm_cnt = 0;
//            alarm_start_flag = 0;
//            
//            if(motor_status == MOTOR_STATUS_CLOSE)  {alarm_cleflag = 1;} //�ض���ʱ��������ر���ģʽ
//            if(motor_status == MOTOR_STATUS_OPEN)   {alarm_opnflag = 1;} //������ʱ���뿪���ر���ģʽ

//            

//        }     
//    }
//    else
//    {
//        alarm_cnt = 0;
//        alarm_start_flag = 0;
//    }

//}
/****************�������Գ���********************/
//void main()
//{
//    KeyIO_Config();
//    Timer1_Config();
//    while(1)
//    {
//        if(key_cmd_done == 0){
//        switch(key_cmd)
//        {
//            case  KEY_CMD_SEN_SHORT:     LED_LSG = !LED_LSG;break;
//            case  KEY_CMD_SEN_LONG:      LED_LSG = 0;break;
//            case  KEY_CMD_SEN_UP:        LED_LSG = !LED_LSG;break;
//            case  KEY_CMD_0_SHORT:       LED_LSG = !LED_LSG;break;
//            case  KEY_CMD_0_LONG:        LED_LSG = 0;break;
//            case  KEY_CMD_0_UP:          LED_LSG = !LED_LSG; break;
//            case  KEY_CMD_100_SHORT:     LED_LSG = !LED_LSG; break;
//            case  KEY_CMD_100_LONG:      LED_LSG = 0;break;
//            case  KEY_CMD_100_UP:        LED_LSG = !LED_LSG; break;
//            default:break;
//        }
//        key_cmd_done = 1;
//    }
//    }
//   
//}

/********************* Timer1�жϺ���************************/
void timer1_int (void) interrupt TIMER1_VECTOR
{      
    //if(key_cmd_done == 1)  2015.2.18 ע��
    //{
        key_cmd = Key_Scan();
        key_cmd_done = 0;
    //}
//    if(LOCAL_STOP == LOCAL_STOP_ON )  //�ֳ��궨����
//    {
//        if(LOCAL_OPEN == LOCAL_OPEN_ON || LOCAL_CLOSE == LOCAL_CLOSE_ON)  locdef_cnt++;
//        else locdef_cnt = 0;
//    }
    
}


//    switch(set_status)
//    {
//        case SET_STATUS_IDLE:
//        {
//            switch(key_cmd)
//            {
//                case KEY_CMD_NONE:          break;
//                case KEY_CMD_100_SHORT:     break;
//                case KEY_CMD_0_SHORT:       break;
//                case KEY_CMD_0_LONG:        set_status = SET_STATUS_DEF0;         break;
//                case KEY_CMD_100_LONG:      set_status = SET_STATUS_DEF100;       break;
//                case KEY_CMD_SEN_SHORT:     set_status = SET_STATUS_SEN;          break;
//                case KEY_CMD_SEN_LONG:      set_status = SET_STATUS_CAL_LCK;      break; 
//                default:break;                
//            } 
//        }
//        
//        case SET_STATUS_DEF0 :
//        {
//            Set0_DIS();
//            adc_tmp = Get_Vx_adc(VA_CH);
//            IapWrite_Opening0_adc(adc_tmp);
//            
//            if(key_cmd == KEY_CMD_0_UP)
//            {
//                set_status = SET_STATUS_IDLE; 
//            }
//            break;
//        }
//        
//        case SET_STATUS_DEF100:
//        {
//            Set100_DIS();
//            adc_tmp = Get_Vx_adc(VA_CH);
//            IapWrite_Opening100_adc(adc_tmp);
//            
//            if(key_cmd == KEY_CMD_100_UP)
//            {
//                set_status = SET_STATUS_IDLE;
//            }
//            break;
//        }
//        
//        case SET_STATUS_SEN:
//        {
//            sen_tmp = IapRead_Sen();
//            Sen_DIS();
//            switch(key_cmd)
//            {
//                case KEY_CMD_100_SHORT:case KEY_CMD_100_UP:
//                {
//                    p_sen = &sen_table[0];
//                    for(i=0;i<6;i++)
//                    {
//                        if(*p_sen == sen_tmp)
//                        {
//                            if(i == 5)
//                            {
//                                sen_tmp = *(p_sen - 5);
//                            }
//                            else
//                            {                                
//                                sen_tmp = *(p_sen + 1);
//                            }
//                            break;
//                        }
//                        p_sen++;
//                    }
//                    IapWrite_Sen(sen_tmp);
//                    break;
//                }
//                
//                case KEY_CMD_0_SHORT:case KEY_CMD_0_UP:
//                {
//                    p_sen = &sen_table[0];
//                    for(i=0;i<6;i++)
//                    {
//                        if(*p_sen == sen_tmp)
//                        {
//                            if(i == 0)
//                            {
//                                sen_tmp = *(p_sen + 5);
//                            }
//                            else
//                            {                                
//                                sen_tmp = *(p_sen - 1);
//                            }
//                            break;
//                        }
//                        p_sen++;
//                    }
//                    IapWrite_Sen(sen_tmp);
//                    break;
//                }
//                
//                case KEY_CMD_SEN_SHORT:case KEY_CMD_SEN_UP:
//                {
//                    IapWrite_Sen(sen_tmp);
//                    set_status = SET_STATUS_IDLE;
//                    break;
//                }                              

//                default:break;                
//            } 
//        }
//        
//        case SET_STATUS_CAL_LCK:
//        {
//            LCD_FullDisplay();
//            vcc_tmp = Get_Vcc();
//            pwm_cnt_cal = (u16)(0.02 * PWM_DUTY * 199.5 / vcc_tmp);  //������ۼ���ֵ
//            PWM_Cal(pwm_cnt_cal);
//            
//            if(key_cmd == KEY_CMD_SEN_UP)
//            {
//                set_status = SET_STATUS_CAL;
//            }
//          break;  
//        }
//        
//        case SET_STATUS_CAL:
//        {
//            LCD_FullDisplay();

//            switch(key_cmd)
//            {
//                case KEY_CMD_100_SHORT:case KEY_CMD_100_LONG:
//                {
//                    pwm_cnt_cal++;
//                    PWM_Cal(pwm_cnt_cal);
//                   break;
//                }                    
//                case KEY_CMD_0_SHORT:case KEY_CMD_0_LONG:
//                {
//                    pwm_cnt_cal--;
//                    PWM_Cal(pwm_cnt_cal);
//                    
//                   break; 
//                }                    

//                case KEY_CMD_SEN_SHORT:
//                {
//                    vcc_tmp = Get_Vcc();
//                    res_out_tmp = (vcc_tmp * pwm_cnt_cal) /(0.02 * PWM_DUTY);  
//                    IapWrite_R_out(res_out_tmp);
//                    
//                    set_status = SET_STATUS_IDLE;
//                    break;
//                }
//                case KEY_CMD_SEN_UP:
//                {
//                    vcc_tmp = Get_Vcc();
//                    res_out_tmp = (vcc_tmp * pwm_cnt_cal) /(0.02 * PWM_DUTY);  
//                    IapWrite_R_out(res_out_tmp);
//                    
//                    vi_tmp = Get_Vi();
//                    res_in_tmp = vi_tmp/0.02;
//                    IapWrite_R_in(res_in_tmp);
//                    
//                    set_status = SET_STATUS_IDLE;
//                    break;
//                }                     
//                default:break;                
//            }         
//            
//        }
//        
//        default:break;
//    }













/**********************************************************
�������ƣ�SET0_100
����   �������ֶ��궨0%~100%
�����������õĺ����嵥��
���ñ������ĺ����嵥��
���������  ��
���������  ��
����ֵ��    ��
***********************************************************/
//void KeyScan()
//{
//    u8 i=0;
//    u8 j=0;
//    switch(KeyLMDScan())
//    {
//        case 0:break;
//        case 1:
//            {
//                u16 *psen;
//                psen = SENtable;
//                LCD_ClearDisplay();
//                SEN_DIS();
//                switch(SEN)                           //ʹָ��psenָ��ʵʱ������
//                {
//                    case SEN0_01MA:break;
//                    case SEN0_1MA:psen = psen+1;break;
//                    case SEN0_2MA:psen = psen+2;break;
//                    case SEN0_4MA:psen = psen+3;break; 
//                    case SEN0_6MA:psen = psen+4;break;    
//                    case SEN0_8MA:psen = psen+5;break;
//                    default:break;
//                }
//                
//                while(KeyLMDScan() != 1)
//                {
//                    WDT_CONTR = RESET;
//                    if(KeySET0Scan())
//                    {
//                        if(psen == SENtable)
//                        {
//                            psen = psen+5;
//                            SEN = *psen;
//                        }
//                        else
//                        {
//                            psen--;
//                            SEN = *psen;
//                        }
//                      
//                        SEN_DIS();
//                        IapWriteSEN(); 
//                        delay_ms(800);
//                    }
//                    
//                    if(KeySET100Scan())
//                    {
//                        if((psen-5) == SENtable)
//                        {
//                            psen = psen-5;
//                            SEN = *psen;
//                        }
//                        else
//                        {
//                            psen++;
//                            SEN = *psen;
//                        }
//                   
//                        SEN_DIS();
//                        IapWriteSEN(); 
//                        delay_ms(800);
//                    }
//                       
//                    
//                }
//                break;
//                
//                

//              }                     
//         case 2:
//             {
//                 LCD_FullDisplay();
//                 PWMCount = ((16 * ActualOpening + 4)/1000) * 200 * PWM_DUTY * VREF25_adc / (2.5 * 1024);
//                 PWM_OUT_Cal();
//                 while((KeyLMDScan()) == 0)
//                 {
//                     WDT_CONTR = RESET;
//                     if(KeySET0Scan())
//                     {
//                        PWMCount--;
//                        PWM_OUT_Cal();
//                        VREF25_adc = GetVx_adc(VREF25_channel);
//                        R2 = (PWMCount * 2.5 *1024)/(0.02 * PWM_DUTY * VREF25_adc);
//                        IapWriteR2();
//                   

//                     }

//                     if(KeySET100Scan())
//                     {
//                         PWMCount++;
//                         PWM_OUT_Cal();
//                         VREF25_adc = GetVx_adc(VREF25_channel);
//                         R2 = (PWMCount * 2.5 *1024)/(0.02 * PWM_DUTY * VREF25_adc);
//                         IapWriteR2();
// 
//                     }

//                 }
//                 if(LMDEXIT == LONGPRESSEXIT)
//                 {  
//                    Vi_adc = GetVx_adc(Vinput_channel);
//                    R1 = (2.5*Vi_adc)/(0.02*VREF25_adc);
//                    IapWriteR1();
//                 }                      
//                 break;                  
//                  
//         }
//     }

//        if(KeySET100Scan())
//       {

//            SET100_DIS();
//            Opening100_adc = GetVx_adc(Vactual_channel);
//            IapWriteOpening100_adc();
//            delay_ms(800);
//           

//        }

//     if(KeySET0Scan())
//        {
//            SET0_DIS();
//            Opening0_adc = GetVx_adc(Vactual_channel);
//            IapWriteOpening0_adc();
//            delay_ms(800);

//        }
//                   
//       



//}
//u8 KeyLMDScan()     //0����û����1����̰���2������
//{
//    if(LMD == 0)
//    {
//        delay_ms(10);
//        if(LMD == 0)
//        {
//            delay_ms(500);
//            if(LMD == 1)
//            {
//                LMDEXIT = SHORTPRESSEXIT;    
//                return 1;
//            }
//            else
//            {
//                LMDEXIT = LONGPRESSEXIT;
//                while(!LMD);
//                return 2;
//            }                       
//        }
//        else
//        {
//            return 0;
//        }      
//    }
//    else
//    {
//        return 0;
//    }

//}

//bit KeySET0Scan()
//{
//    if(SET0 == 0)
//    {
//        delay_ms(10);
//        if(SET0 == 0)
//        {
//            while(!SET0);
//            return 1;       
//        }
//        else
//        {
//            return 0;
//        }      
//    }
//    else
//    {
//        return 0;
//    }

//}

//bit KeySET100Scan()
//{
//    if(SET100 == 0)
//    {
//        delay_ms(10);
//        if(SET100 == 0)
//        {
//            while(!SET100);
//            return 1;       
//        }
//        else
//        {
//            return 0;
//        }      
//    }
//    else
//    {
//        return 0;
//    }

//}


