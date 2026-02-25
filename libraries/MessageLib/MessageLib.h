// Library for shared code between producers and consumers
#include <Arduino.h>
#include <ArduinoJson.h>

#ifndef MessageLib_h
#define MessageLib_h

namespace msg 
{
    namespace // anon namespace for string alias's
    {
        const char* fieldTime = "time";
        const char* fieldDuration = "duration";
        const char* fieldX = "x";
        const char* fieldY = "y";
    }

    struct Message
    {
        JsonDocument ToJson()
        {
            JsonDocument res;

            res[fieldTime] = time;
            res[fieldDuration] = duration;
            res[fieldX] = x;
            res[fieldY] = y;

            return res;
        }

        // placeholder fields for what message may contain
        // Note: enum classes would be a great option here when our protocol is closer to finalized
        unsigned long time;
        unsigned long duration;
        unsigned long x;
        unsigned long y;
    };

    bool Broadcast(const Message& message)
    {
        serializeJson(message.ToJson(), Serial);
        Serial.println();
    }

    bool RecvMessage(Message& message)
    {
        if (Serial.available() > 0) {
            String recvStr = Serial.readStringUntil('\n');
            JsonDocument doc;
            deserializeJson(doc, recvStr);

            message = Message{.time = doc[fieldTime], .duration = doc[fieldDuration], .x = doc[fieldX], .y = doc[fieldY]};
            return true;
        }

        return false;
    }
}

#endif