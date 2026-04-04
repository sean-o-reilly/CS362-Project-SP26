#include <LiquidCrystal.h>

//#include "ProgCar.h"

#define DRIVE_PIN 7
#define REVERSE_PIN 8
#define LEFT_PIN 9
#define RIGHT_PIN 10
//#define SPEED_PIN 10
#define EXECUTE_PIN 13

#define DEBOUNCE_DELAY 50

//using namespace pgc;

// Button class and set up
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

button drive(DRIVE_PIN, HIGH, HIGH, 0);
button reverse(REVERSE_PIN, HIGH, HIGH, 0);
button left(LEFT_PIN, HIGH, HIGH, 0);
button right(RIGHT_PIN, HIGH, HIGH, 0);
//button speed(SPEED_PIN, HIGH, HIGH, 0);
button execute(EXECUTE_PIN, HIGH, HIGH, 0);

// LCD setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// time variables
unsigned long curr_millis, prev_millis = 0;

// from ProgCar.h
    enum Gas
    {
        FORWARD,
        REVERSE
    };

    enum SteerDir
    {
        LEFT,
        STRAIGHT,
        RIGHT
    };

    enum Speed
    {
        LOW_SPEED,
        MEDIUM_SPEED,
        HIGH_SPEED
    };

    struct Move
    {
        Gas gas;
        SteerDir steer;
        Speed speed;
    };

void print_move(struct Move move) {

  lcd.setCursor(0, 0);
  lcd.print("Move: ");

  switch (move.gas) {
    case FORWARD:
      lcd.print("F");
      break;
    case REVERSE:
      lcd.print("B");
      break;
    default:
      break;
  };

  switch (move.steer) {
    case LEFT:
      lcd.print("L,");
      break;
    case RIGHT:
      lcd.print("R,");
      break;
    case STRAIGHT:
      lcd.print("S,");
      break;
    default:
      break;
  }

  switch (move.speed) {
    case LOW_SPEED:
      lcd.print("(L)");
      break;
    case MEDIUM_SPEED:
      lcd.print("(M)");
      break;
    case HIGH_SPEED:
      lcd.print("(H)");
      break;
    default:
      break;
  }

}

/* 
Main A3 do-work function. Will change the return type to int to signify early exit.

Sean recommends making input like slot machine, where each button cycles the choice for gas, steer, speed respectively.
*/
struct Move a3_getMove() {

  Move new_move;

  //int reading = 1;

  // get gas choice

  //lcd.print("gas choice");

  print_move(new_move);
  lcd.setCursor(0, 1);
  lcd.print("choose gas");

  while (1) {

    if (button_get_input(&drive) == 1) {
      new_move.gas = FORWARD;
      break;
    } else if (button_get_input(&reverse) == 1) {
      new_move.gas = REVERSE;
      break;
    }

  }

  // get dir choice

  //lcd.clear();
  //lcd.print("speed choice");
  print_move(new_move);
  lcd.setCursor(0, 1);
  lcd.print("choose steer");

  while (1) {

    if (button_get_input(&left) == 1) {
      new_move.steer = LEFT;
      break;
    } else if (button_get_input(&right) == 1) {
      new_move.steer = RIGHT;
      break;
    } else if (button_get_input(&execute) == 1) {
      new_move.steer = STRAIGHT;
      break;
    }

  }

  // get speed choice
  // currently missing 6th button, will skip for now.
  new_move.speed = MEDIUM_SPEED;

  return new_move;

}

void setup() {
  //initSlave(A3_ADDRESS);
  
  // Pushbutton setup
  pinMode(drive.pin, INPUT_PULLUP);
  pinMode(reverse.pin, INPUT_PULLUP);
  pinMode(left.pin, INPUT_PULLUP);
  pinMode(right.pin, INPUT_PULLUP);
  //pinMode(speed.pin, INPUT_PULLUP);
  pinMode(execute.pin, INPUT_PULLUP);

  // LCD setup
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
}

/*
void getMove() {
  static int moves_set = 0;

  if (moves_sent < MAX_MOVES) {
    ++moves;
  } else {
    reportToMaster(pgc::WORK_DONE);
    moves = 0;
  }

}
*/

void loop() {
  //runSlaveCommand(getMove);
  Move move = a3_getMove();
  lcd.clear();
  print_move(move);
  lcd.setCursor(0, 1);
  lcd.print("final move");
  delay(5000);
}
