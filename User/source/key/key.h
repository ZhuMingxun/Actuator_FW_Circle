/**********************************************************文件名：Key.h
作者：朱明勋
说明：Key.c中的函数声明

**********************************************************/
#ifndef _KEY_H_
#define _KEY_H_
                                                          



#define YES 1
#define NO  0

#define     T1_MS     10    //10ms
#define     T1_CNT   (65536UL - ((T1_MS*MAIN_Fosc)/12/1000))

#define     KEY_SHORT_MS    10   //100ms
#define     KEY_LONG_MS     3000//3s
#define     KEY_CNT_SHORT   (KEY_SHORT_MS / T1_MS)
#define     KEY_CNT_LONG    (KEY_LONG_MS / T1_MS)



#define LOCAL_DEF_MS            5000 //5s后进入标定模式
#define LOCAL_DEF_CNT           (LOCAL_DEF_MS / T1_MS)

#define KEY_STATUS_IDLE     0
#define KEY_STATUS_DOWN     1
#define KEY_STATUS_ACK      2
#define KEY_STATUS_UP       3

#define KEY_NUM_NONE    7
#define KEY_NUM_100     6
#define KEY_NUM_0       5
#define KEY_NUM_SEN     3

#define KEY_CMD_NONE        0     //无按键按下
#define KEY_CMD_0_LONG      1     //0%长按
#define KEY_CMD_100_LONG    2     //100%长按
#define KEY_CMD_100_SHORT   3     //100%键短按
#define KEY_CMD_0_SHORT     4     //0%键短按
#define KEY_CMD_SEN_SHORT   5     //SET键短按
#define KEY_CMD_SEN_LONG    6     //SET键长按
#define KEY_CMD_0_UP        7     //0%松开
#define KEY_CMD_100_UP      8     // 100%松开
#define KEY_CMD_SEN_UP      9     // set松开

#define SET_STATUS_IDLE     0
#define SET_STATUS_DEF0     1
#define SET_STATUS_DEF100   2
#define SET_STATUS_SEN      3
#define SET_STATUS_CAL_LCK  4
#define SET_STATUS_CAL      5


#define     OPENING_0_DEF       P54 //0%标定

void KeyIO_Config();
void Timer1_Config();
//void KeyCmd_Detect();
//void Def0_Proces();
//void Def100_Proces();
//void Sen_Proces();
//void Cal_Lock_Wait();
//void Cal_Res();
void Alarm_Detect();
//void KnobDef_Check();
//void KnobDef_Proces();
//extern u8 key_cmd;
extern float opening_rt;
//extern const u16* p_sen;
//extern u16 sen_table[10];
#endif