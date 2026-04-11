#include "ProgCar.h"
#include <Servo.h> 

using namespace pgc;

//  Hardware Setup
Servo steeringServo;
const int SERVO_PIN = 3; 
// Steering Angles
const int ANGLE_LEFT = 45;
const int ANGLE_STRAIGHT = 90;
const int ANGLE_RIGHT = 135;

//  Execution Function
void executeSteering() {
  SteerDir currentDir = SteerDir::STRAIGHT; 

  // Read the payload sent by the Master
  if (Wire.available()) {
    currentDir = static_cast<SteerDir>(Wire.read());
  }

  // Command the servo (this triggers the hardware timer, no blocking required)
  switch (currentDir) {
    case SteerDir::LEFT:
      steeringServo.write(ANGLE_LEFT);
      Serial.println("Action: Commanded LEFT");
      break;
    case SteerDir::RIGHT:
      steeringServo.write(ANGLE_RIGHT);
      Serial.println("Action: Commanded RIGHT");
      break;
    case SteerDir::STRAIGHT:
    default:
      steeringServo.write(ANGLE_STRAIGHT);
      Serial.println("Action: Commanded STRAIGHT");
      break;
  }

  // Immediately report success. 
  // The servo will physically travel to its position in the background
  // while the Master is freed up to immediately trigger the Drive Arduino (A1).
  reportToMaster(WORK_DONE);
}

void setup() {
  Serial.begin(BAUD_RATE);
  
  // Initialize this Arduino as Slave A2
  initSlave(A2_ADDRESS);

  // Attach the servo and set it to straight by default
  steeringServo.attach(SERVO_PIN);
  steeringServo.write(ANGLE_STRAIGHT);
  
  Serial.println("A2 Steering Arduino Initialized (Non-Blocking).");
}

void loop() {
  // Checks for commands and runs executeSteering instantly when triggered
  runSlaveCommand(executeSteering);
}
