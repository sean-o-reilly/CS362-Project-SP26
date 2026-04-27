#include <Stepper.h>
#include "ProgCar.h"

using namespace pgc;

const int STEPS_PER_REV = 2048;
Stepper leftMotor(STEPS_PER_REV, 2, 4, 3, 5);
Stepper rightMotor(STEPS_PER_REV, 6, 7, 8, 9);

const int TRIG_PIN = 10;
const int ECHO_PIN = 11;
const int BUZZER_PIN = 12;
const int OBSTACLE_THRESHOLD = 10;

unsigned long lastBlink = 0;
const int blinkInterval = 100;

void setup() {
  initSlave(A1_ADDRESS);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(BAUD_RATE);
}

bool isPathBlocked() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  const float soundSpeed = 0.034;
  int distance = duration * soundSpeed / 2.0;
  Serial.print(distance);
  Serial.print(" ");
  Serial.println(duration);

  if (distance >= 0 && distance < OBSTACLE_THRESHOLD) {
    digitalWrite(BUZZER_PIN, HIGH);
    return true;
  }
  digitalWrite(BUZZER_PIN, LOW);
  return false;
}

void runDriveCommand(Gas gas, Speed speed) {
  int motorSpeed = 0;
  int travelSteps = 1000;

  if (speed == Speed::LOW_SPEED) {
    motorSpeed = 5;
  } else if (speed == Speed::MEDIUM_SPEED) {
    motorSpeed = 10;
  } else if (speed == Speed::HIGH_SPEED) {
    motorSpeed = 15;
  }

  leftMotor.setSpeed(motorSpeed);
  rightMotor.setSpeed(motorSpeed);

  int leftDirection, rightDirection;
  if (gas == Gas::FORWARD) {
    leftDirection = 1;
    rightDirection = -1;
  } else {
    leftDirection = -1;
    rightDirection = 1;
  }

  const int STEPS = 10;
  for (int i = 0; i < travelSteps; i += STEPS) {
    if (isPathBlocked()) {
      Serial.println("Obstacle detected.");
      unsigned long startWait = millis();
      while (millis() - startWait < 500) {}
      digitalWrite(BUZZER_PIN, LOW);
      reportToMaster(OBSTACLE_ERROR);
      return;
    }
    leftMotor.step(STEPS * leftDirection);
    rightMotor.step(STEPS * rightDirection);
  }

  reportToMaster(WORK_DONE);
}

void loop() {
  handleDriveCommand(runDriveCommand);

  unsigned long currentMillis = millis();
  if (currentMillis - lastBlink >= blinkInterval) {
    lastBlink = currentMillis;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}
