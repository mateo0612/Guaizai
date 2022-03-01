#include <PID_v1.h>

double setpoint = 180.1;// 181.13 179.60; 178.5;179.7
double uprightPIDInput, uprightPIDOutput;
PID uprightPid(&uprightPIDInput, &uprightPIDOutput, &setpoint,  90, 30, 1.5, DIRECT);// 70, 135, 4.8

double speedInput, speedOutput, speedsFilter , speedErrorIntegral;

double turnInput, turnOutput;

double maxSpeedErrorIntegral = 800;
double Kp = 20, Ki = 0.4, Si=0.5;  //100, 0.1, 0.5
//const double Kp = 2, Ki = Kp / 200, Si=5;
//const double maxSpeedErrorIntegral = 10000;

double setp0 = 0;
long getSpeed();
void resetPulseCount();


double Tpe = 10, Tpm = 0, Ti=0.3;

void initPID()
{
  //setup PID
  uprightPid.SetMode(AUTOMATIC);
  uprightPid.SetSampleTime(5);// 10 - OK, 5 - GOOD, 1- CHANGE PID
  uprightPid.SetOutputLimits(-255, 255);
}


double uprightPidCompute()
{
  uprightPid.Compute();
  return -uprightPIDOutput;
}


extern int throttle, throttle1;
extern int steering, steering1;

double speedPidCompute()
{
  speedInput = (double)getSpeed() - 0;
  
  resetPulseCount();
  
  speedsFilter *= 0.7;
  speedsFilter += speedInput * 0.3;
  speedErrorIntegral += speedsFilter;
  speedErrorIntegral += (throttle + throttle1)*Si;
  
  speedErrorIntegral = normalize(speedErrorIntegral, maxSpeedErrorIntegral);

  speedOutput = (setp0 - speedsFilter) * Kp + (setp0 - speedErrorIntegral) * Ki;

  speedOutput = normalize(speedOutput, 255);
  return speedOutput;
}

extern volatile long rightWheelPulseCount;
extern volatile long leftWheelPulseCount;

double turnPidCompute()
{
  double turnPositionEncoder = (double)(leftWheelPulseCount - rightWheelPulseCount);
  turnOutput = Tpe * turnPositionEncoder +  Tpm * turnInput + Ti*(steering+steering1);
  turnOutput = -turnOutput;
  return turnOutput;
}


double normalize(double input, double maxValue)
{
   if (input > maxValue)
    input = maxValue;
  else if (input < -maxValue)
    input = -maxValue;
  return input;
}
