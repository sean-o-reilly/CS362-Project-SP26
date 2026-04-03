#include "ProgCar.h"

using namespace pgc;

void setup() {
  initSlave(A2_ADDRESS);
}

void runSteerCommand(SteerDir steerDir) {
  // steer logic
  reportToMaster(WORK_DONE);
}

void loop() {
  handleSteerCommand(runSteerCommand);
}
