#ifndef __BUTTERWORTH_FILTER_H
#define __BUTTERWORTH_FILTER_H
#include "config.h"
#include <math.h>
float Butterworth_Filter_Valve(float nowdata);
float Butterworth_Filter_Refer(float nowdata);
float Butterworth_Filter_Input(float nowdata);
float Butterworth_Filter_Encoder(float nowdata);
#endif