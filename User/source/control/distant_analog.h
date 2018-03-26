#ifndef _DISTANT_ANALOG_H
#define _DISTANT_ANALOG_H

#define K_OFFSET 0.8
/* adaptive position control struct */
/*
    pos_target = pos_input - offset_open_filter(offset_close_filter)
    offset_open = pos_end - pos_target
    offset_close = pos_end - pos_target
    offset_open_filter = offset_open_filter+K_OFFSET*(offset_open-offset_open_filter)
    offset_open_filter = offset_close_filter+K_OFFSET*(offset_close-offset_close_filter)
*/
typedef struct{
    
    float pos_input;
    float pos_target;//0-100
    float pos_end;//0-100
    float offset_open;
    float offset_close;
    float offset_open_filter;
    float offset_close_filter;
    u8 pos_end_flag;
    
}PosControl_TypeDef;
extern PosControl_TypeDef* pPosCTR;

void DistantAnalog_Mode();
void DistantAnalog_Control();
void PosControl_Init();
void GetEndPos();
void Set_InputLowHigh_Detect();
#endif