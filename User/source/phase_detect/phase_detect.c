
#include "Exti.h"
#include "IORedefine.h"
#include "phase_detect.h"
#include "control_new.h"
#include "mode_detect.h"
#include "LCD.h"
#include "uart_debug.h"
#include "delay.h"
#include "motor.h"
#include <stdio.h>
#include "actuator_config.h"

volatile PhaseSeq_Type phaseseq_status;//枚举变量，相序状态
volatile CapSeq_TypeDef Cap_Seq;//序列捕获结构体
bit phase_seq = 1;
u8 phase_update_ok=0;
/*************************INT0,INT1 配置********************************/

static void PhsaeSeqIO_Config()
{
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Mode = EXT_MODE_RiseFall;
    EXTI_InitStructure.EXTI_Polity = PolityHigh;
    EXTI_InitStructure.EXTI_Interrupt = ENABLE;
    Ext_Inilize(EXT_INT0,&EXTI_InitStructure);
    Ext_Inilize(EXT_INT1,&EXTI_InitStructure);
    EA = 1;
    
}

static void CapSeq_Init()
{
    Cap_Seq.cap_sequence = 0;
    Cap_Seq.cap_cnt = 0;
    Cap_Seq.capok_flag = 0;

}

/*************相序检测初始化配置**************/
void PhaseSeq_Detect_Config()
{
    PhsaeSeqIO_Config();
    CapSeq_Init(); 
    phase_update_ok = 0;
    while(phase_update_ok !=1)
    {
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif
        PhaseSeq_Update();
    }
}



/*************相序更新函数****************/
void PhaseSeq_Update()//主函数执行
{
    static u8 err_cnt = 0;
    static u8 pcnt;
    static u8 rcnt;
    u8 seq;
    
    if(Cap_Seq.capok_flag == 1)
    {
        seq = Cap_Seq.cap_sequence;
        switch(seq)
        {
            case 0x1E:case 0x87:case 0xE1:case 0x78:
            {
                phase_seq = 1;
                pcnt++;
                if(pcnt>5)
                {
                     phaseseq_status = PHASESEQ_POSITIVE;
                     phase_update_ok = 1;
                     pcnt=0;
                }
                rcnt=0; 
                err_cnt = 0;   
                break;
            }
            case 0x2D:case 0xD2:case 0x4B:case 0xB4:
            {
                phase_seq = 0;
                rcnt++;
                if(rcnt>5)
                {
                    phaseseq_status = PHASESEQ_REVERSE;
                    phase_update_ok = 1;
                    rcnt=0;
                }
                pcnt = 0;
                err_cnt = 0; 
                break;
            }
            default:
            {
                err_cnt++;
                if(err_cnt>=15)
                {
                    err_cnt = 0;
                    phaseseq_status = PHASESEQ_LACK;
                    mode = MODE_LACK_PHASE; 
                    phase_update_ok = 1;
                }
                pcnt=0;
                rcnt=0;
                break;
            }
        }
        
        EX0 = 1;
        EX1 = 1;
       // CapSeq_Init(&Cap_Seq);
    }
}



/****************缺相模式函数*********************/
void LackPhase_Mode()
{
    u8 ep_on = 1;
    LCD_YELLOW_ON
    ERR_OUT
    Motor_Stop();
    

    
    while(mode == MODE_LACK_PHASE)
    {
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif
        
        if(timer2_200ms_flag == 1)
        {
            timer2_200ms_flag = 0;
            PhaseSeq_Update();
            if(phaseseq_status != PHASESEQ_LACK)    
                break;
            
        }
        
        if(timer2_500ms_flag == 1)
        {
            timer2_500ms_flag = 0;
            
            if(ep_on)
            {
                Clear_Alarm_DIS();
                ep_on = 0;
            }
            else
            {
                EP_Alarm_DIS();
                ep_on = 1;
            } 
            
        }   
        
    }
    
    SystemMode_Detect();
  
}

/************相序捕获函数*****************/
/* description:负责填充捕获的序列    */
void PhaseSeq_Cap_A(u8 capvalue)//放在外部中断里
{
    Cap_Seq.cap_sequence <<= 2;
    Cap_Seq.cap_sequence |= (capvalue & 0x03);
    Cap_Seq.cap_cnt++;
    if(Cap_Seq.cap_cnt >= 6)//采集满一个周期的序列，标志置1，关闭中断不再采集
    {
        Cap_Seq.cap_cnt = 0;
        Cap_Seq.capok_flag = 1;
        EX0 = 0;
        EX1 = 0;
    }   
}
void PhaseSeq_Cap_B(u8 capvalue)//放在外部中断里
{
    Cap_Seq.cap_sequence <<= 2;
    Cap_Seq.cap_sequence |= (capvalue & 0x03);
    Cap_Seq.cap_cnt++;
    if(Cap_Seq.cap_cnt >= 6)//采集满一个周期的序列，标志置1，关闭中断不再采集
    {
        Cap_Seq.cap_cnt = 0;
        Cap_Seq.capok_flag = 1;
        EX0 = 0;
        EX1 = 0;
    }   
}

/********************* INT0 *************************/
void Ext_INT0 (void) interrupt INT0_VECTOR		
{
    u8  value_cap;
    EA = 0;
    value_cap = (P3 & 0x0c)>>2;
    PhaseSeq_Cap_A(value_cap);
    EA = 1;

}

/********************* INT1*************************/
void Ext_INT1 (void) interrupt INT1_VECTOR
{
    u8  value_cap;
    EA = 0;
    value_cap = (P3 & 0x0c)>>2;
    PhaseSeq_Cap_B(value_cap);
    EA = 1;
    
}

    
