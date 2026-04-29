#include <Servo.h>
#include "ProgCar.h"

using namespace pgc;

Servo servo;

constexpr int SERVO_PIN = 9;

// Servo timing variables
unsigned long currTime = 0, prevTime = 0;
constexpr unsigned long timeInterval = 75;

// LED timing variables
unsigned long prevLedTime = 0;
constexpr unsigned long ledInterval = 100;

void setup() {
  initSlave(A2_ADDRESS);
  pinMode(LED_BUILTIN, OUTPUT);
  // attach() sends signal to servo motor to center itself at 90 degrees
  // hence the fast spin in the beginning
  servo.attach(SERVO_PIN);
  Serial.begin(BAUD_RATE);
}

/*
Rotates servo's current position to the finalAngle. Allows for leftward and rightward rotation.
*/
void rotateServo(const int finalAngle) {
  int posOffset = (servo.read() < finalAngle) ? 1 : -1;

  while (servo.read() != finalAngle) {
    currTime = millis();
    if ((currTime - prevTime) >= timeInterval) {
      servo.write(servo.read() + posOffset);
      prevTime = currTime;
    }
  }
}

void runSteerCommand(SteerDir steerDir) {
  int angle = static_cast<int>(steerDir);
  Serial.println(angle);
  rotateServo(angle);
  reportToMaster(WORK_DONE);
}

void loop() {
  // Run command handler constantly without blocking
  handleSteerCommand(runSteerCommand);

  // Toggle LED non-blockingly every 100ms
  unsigned long currentMillis = millis();
  if (currentMillis - prevLedTime >= ledInterval) {
    prevLedTime = currentMillis; 
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}
