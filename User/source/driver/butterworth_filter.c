#include "butterworth_filter.h"
#include "data_input.h"


/* 
 * 定义巴特沃斯滤波器系数
 * n=1,wn=0.05 
*************************/
//#define __B1 (0.07295966)
//#define __B2 (0.07295966)
//#define __A1 (1.0)
//#define __A2 (-0.8540807)

/* 
 * 定义巴特沃斯滤波器系数
 * n=1,wn=0.2 
*************************/
#define __B1 (0.2452)
#define __B2 (0.2452)
#define __A1 (1.0)
#define __A2 (-0.5095)

/* 
 * 定义巴特沃斯滤波器系数
 * n=1,wn=0.5 
*************************/
//#define __B1 (0.5)
//#define __B2 (0.5)
//#define __A1 (1.0)
//#define __A2 (0)
//static float b1 = 0.5;
//static float b2 = 0.5;
//static float a2 = 0;

//static float b1 = 0.07295966;
//static float b2 = 0.07295966;
//static float a2 = -0.8540807;

void SelectSortf(float*buf, u8 arrsize)
{
    u8 minindex;
    float temp;
    u8 i;
    u8 j;
    for(i=0;i<arrsize;i++)
    {
        minindex = i;
        for(j=i+1;j<arrsize;j++)
        {
            if(buf[j]<buf[minindex])
            {
                minindex = j;
            }
        }
        if(minindex != i)
        {
            temp = buf[i];
            buf[i] = buf[minindex];
            buf[minindex] = temp;
        }
    }
}

void BubbleSort(u16* buf,u8 n)
{
    u16 temp;
    u8 i,j;
    for(i = 0;i<n-1;++i)
    {
        for(j=0;j<n-i-1;++j)
        {
            if(buf[j]>buf[j+1])
            {
                temp = buf[j];
                buf[j] = buf[j+1];
                buf[j+1] = temp;
            }
        }
    }
}

u16 Average_Process()
{
    static u16 slidebuf[10]={0};
    u16 sortbuf[10];
    //static u8 slidebuf_cnt=0;
    u8 i;
    u16 sum=0;
    u16 tmp;

//    if(slidebuf_cnt>=10)
//        slidebuf_cnt=0;

		for(i=0;i<9;i++)
    		slidebuf[i] = Get_Vx_adc(VA_CH);
    
    for(i=0;i<10;i++)
    {
        sortbuf[i]=slidebuf[i];
    }
    
    BubbleSort(sortbuf,sizeof(sortbuf)/sizeof(u16));
    
    for(i=3;i<8;i++)
        sum += sortbuf[i];
    
    tmp = sum/5.0f;
    return tmp;
    
}

//u16 RemoveJumpData_Valve(u16 dat)
//{
//    u16 curdata;
//    static u16 predata;
//    static u8 jumpcnt = 0;
//    static u8 first_flag = 1;
//    
//    curdata = dat;
//    if(first_flag == 1)
//    {
//        predata = curdata;
//        jumpcnt = 0;
//        first_flag = 0;
//        return curdata;//第一次取当前数据并更新前一个数据
//    }
//    else
//    {
//        if(abs(curdata-predata)>=5 )
//        {
//            jumpcnt++;
//            if(jumpcnt<3) 
//            {
//                return predata;//偶然跳变，取前一个数据
//            }
//            else 
//            {
//                jumpcnt = 0;
//                predata = curdata;
//                return curdata;//长时间跳变，取当前数据并更新前一个数据
//            }
//        }
//        else
//        {
//            jumpcnt = 0;
//            predata = curdata; 
//            return curdata;//不跳变，取当前数据并更新前一个数据
//        }
//    }  
//}
u16 RemoveJumpData_Encoder(u16 dat)
{
    u16 curdata;
    static u16 predata;
    static u8 jumpcnt = 0;
    static u8 first_flag = 1;
    
    curdata = dat;
    if(first_flag == 1)
    {
        predata = curdata;
        jumpcnt = 0;
        first_flag = 0;
        return curdata;//第一次取当前数据并更新前一个数据
    }
    else
    {
        if(abs(curdata-predata)>=5 || curdata == 0xC000)
        {
            jumpcnt++;
            if(jumpcnt<3) 
            {
                return predata;//偶然跳变，取前一个数据
            }
            else 
            {
                jumpcnt = 0;
                predata = curdata;
                return curdata;//长时间跳变，取当前数据并更新前一个数据
            }
        }
        else
        {
            jumpcnt = 0;
            predata = curdata; 
            return curdata;//不跳变，取当前数据并更新前一个数据
        }
    }  
}

//u16 RemoveJumpData_Refer(u16 dat)
//{
//    u16 curdata;
//    static u16 predata;
//    static u8 jumpcnt = 0;
//    static u8 first_flag = 1;
//    
//    curdata = dat;
//    if(first_flag == 1)
//    {
//        predata = curdata;
//        jumpcnt = 0;
//        first_flag = 0;
//        return curdata;//第一次取当前数据并更新前一个数据
//    }
//    else
//    {
//        if(abs(curdata-predata)>=2 )
//        {
//            jumpcnt++;
//            if(jumpcnt<6) 
//            {
//                return predata;//偶然跳变，取前一个数据
//            }
//            else 
//            {
//                jumpcnt = 0;
//                predata = curdata;
//                return curdata;//长时间跳变，取当前数据并更新前一个数据
//            }
//        }
//        else
//        {
//            jumpcnt = 0;
//            predata = curdata; 
//            return curdata;//不跳变，取当前数据并更新前一个数据
//        }
//    }  
//}

u16 RemoveJumpData_Input(u16 dat)
{
    u16 curdata;
    static u16 predata;
    static u8 jumpcnt = 0;
    static u8 first_flag = 1;
    
    curdata = dat;
    if(first_flag == 1)
    {
        predata = curdata;
        jumpcnt = 0;
        first_flag = 0;
        return curdata;//第一次取当前数据并更新前一个数据
    }
    else
    {
        if(abs(curdata-predata)>=5 )
        {
            jumpcnt++;
            if(jumpcnt<3) 
            {
                return predata;//偶然跳变，取前一个数据
            }
            else 
            {
                jumpcnt = 0;
                predata = curdata;
                return curdata;//长时间跳变，取当前数据并更新前一个数据
            }
        }
        else
        {
            jumpcnt = 0;
            predata = curdata; 
            return curdata;//不跳变，取当前数据并更新前一个数据
        }
    }  
}


float Butterworth_Filter_Valve(float nowdata)
{
    static float indata[2];
    static float outdata;
    static u8 first_flag = 1;
    u16 temp;
    
    //temp = RemoveJumpData_Valve((u16)nowdata);//去毛刺
	temp = Average_Process();
    nowdata = (float)temp;

    if(first_flag == 1)
    {
        indata[1] = nowdata;
        indata[0] = nowdata;
        outdata = nowdata;
        first_flag = 0;
        return outdata;        
    }
    indata[1] = nowdata;
    outdata = __B1*indata[1] + __B2*indata[0] - __A2*outdata;
    indata[0] = indata[1];
    return   outdata; 

    
}
float Butterworth_Filter_Encoder(float nowdata)
{
    static float indata[2];
    static float outdata;
    static u8 first_flag = 1;
    u16 temp;
    
    temp = RemoveJumpData_Encoder((u16)nowdata);//去毛刺
    nowdata = (float)temp*1.0f;

	//return nowdata;
    if(first_flag == 1)
    {
        indata[1] = nowdata;
        indata[0] = nowdata;
        outdata = nowdata;
        first_flag = 0;
        return outdata;        
    }
    indata[1] = nowdata;
    outdata = __B1*indata[1] + __B2*indata[0] - __A2*outdata;
    indata[0] = indata[1];
    return   outdata; 

    
}
float Butterworth_Filter_Input(float nowdata)
{
    static float indata[2];
    static float outdata;
    static u8 first_flag = 1;
    u16 temp;
    
    temp = RemoveJumpData_Input((u16)nowdata);//去毛刺
    nowdata = (float)temp;
    
    if(first_flag == 1)
    {
        indata[1] = nowdata;
        indata[0] = nowdata;
        outdata = nowdata;
        first_flag = 0;
        return outdata;        
    }
    indata[1] = nowdata;
    outdata = __B1*indata[1] + __B2*indata[0] - __A2*outdata;
    indata[0] = indata[1];
    return   outdata; 
 
    
}


//float Butterworth_Filter_Refer(float nowdata)
//{
//    static float indata[2];
//    static float outdata;
//    static u8 first_flag = 1;
//    u16 temp;
//    
//    temp = RemoveJumpData_Refer((u16)nowdata);//去毛刺
//    nowdata = (float)temp;
//    
//    if(first_flag == 1)
//    {
//        indata[1] = nowdata;
//        indata[0] = nowdata;
//        outdata = nowdata;
//        first_flag = 0;
//        return outdata;        
//    }
//    indata[1] = nowdata;
//    outdata = __B1*indata[1] + __B2*indata[0] - __A2*outdata;
//    indata[0] = indata[1];
//    return   outdata; 
//    
//    
//}