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
    u8 cap_sequence;//�������������
    u8 cap_cnt;//�����Բ�������ݼ���������Ƿ񲶻�һ�����ڵ�����
    u8 capok_flag;//����һ�����ڵ����к���1��������º���0
    
}CapSeq_TypeDef;

void PhaseSeq_Detect_Config();
void PhaseSeq_Update();
void LackPhase_Mode();
extern bit phase_seq;

#endif