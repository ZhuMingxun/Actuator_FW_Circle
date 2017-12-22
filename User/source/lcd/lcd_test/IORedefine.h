/**************************************************************************/
/* This I/O Redefine header file is suitable for the PCB of Version2.0  
/* date:2015.2.4
/* author:ZhuMX

/**************************************************************************/
#ifndef _IORedefine_H_
#define _IORedefine_H_

/*****LCD��****/
#define     HT1621B_CS          P00   //HT1621BƬѡ��
#define     HT1621B_WR          P01   //HT1621Bд���ݶ�
#define     HT1621B_DATA        P02 //HT1621B���ݶ�
#define     LCD_RED             P03  //LCD���
#define     LCD_GREEN           P04 //LCD�̵�
#define     LCD_YELLOW          P34 //LCD�Ƶ�
#define     IR_RCV              P25 //������� CCP0_3

/*���ذ尴��*/
#define     LED_LSG             P07  //Lose signal LED#define     DEF_BOTTOM          P54 //0%�궨
#define     DEF_TOP             P40 //100%�궨
#define     SET_SEN             P55 // ����/������

/*������*/
#define     LOSE_OPEN           P45 // ���ſ�
#define     LOSE_CLOSE          P46 // ���Ź�
#define     POS_NEG             P23 //����������
#define     ALARM               P24 //���10s����

/*�ֳ��ź�*/
#define     LOCAL               P20 // �ֳ��ź�
#define     LOCAL_STOP          P21 // �ֳ�ֹͣ
#define     LOCAL_CLOSE         P37 // �ֳ����ź�
#define     LOCAL_OPEN          P36 // �ֳ����ź�
/*Զ�������ź�*/
#define     DISTANT_KEEP        P22   //Զ������
#define     DISTANT_OPEN        P26  //CCP1_3 Զ����
#define     DISTANT_CLOSE       P27  //CCP2_3 Զ����

/*�г�������λ�ź�*/                               
#define     UP_MOM              P44 // ������λ       
#define     UP_LIM              P43 // �г���λ 
#define     DOWN_LIM            P42 // �г���λ 
#define     DOWN_MOM            P41 // ������λ 


/*�����Ⲷ���ź�*/
#define     U_CAP               P32 // INT0
#define     V_CAP               P33 // INT1

/*�������ת����*/
#define     M_OPEN              P05  // �����ת
#define     M_CLOSE             P06  // �����ת

/*�������*/
#define     ERR                 P47

                               //P10,Input Current  Vi   ADC0    
                               //P11,Actual Opening Va   ADC1
                               //P12,Vref_2.5V      Vref ADC2
                               //P30��P31ΪTXD,RXD
                               //P35 ΪPWM

#endif

//#ifndef _IORedefine_H_
//#define _IORedefine_H_


//#define     LCD_GREEN      P00 //LCD�̵�
//#define     LCD_RED        P01 //LCD���
//#define     HT1621B_DATA   P02 //HT1621B���ݶ�
//#define     HT1621B_WR     P03 //HT1621Bд���ݶ�
//#define     HT1621B_CS     P04 //HT1621BƬѡ��
//#define     OPEN           P05  // �̵�����
//#define     CLOSE          P06  // �̵�����
//#define     LSG_LED        P07  //����

//                               //P10,P11,P12ΪADC��������
//#define     LOSE_O         P13 // ���ſ�
//#define     LOSE_C         P14 // ���Ź�
//#define     ZZY            P15 //������
//#define     SET0           P45 //0%�궨
//#define     SET100         P24 //100%�궨

//#define     LMD            P47 // ������
//#define     ALARM          P23
//#define     LOC            P20 // �ֳ��ź�
//#define     STOP           P21 // �ֳ������ź�
//#define     CLE            P37 // �ֳ����ź�
//#define     OPN            P36 // �ֳ����ź�
//#define     ERR            P22

//                                //P30��P31ΪTXD,RXD
//                                //P35 ΪPWM
//#define     shangB         P44 // ������λ ��INT3
//#define     shangA         P43 // �г���λ ��INT2
//#define     xiaA           P42 // �г���λ ��INT1
//#define     xiaB           P41 // ������λ ��INT0

//#endif


