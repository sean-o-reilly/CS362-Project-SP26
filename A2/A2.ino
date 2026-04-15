#include <Servo.h>
#include "ProgCar.h"

using namespace pgc;

Servo servo;

constexpr int SERVO_PIN = 9;

void setup() {
  initSlave(A2_ADDRESS);
  pinMode(LED_BUILTIN, OUTPUT);
  servo.attach(SERVO_PIN);
  Serial.begin(BAUD_RATE);
}

void runSteerCommand(SteerDir steerDir) {
  int angle = static_cast<int>(steerDir);
  Serial.println(angle);
  servo.write(angle);
  while (servo.read() != angle);
  reportToMaster(WORK_DONE);
}

void loop() {
  handleSteerCommand(runSteerCommand);
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(100);
}
