#include <LiquidCrystal.h>

#include "ProgCar.h"

#define GAS_PIN 7
#define STEER_PIN 8
#define SPEED_PIN 9
#define SEND_PIN 10
#define EXECUTE_PIN 13

#define DEBOUNCE_DELAY 50

using namespace pgc;

// Button class for holding pin, state, and debounce variables.
struct button {

  int pin;
  int curr_state;
  int last_state;
  int last_debounce;

  button(int pin, int curr_state, int last_state, int last_debounce) {
    this->pin = pin;
    this->curr_state = curr_state;
    this->last_state = last_state;
    this->last_debounce = last_debounce;
  }

};

// Returns 1 if button has been pressed, else returns 0.
int button_get_input(button* button) {

  int reading = digitalRead(button->pin);

  if (reading != button->last_state) {
    button->last_debounce = millis();
  }

  if ((millis() - button->last_debounce) > DEBOUNCE_DELAY) {
    if (reading != button->curr_state) {
      button->curr_state = reading;
      if (button->curr_state == HIGH) {
        button->last_state = reading;
        return 1;
      }
    }
  }

  button->last_state = reading;
  return 0;

}

// button setup
button gas(GAS_PIN, HIGH, HIGH, 0);
button steer(STEER_PIN, HIGH, HIGH, 0);
button speed(SPEED_PIN, HIGH, HIGH, 0);
button send(SEND_PIN, HIGH, HIGH, 0);
button execute(EXECUTE_PIN, HIGH, HIGH, 0);

// LCD setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// time variables
unsigned long curr_millis, prev_millis = 0;

// array and position variables for cycling choices
Gas gas_arr[2] = {Gas::FORWARD, Gas::REVERSE};
int gas_pos = 0;
SteerDir steer_arr[3] = {SteerDir::STRAIGHT, SteerDir::LEFT, SteerDir::RIGHT};
int steer_pos = 0;
Speed speed_arr[3] = {Speed::MEDIUM_SPEED, Speed::HIGH_SPEED, Speed::LOW_SPEED};
int speed_pos = 0;

// amount of moves read
unsigned int moves_amt = 0;

/*
Prints current move in a slot machine style on LCD.
- Format -> G:F St:S Sp:M   X
*/
void print_slotMachine(Move* move, int changed) {

  lcd.setCursor(0, 0);

  // print choice slots
  lcd.print("G:");

  switch (move->gas) {
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

  switch (move->steer) {
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

  switch (move->speed) {
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
  lcd.print(moves_amt);

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
int a3_getMove(Move* new_move) {

  *new_move = {gas_arr[gas_pos], steer_arr[steer_pos], speed_arr[speed_pos]};

  int changed = 1;

  int reading = 1;

  while (reading) {

    if (button_get_input(&gas) == 1) {
      // cycle gas
      gas_pos = (gas_pos + 1) % 2;
      new_move->gas = gas_arr[gas_pos];
      changed = 1;
      lcd.clear();
    } else if (button_get_input(&steer) == 1) {
      // cycle steer
      steer_pos = (steer_pos + 1) % 3;
      new_move->steer = steer_arr[steer_pos];
      changed = 2;
      lcd.clear();
    } else if (button_get_input(&speed) == 1) {
      // cycle speed
      speed_pos = (speed_pos + 1) % 3;
      new_move->speed = speed_arr[speed_pos];
      changed = 3;
      lcd.clear();
    } else if (button_get_input(&send) == 1) {
      // leave to send current move
      lcd.clear();
      moves_amt++;
      return 1;
    } else if (button_get_input(&execute) == 1) {
      // leave to send current move and report work done
      return 0;
    }

    print_slotMachine(new_move, changed);

  }

}

void getMove() {
  
  if (moves_amt < MAX_MOVES) {

    Move move;

    if (a3_getMove(&move) == 1) {
      // send a move and prepare to collect another
      reportToMaster(move);
      moves_amt++;
    } else {
      // tell AM we are done collecting moves
      reportToMaster(pgc::WORK_DONE);
      moves_amt = 0;
    }

  } else {

    reportToMaster(pgc::WORK_DONE);
    moves_amt = 0;

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

  // LCD setup
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
}

void loop() {
  // live code
  handleMovesRequest(getMove);

  // debug code
  //if (moves_amt < MAX_MOVES) {
    //Move move;
    //a3_getMove(&move);
  //}
}
