/********************************************************************************/
/* This I/O Redefine header file is just suitable for the PCB of Version 2.0 !  
/* date:2015.2.4
/* author:ZhuMX

/********************************************************************************/
#ifndef _IORedefine_H_
#define _IORedefine_H_

/*LCD��*/
#define     HT1621B_CS          P00 //HT1621BƬѡ��
#define     HT1621B_WR          P01 //HT1621Bд���ݶ�
#define     HT1621B_DATA        P02 //HT1621B���ݶ�
#define     LCD_RED             P03 //LCD��� 
#define     LCD_GREEN           P04 //LCD�̵� 
#define     LCD_YELLOW          P34 //LCD�Ƶ�
#define     IR_RCV              P25 //������� CCP0_3

/*���ذ尴��*/
#define     LED_LSG             P07 //Lose signal LED#define     OPENING_0_DEF       P54 //0%�궨
#define     OPENING_100_DEF     P40 //100%�궨
#define     SET_SEN             P55 // ����/������

/*������*/
#define     PIN_LOSE_OPEN           P45 // ���ſ�
#define     PIN_LOSE_CLOSE          P46 // ���Ź�
#define     POS_REV             	P23 //����������
#define     PIN_ALARM               P24 //���10s����
#define     ALARM_ON            0
#define     ALARM_OFF           1
/*�ֳ��ź�*/
#define     LOCAL               P20 // �ֳ��ź�
#define     LOCAL_STOP          P21 // �ֳ�ֹͣ
#define     LOCAL_CLOSE         P37 // �ֳ����ź�
#define     LOCAL_OPEN          P36 // �ֳ����ź�
/*Զ�������ź�*/
#define     PIN_DISTANT_STOP        P22   //Զ������
#define     PIN_DISTANT_OPEN        P26  //CCP1_3 Զ����
#define     PIN_DISTANT_CLOSE       P27  //CCP2_3 Զ����

/*�г�������λ�ź�*/                               
#define     PIN_TORQUE_OPEN     P44 // ������λ       
#define     PIN_LIM_OPN         P43 // �г���λ 
#define     PIN_LIM_CLE         P42 // �г���λ 
#define     PIN_TORQUE_CLOSE    P41 // ������λ 
#define     PIN_FLAG_LIMOPN     P16
#define     PIN_FLAG_LIMCLE     P17

#define     PIN_FLAG_LIMCLE_ON  {PIN_FLAG_LIMCLE = 0;PIN_FLAG_LIMOPN = !PIN_FLAG_LIMCLE;}
#define     PIN_FLAG_LIMOPN_ON  {PIN_FLAG_LIMOPN = 0;PIN_FLAG_LIMCLE = !PIN_FLAG_LIMOPN;}
#define     PIN_FLAG_LIMCLE_OFF {PIN_FLAG_LIMCLE = 1;}
#define     PIN_FLAG_LIMOPN_OFF {PIN_FLAG_LIMOPN = 1;}

/*�����Ⲷ���ź�*/
#define     U_CAP               P32 // INT0
#define     V_CAP               P33 // INT1

/*�������ת�����ź�*/
#define     RELAY_A              P05  // �̵���A
#define     RELAY_B              P06  // �̵���B

/*�����ź�*/
#define     ERR                 P47
#define     ERR_OUT             {if(ERR != 0) ERR = 0;}
#define     ERR_OFF             {if(ERR != 1) ERR = 1;}

#define     P_PWM              P35 //ΪPWM

                               //P10,Input Current  Vi   ADC0    
                               //P11,Actual Opening Va   ADC1
                               //P12,Vref_2.5V      Vref ADC2
                               //P30��P31ΪTXD,RXD


#endif

///**************************************************************************/
///* This I/O Redefine header file is just suitable for the PCB of Version 1.0 !  
///* date:
///* author:ZhuMX

///**************************************************************************/

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



