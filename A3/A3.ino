#include <LiquidCrystal.h>

#include "ProgCar.h"

#define GAS_PIN 6
#define STEER_PIN 7
#define SPEED_PIN 8
#define SEND_PIN 9
#define EXECUTE_PIN 10

// LED lights up when A3 is taking input, turns off when sleeping
#define LED_PIN 13

#define DEBOUNCE_DELAY 50

using namespace pgc;

// Button class for holding pin, state, and debounce variables.
struct Button {

  int pin;
  int currState;
  int lastState;
  int lastDebounce;

  Button(int pin) {
    this->pin = pin;
    this->currState = HIGH;
    this->lastState = HIGH;
    this->lastDebounce = 0;
  }

};

// Returns 1 if button has been pressed, else returns 0.
int buttonGetInput(Button& button) {

  int reading = digitalRead(button.pin);

  if (reading != button.lastState) {
    button.lastDebounce = millis();
  }

  if ((millis() - button.lastDebounce) > DEBOUNCE_DELAY) {
    if (reading != button.currState) {
      button.currState = reading;
      if (button.currState == HIGH) {
        button.lastState = reading;
        return 1;
      }
    }
  }

  button.lastState = reading;
  return 0;

}

// button setup
Button gas(GAS_PIN);
Button steer(STEER_PIN);
Button speed(SPEED_PIN);
Button send(SEND_PIN);
Button execute(EXECUTE_PIN);

// LCD setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// time variables
unsigned long currMillis, prevMillis = 0;

// array and position variables for cycling choices
Gas gasArr[2] = {Gas::FORWARD, Gas::REVERSE};
int gasPos = 0;
SteerDir steerArr[3] = {SteerDir::STRAIGHT, SteerDir::LEFT, SteerDir::RIGHT};
int steerPos = 0;
Speed speedArr[3] = {Speed::MEDIUM_SPEED, Speed::HIGH_SPEED, Speed::LOW_SPEED};
int speedPos = 0;

// amount of moves read
unsigned int movesAmt = 0;

/*
Prints current move in a slot machine style on LCD.
- Format -> G:F St:S Sp:M   X
*/
void printSlotMachine(Move& move, int changed) {

  lcd.setCursor(0, 0);

  // print choice slots
  lcd.print("G:");

  switch (move.gas) {
    case Gas::FORWARD:
      lcd.print("F");
      break;
    case Gas::REVERSE:
      lcd.print("B");
      break;
    default:
      break;
  };

  lcd.print(" St:");

  switch (move.steer) {
    case SteerDir::LEFT:
      lcd.print("L");
      break;
    case SteerDir::RIGHT:
      lcd.print("R");
      break;
    case SteerDir::STRAIGHT:
      lcd.print("S");
      break;
    default:
      break;
  }

  lcd.print(" Sp:");

  switch (move.speed) {
    case Speed::LOW_SPEED:
      lcd.print("L");
      break;
    case Speed::MEDIUM_SPEED:
      lcd.print("M");
      break;
    case Speed::HIGH_SPEED:
      lcd.print("H");
      break;
    default:
      break;
  }

  // print amount of moves we have
  lcd.print("  ");
  lcd.print(movesAmt);

  // print ^ icon for which thing is being changed
  lcd.setCursor(0,1);
  switch(changed) {
    case 1:
      lcd.print("  ^");
      break;
    case 2:
      lcd.print("       ^");
      break;
    case 3:
      lcd.print("            ^");
      break;
    default:
      break;
  }

}

/* 
Main A3 do-work function. Returns 1 to continue collecting moves, else returns 0 if user wants less than 10 moves.
*/
int a3GetMove(Move& newMove) {

  newMove = {gasArr[gasPos], steerArr[steerPos], speedArr[speedPos]};

  int changed = 1;

  digitalWrite(LED_PIN, HIGH);

  while (true) {

    if (buttonGetInput(gas) == 1) {
      // cycle gas
      gasPos = (gasPos + 1) % 2;
      newMove.gas = gasArr[gasPos];
      changed = 1;
      lcd.clear();
    } else if (buttonGetInput(steer) == 1) {
      // cycle steer
      steerPos = (steerPos + 1) % 3;
      newMove.steer = steerArr[steerPos];
      changed = 2;
      lcd.clear();
    } else if (buttonGetInput(speed) == 1) {
      // cycle speed
      speedPos = (speedPos + 1) % 3;
      newMove.speed = speedArr[speedPos];
      changed = 3;
      lcd.clear();
    } else if (buttonGetInput(send) == 1) {
      // leave to send current move
      lcd.clear();
      return 1;
    } else if (buttonGetInput(execute) == 1) {
      // leave to send current move and report work done
      digitalWrite(LED_PIN, LOW);
      return 0;
    }

    printSlotMachine(newMove, changed);

  }

}

void getMove() {
  
  if (movesAmt < MAX_MOVES) {

    Move move;

    if (a3GetMove(move) == 1) {
      // send a move and prepare to collect another
      reportToMaster(move);
      movesAmt++;
    } else {
      // tell AM we are done collecting moves
      reportToMaster(pgc::WORK_DONE);
      movesAmt = 0;
    }

  } else {

    reportToMaster(pgc::WORK_DONE);
    movesAmt = 0;

  }

}

void setup() {
  initSlave(A3_ADDRESS);
  
  // Pushbutton setup
  pinMode(gas.pin, INPUT_PULLUP);
  pinMode(steer.pin, INPUT_PULLUP);
  pinMode(speed.pin, INPUT_PULLUP);
  pinMode(send.pin, INPUT_PULLUP);
  pinMode(execute.pin, INPUT_PULLUP);

  // LED setup
  pinMode(LED_PIN, OUTPUT);

  // LCD setup
  lcd.begin(16, 2);
}

void loop() {
  handleMovesRequest(getMove);
}