#define soundTriggerPin  33
#define soundEchoInt1    0
#define soundEchoInt2    1
#define soundSpeed       343.0

volatile unsigned long travelTime[2];  // Place to store traveltime of the pusle
volatile unsigned long startTime[2];   // Place to store ping times (interrupt)

void initRadar()
{
  pinMode(soundTriggerPin, OUTPUT);   // Set common triggerpin as output
  pinMode(2, INPUT);    // Set interrupt pin 2 (INT0) as INPUT (sensor 1)
  pinMode(3, INPUT);    // Set interrupt pin 3 (INT1) as INPUT (sensor 2)
  attachInterrupt(0, call_INT0, CHANGE );   // ISR for INT0
  attachInterrupt(1, call_INT1, CHANGE );   // ISR for INT1
}

void call_INT0()
{
  byte pinRead;
  pinRead = digitalRead(2);      // Slower ; Read pin 2
  // pinRead = PIND >> 2 & B00000001;  // Faster ; Read pin 2/PD2
  interruptHandler(pinRead, 0);
}

void call_INT1()
{
  byte pinRead;
  pinRead = digitalRead(3);      // Slower ; Read pin 3
  // pinRead = PIND >> 3 & B00000001;  // Faster ; Read pin 3/PD3
  interruptHandler(pinRead, 1);
}
void interruptHandler(bool pinState, int nIRQ)
{
  unsigned long currentTime = micros();  // Get current time (in µs)
  if (pinState)
  {
    // If pin state has changed to HIGH -> remember start time (in µs)
    startTime[nIRQ] = currentTime;
  }
  else
  {
    // If pin state has changed to LOW -> calculate time passed (in µs)
    travelTime[nIRQ] = currentTime - startTime[nIRQ];
  }
}

void calculateDistance(float* distance)
{
  noInterrupts();   // cli()
  for (int i = 0; i < 2; i++)
  {
    distance[i] = travelTime[i] / 2.0 * (float)soundSpeed / 10000.0;   // in cm
  }
  interrupts();   // sei();
  digitalWrite(soundTriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(soundTriggerPin, LOW);
}
