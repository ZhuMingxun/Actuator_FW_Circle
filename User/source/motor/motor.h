
#ifndef _MOTOR_H_
#define _MOTOR_H_
#include "data_input.h"


typedef enum
{
    MOTOR_STATUS_OPEN  = 0,
    MOTOR_STATUS_CLOSE, 
    MOTOR_STATUS_STOP 
}MotorStatus_Type;


void Motor_Config();
void Motor_Stop();
void MotorOpen_Control();
void MotorClose_Control();
void MotorStop_Delay_60ms();
//u8 Get_ValvePosition();
//void Limit_Detect();
void Torque_Detect();
void CloseDir_Protect();
void TravelProtect();
void TravelProtect_Machine();
void MotorErr_Detect();
u8 LimitPosition_Output();
void IntegAlarm();

void MotorVel_Detect();
void TravelProtect_Def();
void CloseDir_Protect();
void Motor_PosRev();

extern volatile MotorStatus_Type motor_status;
extern  u8 bdata Motor_CR;
extern  bit torquelock_open_flag;//
extern  bit torquelock_close_flag;//
extern  bit open100_flag;
extern  bit close0_flag;
extern  bit motorerr_flag;

#endif