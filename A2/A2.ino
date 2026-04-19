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
  // attach() sends signal to servo motor to center itself at 90 degrees
  // hence the fast spin in the beginning
  servo.attach(SERVO_PIN);
  Serial.begin(BAUD_RATE);
}

/*
Rotates in the direction of 0 degrees -> 180 degrees.
*/
void rotateLeft(int finalAngle) {
  int pos = servo.read();
  while (pos <= finalAngle) {
    currTime = millis();
    if ((currTime - prevTime) > timeInterval) {
      servo.write(pos);
      prevTime = currTime;
      pos++;
    }
  }
}

/*
Rotates in the direction of 180 degrees -> 0 degrees.
*/
void rotateRight(int finalAngle) {
  int pos = servo.read();
  while (pos >= finalAngle) {
    currTime = millis();
    if ((currTime - prevTime) > timeInterval) {
      servo.write(pos);
      prevTime = currTime;
      pos--;
    }
  }
}

/*
Rotates left or right based on if finalAngle is greater/less than current position.
*/
void rotateServo(int finalAngle) {
  if (finalAngle > servo.read()) {
  	rotateLeft(finalAngle);
  } else {
  	rotateRight(finalAngle);
  }
}

void runSteerCommand(SteerDir steerDir) {
  int angle = static_cast<int>(steerDir);
  Serial.println(angle);
  rotateServo(angle);
  reportToMaster(WORK_DONE);
}

void loop() {
  handleSteerCommand(runSteerCommand);
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(100);
}
