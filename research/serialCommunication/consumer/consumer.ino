#include <ArduinoJson.h>
#include <MessageLib.h>

unsigned long now = 0;
unsigned long prevTs = 0;
unsigned int interval = 1500;

void setup() {
  Serial.begin(9600);
}

void loop() {
  now = millis();

  if (now - interval > prevTs) {
    msg::Message message;

    if (msg::RecvMessage(message)) {
      Serial.println("Message Received!");
      Serial.println(message.time);
      Serial.println(message.duration);
      Serial.println(message.x);
      Serial.println(message.y);
      Serial.println();
    }
    else
    {
      Serial.println("No messages");
    }
    prevTs = now;
  }
}
