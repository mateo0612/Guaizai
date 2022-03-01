#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

#define MPU_YPR_SELECT 1
#define MPU_INT_PIN 0
#define USE_INT 0
MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady()
{
  mpuInterrupt = true;
}


extern double uprightPIDInput;
extern double turnInput;
void initMPU()
{
  mpu.initialize();

  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  //-1888  3196  1523  139 60  -7
  //3682  -4734 1288  124 21  -22
  //3658  -4735 1314  -4  25  -132
  //3637  -4734 1278  19  26  -105
  //3637  -4719 1258  65  16  -54
  //-1509  781 837 253 -28 7 mega2
  //-1491  768 840 249 -23 7

  mpu.setXAccelOffset(-1491);
  mpu.setYAccelOffset(768);
  mpu.setZAccelOffset(840);
  mpu.setXGyroOffset(249);
  mpu.setYGyroOffset(-23);
  mpu.setZGyroOffset(7);
  if (devStatus == 0)
  {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    if(USE_INT)
    {
      attachInterrupt(MPU_INT_PIN, dmpDataReady, RISING);
    }
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
  else
  {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }
}


bool DMPDataReady()
{
  if(USE_INT)
  {
    return mpuInterrupt || fifoCount >= packetSize;
  }else
  {
    return fifoCount >= packetSize;
  }
}

void MPUUpdate()
{
  fifoCount = mpu.getFIFOCount();
  if (!DMPDataReady())
  {
    return;
  }
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  if ((mpuIntStatus & 0x10) || fifoCount == 1024)
  {
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));
  }
  else if (mpuIntStatus & 0x02)
  {
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    fifoCount -= packetSize;
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    uprightPIDInput = ypr[MPU_YPR_SELECT] * 180 / M_PI + 180;
    //mpu.dmpGetAccel(&aa, fifoBuffer);
    //mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    //turnInput = aaReal.y;
  }
}
