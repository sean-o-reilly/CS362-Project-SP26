#include <MessageLib.h>

unsigned long now = 0;
unsigned long prevTs = 0;
unsigned int interval = 2000;

void setup() {
  Serial.begin(9600);
}

void loop() {
  now = millis();
  
  if (now - interval > prevTs) {
    msg::Message newMessage{ .time = millis(), .duration = rand() % 1000, .x = rand() % 50, .y = rand() % 50 };
    msg::Broadcast(newMessage);

    prevTs = now;
  }
}