#include <LMotorController.h>

//MOTOR CONTROLLER
int ENA = 5;
int IN1 = 6;
int IN2 = 7;
int IN3 = 8;
int IN4 = 9;
int ENB = 10;
#define MIN_ABS_SPEED 150
#define MAX_ABS_SPEED 255
LMotorController motorController(ENA, IN1, IN2, ENB, IN3, IN4, 1, 1);

#define ENC_INT_RIGHT_A 4  //19-->4
#define ENC_INT_LEFT_A 5   //18-->5

#define ENC_IN_RIGHT_B 11
#define ENC_IN_LEFT_B 12

// Keep track of the number of right wheel pulses
volatile long rightWheelPulseCount = 0;
volatile long leftWheelPulseCount = 0;

void initEncoderMotor()
{
  // Set pin states of the encoder
  //pinMode(ENC_IN_RIGHT_A , INPUT_PULLUP);
  //pinMode(ENC_IN_LEFT_A , INPUT_PULLUP);
  pinMode(ENC_IN_RIGHT_B , INPUT);
  pinMode(ENC_IN_LEFT_B , INPUT);

  // Every time the pin goes high, this is a pulse
  attachInterrupt(ENC_INT_RIGHT_A, rightWheelPulse, RISING);
  attachInterrupt(ENC_INT_LEFT_A, leftWheelPulse, RISING);
}

void setMotor(int pwmVal)
{
  if (pwmVal == 0)
  {
    motorController.stopMoving();
  }
  //pwmVal = getNormPWM(pwmVal);
  motorController.move(pwmVal, MIN_ABS_SPEED);
}

void setMotor(int pwmValL, int pwmValR)
{
  //pwmValL = getNormPWM(pwmValL);
  //pwmValR = getNormPWM(pwmValR);
  motorController.move(pwmValL, pwmValR, MIN_ABS_SPEED);
}

// Increment the number of pulses by 1
void rightWheelPulse() {
  int b = digitalRead(ENC_IN_RIGHT_B);
  if (b > 0)
  {
    rightWheelPulseCount--;
  } else
  {
    rightWheelPulseCount++;
  }
}

// Increment the number of pulses by 1
void leftWheelPulse() {
  int b = digitalRead(ENC_IN_LEFT_B);
  if (b > 0)
  {
    leftWheelPulseCount++;
  } else
  {
    leftWheelPulseCount--;
  }
}

double getNormPWM(double pwm)
{
  if (pwm < 0) pwm = -pwm;
  double normPWM = (pwm / 255) * (MAX_ABS_SPEED - MIN_ABS_SPEED) + MIN_ABS_SPEED;
  return pwm < 0 ? -normPWM : normPWM;
}
long getSpeed()
{
  return (rightWheelPulseCount + leftWheelPulseCount) / 2;
}

void resetPulseCount()
{
  rightWheelPulseCount = 0;
  leftWheelPulseCount = 0;
}

void testMinAbsSpeed()
{
  for (int i = 0; i < 255; i += 10)
  {
    setMotor(i, -i);
    displayOLED(String(i));
    delay(5000);
  }
}
