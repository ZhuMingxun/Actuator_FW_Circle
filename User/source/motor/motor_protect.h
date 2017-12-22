#ifndef __MOTOR_PROTECT_H
#define __MOTOR_PROTECT_H

#include "config.h"
#include "motor.h"

void MotorErrFlag_Update (float curopening);
void TorqueFlag_Update();
void LimitFlag_Update();
void FullyFlag_Update(float curopening);
void SetStopDelayFlag();
void StopDelayFlag_Update();
extern u8 PROTECT_SR;



#endif