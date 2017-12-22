#ifndef _Motor_H_
#define _Motor_H_

#define     MotorFWDFlag        0
#define     MotorREVFlag        1
#define     MotorNormalStopFlag 2
#define     MotorUpStopFlag     3
#define     MotorDownStopFlag   4
#define     MotorAlarmStopFlag  5

#define		MOTOR_STATUS_FWD    0
#define		MOTOR_STATUS_REV    1
#define		MOTOR_STATUS_STOP   2
#define		MOTOR_STATUS_UP    	3
#define		MOTOR_STATUS_DOWN   4
#define		MOTOR_STATUS_ERR    5

#define     UpStop      0
#define     DownStop    1
#define     NormalStop  2
#define     AlarmStop   3

#define DistantMode  1
#define LocalMode    0
#define LoseMode     2

#define REVCTRFlag 0
#define FWDCTRFlag 1
#define CLEARFLAG  2

#define PROTECTSTATUS 1

#define RESET 0x34

void DistantControl();
void LocalControl();
void LoseSignalControl();
void Motor_Stop(u8 Stoptype);
void Motor_FWD();
void Motor_REV();
void ProtectCheck();


#endif