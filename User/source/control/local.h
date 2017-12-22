#ifndef _LOCAL_H
#define _LOCAL_H

#define LOCAL_ON            0
#define LOCAL_STOP_ON       0

//#define SEN0_05MA   50
//#define SEN0_1MA    100
//#define SEN0_15MA   150
//#define SEN0_2MA    200
//#define SEN0_25MA   250
//#define SEN0_3MA    300
//#define SEN0_35MA   350
//#define SEN0_4MA    400
//#define SEN0_45MA   450
//#define SEN0_5MA    500
#define SEN0_05MA   160
#define SEN0_1MA    320
#define SEN0_15MA   480
#define SEN0_2MA    640
#define SEN0_25MA   800
#define SEN0_3MA    960
#define SEN0_35MA   1120
#define SEN0_4MA    1280
#define SEN0_45MA   1440
#define SEN0_5MA    1600


typedef enum
{
    LOCAL_CMD_STOP =0,     
    LOCAL_CMD_OPEN = 1 ,    
    LOCAL_CMD_CLOSE =2       
}LocalCmd_Type;

void Local_Mode();
void Local_Control();
void DefZero_Mode();
void DefHundred_Mode();
void SetSen_Mode();
void CalOut_Mode();
void CalIn_Mode();
void Exti23_Config();
void Exti23_Disable();

extern volatile LocalCmd_Type local_cmd;

#endif