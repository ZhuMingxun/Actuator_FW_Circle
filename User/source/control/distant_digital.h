#ifndef _DISTANT_DIGITAL_H
#define _DISTANT_DIGITAL_H

typedef enum
{
    DISTANT_CMD_STOP =0,
    DISTANT_CMD_OPEN   ,
    DISTANT_CMD_CLOSE  
        
}DistantCmd_Type;    

#define LOCK_DELAY          30//65ms*30 ~ 2s

void Distant_PCA_Config();
void DistantDigital_Mode();
void DistantDigital_Control();
void DistantDigital_Control2();
extern u8 distant_cmd;
//extern u8 distant_trigger_flag;
//extern u8 lock_flag;
//extern u8 lock_cnt;
extern volatile DistantCmd_Type distant_cmd;
#endif