#include <Stepper.h>
#include "ProgCar.h"

using namespace pgc;

// Stepper configuration
const int STEPS_PER_REV = 2048;
Stepper leftMotor(STEPS_PER_REV, 2, 4, 3, 5);  // IN1, IN3, IN2, IN4
Stepper rightMotor(STEPS_PER_REV, 6, 7, 8, 9);

// Sensor Pins
const int TRIG_PIN = 10;
const int ECHO_PIN = 11;
const int BUZZER_PIN = 12;
const int OBSTACLE_THRESHOLD = 10;

void setup() {
  initSlave(A1_ADDRESS);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(BAUD_RATE);
}

// check for obstacles
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
  int travelSteps = 1000;  // distance per move

  // Set speed based on the Speed enum
  if (speed == Speed::LOW_SPEED) {
    motorSpeed = 5;
  } else if (speed == Speed::MEDIUM_SPEED) {
    motorSpeed = 10;
  } else if (speed == Speed::HIGH_SPEED) {
    motorSpeed = 15;
  }

  leftMotor.setSpeed(motorSpeed);
  rightMotor.setSpeed(motorSpeed);
  // motor directions are opposites since they are positioned
  // with shafts facing outwards from eachother.
  int leftDirection, rightDirection;
  if (gas == Gas::FORWARD) {
    leftDirection = 1;
    rightDirection = -1;
  } else {
    leftDirection = -1;
    rightDirection = 1;
  }

  // check sensor frequently
  const int STEPS = 10;
  for (int i = 0; i < travelSteps; i += STEPS) {
    if (isPathBlocked()) {
      Serial.println("Obstacle detected.");
      delay(500);
      reportToMaster(OBSTACLE_ERROR);
      digitalWrite(BUZZER_PIN, LOW);
      return;
    }
    leftMotor.step(STEPS * leftDirection);
    rightMotor.step(STEPS * rightDirection)
  }

  reportToMaster(WORK_DONE);
}

void loop() {
  // check for commands
  handleDriveCommand(runDriveCommand);

  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(100);
}
