void initBluetooth()
{
    Serial2.begin(9600);
    Serial3.begin(9600); 
}
int throttle=0, steering=0, nextThrottle, nextSteering, nextTag=0;

void updateThrottleSteering(int value)
{
  if (value == 251)
  {
    nextTag = 1;
  }else if (nextTag == 1)
  {
    nextThrottle = value-49;
    nextTag = 2;
  } else if (nextTag == 2)
  {
    nextSteering = 49-value;
    nextTag = 3;
  } else if (nextTag == 3)
  {
    nextTag = 0;
    if (value != 250)
    {
      Serial.println("Received wrong data");
    }else
    {
      throttle = nextThrottle;
      steering = nextSteering;
    }
  }
}

int throttle1=0, steering1=0, nextThrottle1, nextSteering1, nextTag1=0;

void updateThrottleSteering1(int value)
{
  if (value == 251)
  {
    nextTag1 = 1;
  }else if (nextTag1 == 1)
  {
    nextThrottle1 = value-50;
    nextTag1 = 2;
  } else if (nextTag1 == 2)
  {
    nextSteering1 = -(value-50);
    nextTag1 = 3;
  } else if (nextTag1 == 3)
  {
    nextTag1 = 0;
    if (value != 250)
    {
      Serial.println("Received wrong data");
    }else
    {
      throttle1 = nextThrottle1;
      steering1 = nextSteering1;
    }
  }
}

void updateBluetoothValue()
{
  if (Serial3.available() > 0)
  {
    int income = Serial3.read(); 
    updateThrottleSteering(income);
  }
  if (Serial2.available() > 0)
  {
    int income = Serial2.read();      //Read the incoming data and store it into variable Incoming_value
    updateThrottleSteering1(income);
  }
}
