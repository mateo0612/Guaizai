#include "Wire.h"

extern bool dmpReady;
extern double uprightPIDInput;
extern double speedInput, speedsFilter;
extern double speedErrorIntegral;
extern int throttle, throttle1;
extern int steering, steering1;
extern double turnInput;

extern volatile long rightWheelPulseCount;
extern volatile long leftWheelPulseCount;

bool DMPDataNotReady();
double uprightPidCompute();
double turnPidCompute();
void setMotor(int pwmValL, int pwmValR);
void MPUUpdate();
void updateBluetoothValue();
void displayOLED(String str);
double normalize(double input, double maxValue);
void calculateDistance(float* distance);
void displayLCD(int cursorW, int cursorH, int textColor, int fontSize, String str);

void initMPU();
void initPID();
void initEncoderMotor();
void initBluetooth();
void initOLED();
void initRadar();
void initLCD();
#define STD_LOOP_TIME 1
unsigned long loopStartTime = 0;

void setup()
{
  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin();
  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)

  // initialize serial communication
  // (115200 chosen because it is required for Teapot Demo output, but it's
  // really up to you depending on your project)
  Serial.begin(115200);
  while (!Serial); // wait for Leonardo enumeration, others continue immediately
  Serial.println("Init MPU!!!");
  initMPU();
  Serial.println("Init PID!!!");
  initPID();
  Serial.println("Init Motors and Encoders!!!");
  initEncoderMotor();

  Serial.println("Init initBluetooth!!!");
  initBluetooth();

  Serial.println("Init OLED!!!");
  initLCD();

  Serial.println("Init Radar!!!");
  initRadar();
}

long loopCount = 0;
double speedPWM = 0;
double uprightPWM = 0;
double turnPWM = 0;
double pwmValL = 0, pwmValR = 0;
unsigned long dur = 0;

void testSpeedPIDloop()
{ //test speed pid
  loopCount++;
  double speeds = sin(loopCount * 0.01) * 255;
  speedPWM = speedPidCompute();
  double pwmVal = speedPWM;
  setMotor(pwmVal);

  Serial.print("speeds\t");
  Serial.print(speeds);
  Serial.print("\tspeedPWM\t");
  Serial.println(speedPWM);
}

void loop1()
{
  testMinAbsSpeed();
}
int errorCount = 0;
float distance[2];
void loop()
{
  unsigned long start = millis();
  if (!dmpReady)
    return;

  updateBluetoothValue();
  loopCount++;
  MPUUpdate();

  if ( uprightPIDInput > 200 || uprightPIDInput < 160)
  {
    errorCount ++;
    if (errorCount > 5)
      return;
  } else
  {
    errorCount = 0;
  }

  uprightPWM = uprightPidCompute();

  if (loopCount % 4 == 0) //20 ms for turn pid
  {
    turnPWM = turnPidCompute();
  }

  if (loopCount % 8 == 0) //40 ms for speed pid
  {
    speedPWM = speedPidCompute();
    //speedPWM = 0;
    //displayOLED(String(uprightPIDInput));
  }
  pwmValL = uprightPWM + speedPWM + turnPWM;
  pwmValL = normalize(pwmValL, 255);
  pwmValR = uprightPWM + speedPWM - turnPWM;
  pwmValR = normalize(pwmValR, 255);

  if (loopCount > 200 && loopCount % 80 == 0) //0.4s and AFTER 1s for distance calculation
  {
    calculateDistance(distance);
    if (distance[0] < 15 && speedPWM > -150)
    {
      speedPWM = -150;
    }
    if (distance[1] < 15 && speedPWM < 200)
    {
      speedPWM = 200;
    }
  }
  //displayLCD(10, 10, 0x0000, 2, String(uprightPIDInput));
  setMotor(pwmValL, pwmValR);
  timekeeper();
  
  //displayInfo();
  dur = millis() - start;
  //Serial.print("\tdur\t");
 
}

void displayInfo()
{
  Serial.print("upIn\t");
  Serial.print(uprightPIDInput);
  Serial.print("\tupPWM\t");
  Serial.print(uprightPWM);
  //Serial.print("\tspIn\t");
  //Serial.print(speedInput);
  //Serial.print("\tspPWM\t");
  //Serial.print(speedPWM);

  Serial.print("\tpwmValL\t");
  Serial.print(pwmValL);
  Serial.print("\tpwmValR\t");
  Serial.print(pwmValR);

  Serial.print("\tthrottle1\t");
  Serial.print(throttle1);
  Serial.print("\tsteering1\t");
  Serial.print(steering1);

  //Serial.print("\tdistance[0]\t");
  //Serial.print(distance[0]);
  //Serial.print("\tdistance[1]\t");
  //Serial.print(distance[1]);

  //Serial.print("\tR\t");
  //Serial.print(rightWheelPulseCount);
  //Serial.print("\tL\t");
  //Serial.print(leftWheelPulseCount);
  Serial.print("\tdu\t");
  Serial.println(dur);

  //displayOLED(String(uprightPIDInput));
}
void timekeeper() {

  // Calculate time since loop began
  float timeChange = millis() - loopStartTime;

  // If the required loop time has not been reached, please wait!
  if (timeChange < STD_LOOP_TIME) {
    int delayT = STD_LOOP_TIME - timeChange;
    delay(delayT);
  }
  Serial.println(timeChange);
  // Update loop timer variables
  loopStartTime = millis();
}
