#include <LiquidCrystal.h>

#include "ProgCar.h"

#define DEBOUNCE_DELAY 50

using namespace pgc;

class A3 {

  class Button {

    private:
      const unsigned int pinNumber;
      unsigned int currState, prevState;
      unsigned long lastDebounce;

    public:

      Button(int pinNumber) {
        this->pinNumber = pinNumber;
        this->currState = LOW;
        this->prevState = LOW;
        this->lastDebounce = 0;
        pinMode(pin, INPUT_PULLUP);
      }

      /*
      Returns 1 if button is pressed, else returns 0.
      */
      int getInput() {
        int reading = digitalRead(this->pinNumber);

        if (reading != this->prevState) {
          lastDebounce = millis();
        }

        if ((millis() - this->lastDebounce) > DEBOUNCE_DELAY) {
          if (reading != this->currState) {
            this->currState = reading;
            if (this->currState == HIGH) {
              return 1;
            }
          }
        }

        last_left_button_state = reading;
        return 0;
      }

  }

  private:
    Button execute, forward, backward, left, right, speed;
    LiquidCrystal lcd;

  public:
    A3(int execute_pin, 
    int forward_pin, 
    int backward_pin, 
    int left_pin, 
    int right_pin,
    int speed_pin) {
      
    }

};

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
