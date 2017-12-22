#ifndef _PHASE_DETECT_H_
#define _PHASE_DETECT_H_

#define     PHASE_STATUS_POSSEQ     1  // 
#define     PHASE_STATUS_REVSEQ     0  // 
#define     PHASE_STATUS_LACK       2  // 

typedef enum
{
    PHASESEQ_POSITIVE = 0,
    PHASESEQ_REVERSE  = 1,
    PHASESEQ_LACK     = 2
}PhaseSeq_Type;

typedef struct
{
    u8 cap_sequence;//捕获的相序序列
    u8 cap_cnt;//用来对捕获的数据计数，检测是否捕获到一个周期的序列
    u8 capok_flag;//捕获到一个周期的序列后置1，相序更新后置0
    
}CapSeq_TypeDef;

void PhaseSeq_Detect_Config();
void PhaseSeq_Update();
void LackPhase_Mode();
extern bit phase_seq;

#endif