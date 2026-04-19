#include <Servo.h>
#include "ProgCar.h"

using namespace pgc;

Servo servo;

constexpr int SERVO_PIN = 9;

// variables for delay between each call to write()
// for slower spinning, set timeInterval to a higher value
unsigned long currTime, prevTime = 0;
unsigned long timeInterval = 15;

void setup() {
  initSlave(A2_ADDRESS);
  pinMode(LED_BUILTIN, OUTPUT);
  servo.attach(SERVO_PIN);
  Serial.begin(BAUD_RATE);
}

/*
Spins servo motor from its current position to the desired angle at a custom speed.
In order to reduce jitter and inconsistent spinning, there is custom delay logic
between each call to write().
*/
void spinServo(int finalAngle) {
  for (int pos = servo.read(); pos < finalAngle; pos++) {
    currTime = millis();
    if ((currTime - prevTime) > timeInterval) {
      servo.write(pos);
      prevTime = currTime;
    }
  }
}

void runSteerCommand(SteerDir steerDir) {
  int angle = static_cast<int>(steerDir);
  Serial.println(angle);
  spinServo(angle);
  reportToMaster(WORK_DONE);
}

void loop() {
  handleSteerCommand(runSteerCommand);
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(100);
}
