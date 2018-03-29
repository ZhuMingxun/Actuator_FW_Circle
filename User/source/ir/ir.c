#include "ir.h"
#include "IORedefine.h"
#include "data_save.h"
#include "delay.h"
#include "control_new.h"
#include "distant_digital.h"
#include "local.h"
#include "ringbuffer.h"

u16 data ircmd = 0;

static u8 data ir_rx_buff[64];
static RingBuffer_TypeDef  IR_RingBuffer;



void IR_PCA_Config()
{
	PCA_InitTypeDef		PCA_InitStructure;

	PCA_InitStructure.PCA_IoUse    = PCA_P24_P25_P26_P27;	//PCA_P12_P11_P10_P37, PCA_P34_P35_P36_P37, PCA_P24_P25_P26_P27
	PCA_InitStructure.PCA_Clock    = PCA_Clock_12T;		//PCA_Clock_1T, PCA_Clock_2T, PCA_Clock_4T, PCA_Clock_6T, PCA_Clock_8T, PCA_Clock_12T, PCA_Clock_Timer0_OF, PCA_Clock_ECI
	PCA_InitStructure.PCA_Polity   = PolityHigh;		//优先级设置	PolityHigh,PolityLow
	PCA_InitStructure.PCA_Interrupt_Mode = DISABLE;		//
	PCA_Init(PCA_Counter,&PCA_InitStructure);

	PCA_InitStructure.PCA_Interrupt_Mode = PCA_Rise_Active | PCA_Fall_Active | ENABLE;		//上升沿下降沿中断
	PCA_InitStructure.PCA_Mode     = PCA_Mode_Capture;		//PCA_Mode_PWM, PCA_Mode_Capture, PCA_Mode_SoftTimer, PCA_Mode_HighPulseOutput
	PCA_InitStructure.PCA_Value    = 0;			//
	PCA_Init(PCA0,&PCA_InitStructure);

	CR = 1;
	EA = 1;    
    
}

void IR_Disable()
{
    PCA_InitTypeDef		PCA_InitStructure;
    PCA_InitStructure.PCA_Interrupt_Mode = PCA_Rise_Active | PCA_Fall_Active | DISABLE;
	PCA_InitStructure.PCA_Mode     = PCA_Mode_Capture;		//PCA_Mode_PWM, PCA_Mode_Capture, PCA_Mode_SoftTimer, PCA_Mode_HighPulseOutput
    PCA_InitStructure.PCA_Value    = 0;			//
	PCA_Init(PCA0,&PCA_InitStructure);
    
}

void IR_RingBuffer_Init()
{
    RingBuffer_Init(&IR_RingBuffer,ir_rx_buff,sizeof(ir_rx_buff));
    
}

u8 IRisReadyToRead()
{
    return !RingBufferIsEmpty(&IR_RingBuffer);
        
}



void IRCmd_LongPress_Count(u16 cmd)
{
    static u8 ircmd_100_cnt,ircmd_0_cnt,ircmd_4_cnt,ircmd_5_cnt,ircmd_6_cnt,ircmd_7_cnt,ircmd_8_cnt;
    switch(cmd)
    {
        case IR_COMMAND_100 :
        {
            ircmd_100_cnt++;
            if(ircmd_0_cnt !=0)     ircmd_0_cnt = 0;
            if(ircmd_4_cnt !=0)     ircmd_4_cnt = 0;
            if(ircmd_5_cnt !=0)     ircmd_5_cnt = 0;
            if(ircmd_6_cnt !=0)     ircmd_6_cnt = 0;
            if(ircmd_7_cnt !=0)     ircmd_7_cnt = 0;
            if(ircmd_8_cnt !=0)     ircmd_8_cnt = 0;
            if(ircmd_100_cnt>=36)   {ircmd = IR_COMMAND_100;ircmd_100_cnt = 0;}
            break;
        }
        case IR_COMMAND_0 :
        {
            ircmd_0_cnt++;
            if(ircmd_100_cnt !=0)   ircmd_100_cnt = 0;
            if(ircmd_4_cnt !=0)     ircmd_4_cnt = 0;
            if(ircmd_5_cnt !=0)     ircmd_5_cnt = 0;
            if(ircmd_6_cnt !=0)     ircmd_6_cnt = 0;
            if(ircmd_7_cnt !=0)     ircmd_7_cnt = 0;
            if(ircmd_8_cnt !=0)     ircmd_8_cnt = 0;
            if(ircmd_0_cnt>=36)   {ircmd = IR_COMMAND_0;ircmd_0_cnt = 0;}
            break;
        }
        case IR_COMMAND_4 :
        {
            ircmd_4_cnt++;
            if(ircmd_100_cnt !=0)   ircmd_100_cnt = 0;
            if(ircmd_0_cnt !=0)     ircmd_0_cnt = 0;
            if(ircmd_5_cnt !=0)     ircmd_5_cnt = 0;
            if(ircmd_6_cnt !=0)     ircmd_6_cnt = 0;
            if(ircmd_7_cnt !=0)     ircmd_7_cnt = 0;
            if(ircmd_8_cnt !=0)     ircmd_8_cnt = 0;
            if(ircmd_4_cnt>=4)   {ircmd = IR_COMMAND_4;ircmd_4_cnt = 0;}
            break;
        }
        case IR_COMMAND_OPEN:
        {
            ircmd_5_cnt++;
            if(ircmd_100_cnt !=0)   ircmd_100_cnt = 0;
            if(ircmd_0_cnt !=0)     ircmd_0_cnt = 0;
            if(ircmd_4_cnt !=0)     ircmd_4_cnt = 0;
            if(ircmd_6_cnt !=0)     ircmd_6_cnt = 0;
            if(ircmd_7_cnt !=0)     ircmd_7_cnt = 0;
            if(ircmd_8_cnt !=0)     ircmd_8_cnt = 0;
            if(ircmd_5_cnt >=4)    {ircmd = IR_COMMAND_OPEN;ircmd_5_cnt = 0;}
            break;
            
        }
        case IR_COMMAND_CLOSE:
        {
            ircmd_6_cnt++;
            if(ircmd_100_cnt !=0)   ircmd_100_cnt = 0;
            if(ircmd_0_cnt !=0)     ircmd_0_cnt = 0;
            if(ircmd_4_cnt !=0)     ircmd_4_cnt = 0;
            if(ircmd_5_cnt !=0)     ircmd_5_cnt = 0;
            if(ircmd_7_cnt !=0)     ircmd_7_cnt = 0;
            if(ircmd_8_cnt !=0)     ircmd_8_cnt = 0;
            if(ircmd_6_cnt >=4)    {ircmd = IR_COMMAND_CLOSE;ircmd_6_cnt = 0;} 
            break;
        }
        
        case IR_COMMAND_4_OPEN:
        {
            ircmd_7_cnt++;
            if(ircmd_100_cnt !=0)   ircmd_100_cnt = 0;
            if(ircmd_0_cnt !=0)     ircmd_0_cnt = 0;
            if(ircmd_4_cnt !=0)     ircmd_4_cnt = 0;
            if(ircmd_5_cnt !=0)     ircmd_5_cnt = 0;
            if(ircmd_8_cnt !=0)     ircmd_8_cnt = 0;
            if(ircmd_7_cnt >=64)    {ircmd = IR_COMMAND_4_OPEN;ircmd_7_cnt = 0;} 
            break;
        }
        case IR_COMMAND_4_CLOSE:
        {
            ircmd_8_cnt++;
            if(ircmd_100_cnt !=0)   ircmd_100_cnt = 0;
            if(ircmd_0_cnt !=0)     ircmd_0_cnt = 0;
            if(ircmd_4_cnt !=0)     ircmd_4_cnt = 0;
            if(ircmd_5_cnt !=0)     ircmd_5_cnt = 0;
            if(ircmd_7_cnt !=0)     ircmd_7_cnt = 0;
            if(ircmd_8_cnt >=64)    {ircmd = IR_COMMAND_4_CLOSE;ircmd_8_cnt = 0;} 
            break;
        } 
        default:
        {
            ircmd = cmd;
            if(ircmd_100_cnt !=0)   ircmd_100_cnt = 0;
            if(ircmd_0_cnt !=0)     ircmd_0_cnt = 0;
            if(ircmd_4_cnt !=0)     ircmd_4_cnt = 0;
            if(ircmd_5_cnt !=0)     ircmd_5_cnt = 0;
            if(ircmd_6_cnt !=0)     ircmd_6_cnt = 0;
            if(ircmd_7_cnt !=0)     ircmd_7_cnt = 0;
            if(ircmd_8_cnt !=0)     ircmd_8_cnt = 0;
            break;
        }
        
    }
  
}

void IR_Update()
{
    static u8 ircmd_bit_cnt = 0;
    static u16 ircmd_temp = 0;
    
    u8 cur;
    
    while(RingBufferIsEmpty(&IR_RingBuffer) !=1)
    {
        cur = RingBuffer_Pop(&IR_RingBuffer);
        if(cur !=1 && cur !=0)  break;
        else     cur &= 0x01;
 
        ircmd_temp <<= 1;
        ircmd_temp |= ((u16)cur & 0x0001);
        
        if(ircmd_bit_cnt == 0)
        {
            ircmd_temp &= 0x0007;
            if(ircmd_temp == 6) ircmd_bit_cnt = 3;
                
        }
        else
        {
            ircmd_temp &= 0x0FFF;
            ircmd_bit_cnt++;
            if(ircmd_bit_cnt == 12)
            {
                ircmd_bit_cnt = 0;
                IRCmd_LongPress_Count(ircmd_temp);
                
            }
        }

    }
   
}










//========================================================================
// 函数: void	PCA_Handler (void) interrupt PCA_VECTOR
// 描述: PCA中断处理程序.
// 参数: None
// 返回: none.
// 
//========================================================================
//extern u8 distant_open_trigger_flag;
//extern u8 distant_close_trigger_flag;
void	PCA_Handler (void) interrupt PCA_VECTOR
{
     
    static u16 ircnt_rise;
    static u16 ircnt_fall;
    u16 ircnt = 0;
    u8 temp = 0;
    
    if(CCF0)		//PCA模块0中断
	{  
        CCF0 = 0;		//清PCA模块0中断标志
		if(P25)
        {
            
            ircnt_rise = CCAP0H; 
            ircnt_rise = (ircnt_rise<<8) & 0xFF00;
            ircnt_rise |= CCAP0L;       
            ircnt = (ircnt_rise + 65536 - ircnt_fall) & (65535);
            if(ircnt>211 && ircnt<633)  temp = 0;
            else if(ircnt>633 && ircnt <1899)   temp = 1;
            else    temp = 0xff;//错误
            RingBuffer_Push(&IR_RingBuffer,temp);
           
        }            
		else
        {
            
            ircnt_fall = CCAP0H; 
            ircnt_fall = (ircnt_fall<<8) & 0xFF00;
            ircnt_fall |= CCAP0L;  

        }      

	}
    
    if(CCF1)
    {
        CCF1 = 0;
//        if( mode == MODE_DISTANT_DIGITAL && PIN_DISTANT_STOP==0)
//        {
//            delay_ms(2);
//            if(PIN_DISTANT_OPEN == 0 )//下降沿触发
//            {
//                distant_open_trigger_flag = 1;
//                distant_close_trigger_flag = 0;
//            }           
//        }
    }
    
    if(CCF2)
    {
        CCF2 = 0;
//        if(mode == MODE_DISTANT_DIGITAL && PIN_DISTANT_STOP==0)
//        {
//            delay_ms(2);
//            if(PIN_DISTANT_CLOSE == 0)//下降沿触发
//            {
//                distant_close_trigger_flag = 1;
//                distant_open_trigger_flag = 0;

//            }

//        }            
        
    }


	if(CF)	
	{
		CF = 0;	
         
	}


}
