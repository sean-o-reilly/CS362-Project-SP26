#include <LiquidCrystal.h>

#include "ProgCar.h"

/*

Input state transitions (assuming max_moves is not accounted for):

- gas_0: # collecting gas with no existing moves
  - f -> steer
  - b -> steer
  - l -> null
  - r -> null
  - s -> null
  - e -> null

- gas_1: # collecting gas with an existing move
  - f -> steer
  - b -> steer
  - l -> null
  - r -> null
  - s -> null
  - e -> execute

- steer:
  - f -> steer
  - b -> steer
  - l -> speed
  - r -> speed
  - s -> null
  - e -> execute

- speed:
  - f -> steer
  - b -> steer
  - l -> null
  - r -> null
  - s -> gas
  - e -> execute

*/

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
      int checkInput() {
        int res = 0;
        int reading = digitalRead(this->pinNumber);

        if (reading != this->prevState) {
          lastDebounce = millis();
        }

        if ((millis() - this->lastDebounce) > DEBOUNCE_DELAY) {
          if (reading != this->currState) {
            this->currState = reading;
            if (this->currState == HIGH) {
              res = 1;
            }
          }
        }

        last_left_button_state = reading;
        return res;
      }

  }

  class LCD {

    private:
      //unsigned const int rs, en, d4, d5, d6, d7;
      unsigned int cursorRow, cursorCol;
      String topText, bottomText;
      LiquidCrystal lcd;
    
    public:
      LCD(int rs, en, d4, d5, d6, d7) {
        this->lcd(rs, en, d4, d5, d6, d7)
        this->cursorRow = 0;
        this->cursorCol = 0;
        this->topText = "";
        this->bottomText = "";
      }

      void setTopText(String newText) {
        this->topText = newText;
      }

      void setBottomText(String newText) {
        this->bottomText = newText;
      }

      void appendBottomText(String newText) {
        this->buttomText += newText;
      }

      void display() {
        this->lcd.setCursor(0, 0);
        this->lcd.display(this->topText);
        this->lcd.setCursor(1, 0);
        this->lcd.display(this->bottomText);
      }

  }

  private:

    Button execute, forward, backward, left, right, speed;
    LCD lcd;
    Move moveArray[MAX_MOVES];
    int arrSize;

  public:

    A3(int execute_pin, 
    int forward_pin, 
    int backward_pin, 
    int left_pin, 
    int right_pin,
    int speed_pin,
    int rs, int en,
    int d4, int d5,
    int d6, int d7,) {
      this->execute(execute_pin);
      this->forward(forward_pin);
      this->backward(backward_pin);
      this->left(left_pin);
      this->right(right_pin);
      this->speed(speed_pin);

      this->lcd(rs, en, d4, d5, d6, d7);

      this->arrSize = 0;
    }

    void fillMoveArray() {

      int gas, steer, speed = 0;

      int filling = 1;

      while (filling) {

        Move currMove = {FORWARD, STRAIGHT, MEDIUM_SPEED};

        gas = 1;

        while (gas) {

          if (this->forward.checkInput() == 1) {

            currMove.gas = FORWARD;
            steer = 1;

          } else if (this->execute.checkInput() == 1) {
              
            gas = 0;
          
          }

          while (steer) {

            if (this->forward.checkInput() == 1) {

              this->moveArray[this->arrSize] = currMove;
              this->arrSize++;

              if (this->arrSize == 10) gas, steer = 0;

            } else if (this->left.checkInput() == 1) {

              currMove.steer = LEFT;
              speed = 1;

            }

            while (speed) {

              if (this->forward.checkInput() == 1) {

                this->moveArray[this->arrSize] = currMove;
                this->arrSize++;
                speed = 0;

                if (this->arrSize == 10) gas, steer = 0;

              } else if (this->speed.checkInput() == 1) {

                switch (currMove.speed) {

                  case LOW_SPEED:
                    currMove.speed = MEDIUM_SPEED;
                    break;

                  case MEDIUM_SPEED:
                    currMove.speed = HIGH_SPEED;
                    break;

                  case HIGH_SPEED:
                    currMove.speed = LOW_SPEED;
                    break;

                }

              }

            }

          }

        }

        if (this->arrSize == 10) {
          filling = 0;
        }

      }

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
