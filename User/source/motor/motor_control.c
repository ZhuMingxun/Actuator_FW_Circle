#include "motor_control.h"
#include "motor_protect.h"
#include "motor.h"

#define __PIN_RELAY_A
#define __PIN_RELAY_B
#define __RELAY_ON            0
#define __RELAY_OFF           1
#define __M_OPEN              {__PIN_RELAY_A = (__RELAY_ON ^ phase_seq ^ POS_REV);__PIN_RELAY_B = !RELAY_A;}
#define __M_CLOSE             {__PIN_RELAY_B = (__RELAY_ON ^ phase_seq ^ POS_REV);__PIN_RELAY_A = !RELAY_B;}
#define __M_STOP              {__PIN_RELAY_A = __RELAY_OFF;__PIN_RELAY_B = __RELAY_OFF;}

#define __OUTPUTCMD_OPEN    1
#define __OUTPUTCMD_CLOSE   2
#define __OUTPUTCMD_STOP    0

#define __DRIVECMD_OPEN     1
#define __DRIVECMD_CLOSE    2
#define __DRIVECMD_STOP     0
#define __DRIVECMD_KEEP     3


//==================================
//获取输出命令函数
//===================================
static u8 GetOutputCmd(u8 incmd)
{
    u8 inputcmd = incmd;
    u8 outputcmd;
    
    switch (inputcmd)
    {
        case INPUTCMD_OPEN:
            {
                if( PROTECT_SR&0xF8 )
                    {
                        outputcmd = __OUTPUTCMD_STOP;
                    }
                    else 
                    {
                        outputcmd = __OUTPUTCMD_OPEN;
                    }
                    break;
            }
        case INPUTCMD_CLOSE:
            {
                if( PROTECT_SR&0x8F )
                    {
                        outputcmd = __OUTPUTCMD_STOP;
                    }
                    else 
                    {
                        outputcmd = __OUTPUTCMD_CLOSE;
                    }
                    break; 
            }
        case INPUTCMD_STOP:
            {
                outputcmd = __OUTPUTCMD_STOP;   
                break; 
            }
        default:break;
    } 
    
    return  outputcmd;
}

//==================================
//获取驱动命令函数
//===================================
static u8 GetDriveCmd(u8 outcmd)
{
    u8 drivecmd;
    u8 outputcmd = outcmd;
    
    switch(motor_status)
    {
        case MOTOR_STATUS_OPEN:
            {
                if(outputcmd==__OUTPUTCMD_OPEN)   drivecmd =  __DRIVECMD_KEEP;
                else  drivecmd = __DRIVECMD_STOP;
                break;
            }
        case MOTOR_STATUS_CLOSE:
            {
                if(outputcmd==__OUTPUTCMD_CLOSE)   drivecmd =  __DRIVECMD_KEEP;
                else  drivecmd = __DRIVECMD_STOP;
                break;   
            }    
        case MOTOR_STATUS_STOP:
            {
                if(outputcmd==__OUTPUTCMD_OPEN)    drivecmd  =  __DRIVECMD_OPEN; break;
                if(outputcmd==__OUTPUTCMD_CLOSE)   drivecmd  =  __DRIVECMD_CLOSE;break;
                if(outputcmd==__OUTPUTCMD_STOP)    drivecmd  =  __DRIVECMD_KEEP; break;  
                
            }
        default:break; 
    }
    return  drivecmd;        
}

//==================================
//电机驱动函数
//===================================          
static void DriveMotor(u8 drvcmd)     
{
    u8 cmd = drvcmd;
    
    switch(cmd)
    {
        case __DRIVECMD_OPEN:
            {
                __M_OPEN
                motor_status = MOTOR_STATUS_OPEN;
                break;
            }
        case __DRIVECMD_CLOSE:
            {
                
                __M_CLOSE
                motor_status = MOTOR_STATUS_CLOSE;
                break;
            }
        case __DRIVECMD_STOP:
            {
                __M_STOP
                motor_status = MOTOR_STATUS_STOP;
                SetStopDelayFlag();
                break;
            }
        default:break;
    } 
          
}

void MotorControl(u8 inputcmd)
{
    u8 incmd = inputcmd;
    u8 outcmd;
    u8 drivecmd;
    
    outcmd = GetOutputCmd(incmd);
    drivecmd = GetDriveCmd(outcmd);
    DriveMotor(drivecmd);
    
}                           
            
            
                        
                
    
    
    
    



