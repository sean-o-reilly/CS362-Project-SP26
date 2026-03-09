#include <LiquidCrystal.h>

#include "ProgCar.h"

// Pushbutton setup
const int execute_button_pin = 13;
const int forward_button_pin = 6;
const int backward_button_pin = 7;
const int left_button_pin = 8;
const int right_button_pin = 9;
const int speed_button_pin = 10;

// LCD setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

using namespace pgc;

void setup() {
  initSlave(A3_ADDRESS);
  
  // Pushbutton setup
  pinMode(execute_button_pin, INPUT_PULLUP);
  pinMode(forward_button_pin, INPUT_PULLUP);
  pinMode(backward_button_pin, INPUT_PULLUP);
  pinMode(left_button_pin, INPUT_PULLUP);
  pinMode(right_button_pin, INPUT_PULLUP);
  pinMode(speed_button_pin, INPUT_PULLUP);

  // LCD setup
  lcd.begin(16, 2);
}

void getMove() {
  static int moves = 0;

  for (int i = 0; i < 5; ++i)  // Placeholder for doing work
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(250);
  }

  if (moves < MAX_MOVES) {
    reportToMaster(Move{ .gas = Gas::FORWARD, .steer = SteerDir::STRAIGHT, .speed = Speed::LOW_SPEED });
    ++moves;
  } else {
    reportToMaster(pgc::WORK_DONE);
    moves = 0;
  }
}

void loop() {
  runSlaveCommand(getMove);
}
