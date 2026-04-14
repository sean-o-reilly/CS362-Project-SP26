#include "ProgCar.h"

using namespace pgc;

void setup() {
  initAM();
  pinMode(LED_BUILTIN, OUTPUT);
  while (!Serial);
}

void loop() {
  Move moves[MAX_MOVES];
  int movesRecv = 0;
  bool done = false;
  while (movesRecv < MAX_MOVES && !done) {
    getMoveFromA3(moves, movesRecv, done);
  }

  for (int i = 0; i < movesRecv; ++i) {
    const Move& m = moves[i];
    Serial.print(static_cast<int>(m.gas));
    Serial.print(", ");
    Serial.print(static_cast<int>(m.steer));
    Serial.print(", ");
    Serial.print(static_cast<int>(m.speed));
    Serial.println();
  }

  for (int i = 0; i < movesRecv; ++i) {
    broadcastMove(moves[i]);
    waitForMoveToFinish();
  }

  delay(2000);
}

void broadcastMove(const Move& move) {
  byte a2Message[MESSAGE_SIZE] = { RUN_COMMAND, static_cast<byte>(move.steer), 0, 0 };
  byte a1Message[MESSAGE_SIZE] = { RUN_COMMAND, static_cast<byte>(move.gas), static_cast<byte>(move.speed), 0 };

  Wire.beginTransmission(A2_ADDRESS);
  Wire.write(a2Message, MESSAGE_SIZE);
  Wire.endTransmission();

  Wire.beginTransmission(A1_ADDRESS);
  Wire.write(a1Message, MESSAGE_SIZE);
  Wire.endTransmission();
}

void waitForMoveToFinish() {
  bool a1Done = false;
  bool a2Done = false;

  while (!a1Done || !a2Done) {
    Wire.requestFrom(A1_ADDRESS, MESSAGE_SIZE);

    if (Wire.available() >= MESSAGE_SIZE && Wire.read() == WORK_DONE) {
      a1Done = true;
    }

    Wire.requestFrom(A2_ADDRESS, MESSAGE_SIZE);

    if (Wire.available() >= MESSAGE_SIZE && Wire.read() == WORK_DONE) {
      a2Done = true;
    }

    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(400);
  }
}

void getMoveFromA3(Move moves[], int& movesRecv, bool& done) {
  Serial.println("Requesting move from A3...");

  Wire.beginTransmission(A3_ADDRESS);
  byte msg[MESSAGE_SIZE] = { RUN_COMMAND, 0, 0, 0 };
  Wire.write(msg, MESSAGE_SIZE);
  Wire.endTransmission();

  byte response[MESSAGE_SIZE];

  while (true) {
    Wire.requestFrom(A3_ADDRESS, MESSAGE_SIZE);

    if (Wire.available() >= MESSAGE_SIZE) {
      byte firstByte = Wire.read();

      if (firstByte != NOT_READY) {
        response[0] = firstByte;

        for (int i = 1; i < MESSAGE_SIZE; ++i) {
          response[i] = Wire.read();
        }

        break;
      }
    }

    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(200);
  }

  if (response[0] == WORK_DONE) {
    Serial.println("All moves in -> Execute!");
    done = true;
  } else {

    Serial.print("Move received: ");
    for (int i = 0; i < MESSAGE_SIZE; ++i) {
      Serial.print(response[i]);
    }

    moves[movesRecv++] = Move{ static_cast<Gas>(response[0]), static_cast<SteerDir>(response[1]), static_cast<Speed>(response[2]) };

    Serial.println();
  }
}
