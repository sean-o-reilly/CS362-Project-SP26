#include "ProgCar.h"

using namespace pgc;

void setup() {
  initSlave(A3_ADDRESS);
}

void getMove() {
  static int moves = 0;

  for (int i = 0; i < 5; ++i)  // do work (get one move from user)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(250);
  }

  if (moves < MAX_MOVES) {
    reportMove(Move{ .gas = Gas::FORWARD, .steer = SteerDir::STRAIGHT, .speed = Speed::LOW_SPEED });
    ++moves;
  } else {
    reportDone();
    moves = 0;
  }
}

void loop() {
  runSlaveCommand(getMove);
}