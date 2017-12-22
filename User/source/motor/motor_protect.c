

#include "motor_protect.h"


u8 PROTECT_SR = 0x00;//阀门保护标志寄存器
/*宏定义保护标志位*/
#define __BIT_MOTOR_ERROR_FLAG  7   //电机错误标志
#define __BIT_TORQUE_OPEN_FLAG  6   //开力矩标志
#define __BIT_FULLY_OPEN_FLAG   5   //开到位标志
#define __BIT_LIMIT_OPEN_FLAG   4   //开行程标志
#define __BIT_STOP_DELAY_FLAG   3   //电机启停延时标志
#define __BIT_TORQUE_CLOSE_FLAG 2   //关力矩标志
#define __BIT_FULLY_CLOSE_FLAG  1   //关到位标志
#define __BIT_LIMIT_CLOSE_FLAG  0   //关行程标志

/*宏定义保护标志MASK*/
#define __MOTOR_ERROR_MASK  ((u8)(1<<__BIT_MOTOR_ERROR_FLAG))
#define __TORQUE_OPEN_MASK  ((u8)(1<<__BIT_TORQUE_OPEN_FLAG))
#define __FULLY_OPEN_MASK   ((u8)(1<<__BIT_FULLY_OPEN_FLAG))
#define __LIMIT_OPEN_MASK   ((u8)(1<<__BIT_LIMIT_OPEN_FLAG))
#define __STOP_DELAY_MASK   ((u8)(1<<__BIT_STOP_DELAY_FLAG))
#define __TORQUE_CLOSE_MASK ((u8)(1<<__BIT_TORQUE_CLOSE_FLAG))
#define __FULLY_CLOSE_MASK  ((u8)(1<<__BIT_FULLY_CLOSE_FLAG))
#define __LIMIT_CLOSE_MASK  ((u8)(1<<__BIT_LIMIT_CLOSE_FLAG))

/*宏定义标志位操作*/
#define __MOTOR_ERROR_FLAG_SET     {PROTECT_SR |= __MOTOR_ERROR_MASK;}
#define __MOTOR_ERROR_FLAG_CLEAR   {PROTECT_SR &= ~__MOTOR_ERROR_MASK;}

#define __TORQUE_OPEN_FLAG_SET     {PROTECT_SR |= __TORQUE_OPEN_MASK;}
#define __TORQUE_OPEN_FLAG_CLEAR   {PROTECT_SR &= ~__TORQUE_OPEN_MASK;}                                 

#define __FULLY_OPEN_FLAG_SET      {PROTECT_SR |= __FULLY_OPEN_MASK;}
#define __FULLY_OPEN_FLAG_CLEAR    {PROTECT_SR &= ~__FULLY_OPEN_MASK;}                               

#define __LIMIT_OPEN_FLAG_SET      {PROTECT_SR |= __LIMIT_OPEN_MASK;}
#define __LIMIT_OPEN_FLAG_CLEAR    {PROTECT_SR &= ~__LIMIT_OPEN_MASK;}

#define __STOP_DELAY_FLAG_SET      {PROTECT_SR |= __STOP_DELAY_MASK;}
#define __STOP_DELAY_FLAG_CLEAR    {PROTECT_SR &= ~__STOP_DELAY_MASK;}

#define __TORQUE_CLOSE_FLAG_SET    {PROTECT_SR |= __TORQUE_CLOSE_MASK;}
#define __TORQUE_CLOSE_FLAG_CLEAR  {PROTECT_SR &= ~__TORQUE_CLOSE_MASK;}                               

#define __FULLY_CLOSE_FLAG_SET     {PROTECT_SR |= __FULLY_CLOSE_MASK;}
#define __FULLY_CLOSE_FLAG_CLEAR   {PROTECT_SR &= ~__FULLY_CLOSE_MASK;}                                

#define __LIMIT_CLOSE_FLAG_SET     {PROTECT_SR |= __LIMIT_CLOSE_MASK;}
#define __LIMIT_CLOSE_FLAG_CLEAR   {PROTECT_SR &= ~__LIMIT_CLOSE_MASK;}                                \

                                        
u8 GetFlagOfPROTECT_SR(u8 flag);

static u8 GetFlagOfPROTECT_SR(u8 flag)
{
    u8 mask = (u8)(1<<flag);
    if(PROTECT_SR & mask) return 1;
        else return 0;
  
}                                       
                                        

#define __MOTORERR_TIME_MAX    5//秒
#define __MOTORERR_DETECT_FREQ 10//Hz
#define __MOTORERR_CNT_MAX     (__MOTORERR_TIME_MAX*__MOTORERR_DETECT_FREQ)
//============================================
//阀位卡死标志更新函数
//执行频率：10Hz
//============================================
void MotorErrFlag_Update (float curopening)
{
    static u8 opening_pre;
    static u8 cnt;
    u8 opening_cur = (u8)(curopening*100.0+0.5);
    
    switch(motor_status)
        {
            case MOTOR_STATUS_OPEN:case MOTOR_STATUS_CLOSE:
                {
                    if(opening_cur == opening_pre)      cnt++;
                    else    
                        {
                            cnt = 0;
                            __MOTOR_ERROR_FLAG_CLEAR
                        }
                            
                    if(cnt>=__MOTORERR_CNT_MAX)    
                        {
                             __MOTOR_ERROR_FLAG_SET 
                             cnt = __MOTORERR_CNT_MAX;
                        }
                    
                    opening_pre = opening_cur;
                    break;
                }
            default:
                {
                    cnt = 0;
                    __MOTOR_ERROR_FLAG_CLEAR//opening_pre = opening_cur;
                    break;
                }
        }                     
	
}


#define __PIN_ON    1
#define __PIN_OFF   0
#define __PIN_TORQUE_OPEN     P44
#define __PIN_TORQUE_CLOSE    P41
#define __TORQUE_LOCK_TIME_SEC      2//秒
#define __TORQUE_DETECT_FREQ_Hz    10//Hz
#define __TORQUE_CNT_MAX            (__TORQUE_LOCK_TIME_SEC*__TORQUE_DETECT_FREQ_Hz)     
//============================================
//力矩标志更新函数
//执行频率：10Hz
//============================================
void TorqueFlag_Update()
{
    static u8  torqueopen_cnt;
    static u8  torqueclose_cnt;
    
    /*开力矩检测*/
    if(__PIN_TORQUE_OPEN==__PIN_ON)
        {
            torqueopen_cnt = 0;
            __TORQUE_OPEN_FLAG_SET
        }
    if(GetFlagOfPROTECT_SR(__BIT_TORQUE_OPEN_FLAG)) 
        {
            torqueopen_cnt++;
            if(torqueopen_cnt>=__TORQUE_CNT_MAX) 
                {
                    __TORQUE_OPEN_FLAG_CLEAR
                    torqueopen_cnt = __TORQUE_CNT_MAX;
                }
        }
        
    /*关力矩检测*/  
    if(__PIN_TORQUE_CLOSE==__PIN_ON)
        {
            torqueclose_cnt = 0;
            __TORQUE_CLOSE_FLAG_SET
        }
    if(GetFlagOfPROTECT_SR(__BIT_TORQUE_CLOSE_FLAG)) 
        {
            torqueclose_cnt++;
            if(torqueclose_cnt>=__TORQUE_CNT_MAX) 
                {
                    __TORQUE_CLOSE_FLAG_CLEAR
                    torqueclose_cnt = __TORQUE_CNT_MAX;
                }
        } 
 
}

#define __PIN_LIMIT_OPEN    P43
#define __PIN_LIMIT_CLOSE   P42
//============================================
//行程标志更新函数
//执行频率：10Hz
//============================================
void LimitFlag_Update()
{
    if(__PIN_LIMIT_OPEN==__PIN_ON)  
        {
            __LIMIT_OPEN_FLAG_SET
        }
        else __LIMIT_OPEN_FLAG_CLEAR
    
    if(__PIN_LIMIT_CLOSE==__PIN_ON)
        {
            __LIMIT_CLOSE_FLAG_SET
        }
        else __LIMIT_CLOSE_FLAG_CLEAR
    
}

//============================================
//到位标志更新函数
//执行频率：10Hz
//============================================
void FullyFlag_Update(float curopening)
{
    float opening = curopening;
    if(opening <= 0.0)
        {
            __FULLY_CLOSE_FLAG_SET
        }
        else if(opening >= 1.0)
                {
                    __FULLY_OPEN_FLAG_SET
                }
                else 
                    {
                        __FULLY_CLOSE_FLAG_CLEAR
                        __FULLY_OPEN_FLAG_CLEAR
                    }   
    
}


#define __STOPDELAY_TIME_SEC    2//秒
#define __STOPDELAY_DETECT_FREQ 10//Hz
#define __STOPDELAT_CNT_MAX     (__STOPDELAY_TIME_SEC*__STOPDELAY_DETECT_FREQ)
//============================================
//电机启停延时标志更新函数
//执行频率：10Hz
//============================================
void StopDelayFlag_Update()
{
    static u8 stopdelay_cnt;
    
    if(GetFlagOfPROTECT_SR(__BIT_STOP_DELAY_FLAG)==1)//电机停止后会立即置位
        {
            stopdelay_cnt++;
            if(stopdelay_cnt>=__STOPDELAT_CNT_MAX)  
                {
                    __STOP_DELAY_FLAG_CLEAR
                    stopdelay_cnt = 0;
                }
                      
        }
        else 
        {
            //__STOP_DELAY_FLAG(CLEAR)
            stopdelay_cnt = 0;
        }
     
}

void SetStopDelayFlag()
{
    __STOP_DELAY_FLAG_SET
}
        
        
















