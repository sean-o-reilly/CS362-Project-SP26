#include "ProgCar.h"

using namespace pgc;

void setup() {
  initSlave(A1_ADDRESS);
}

void runDriveCommand(Gas gas, Speed speed) {
  // drive logic
  reportToMaster(WORK_DONE);
}

void loop() {
  handleDriveCommand(runDriveCommand);
}
