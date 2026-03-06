#include "ProgCar.h"

using namespace pgc;

void setup() {
  initSlave(A3_ADDRESS);
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