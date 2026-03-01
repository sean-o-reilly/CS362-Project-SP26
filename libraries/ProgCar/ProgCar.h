#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>

namespace pgc
{
    constexpr int BAUD_RATE = 9600;
    constexpr int MAX_WIRE_BYTES = 32;

    constexpr int A1_ADDRESS = 8;
    constexpr int A2_ADDRESS = 9;
    constexpr int A3_ADDRESS = 10;

    constexpr int MAX_MOVES = 10;

    constexpr int MESSAGE_SIZE = 4;
    byte slaveResponseBuffer[MESSAGE_SIZE];

    constexpr byte RUN_COMMAND = 1;
    constexpr byte WORK_DONE = 0xEE;
    constexpr byte NOT_READY = 0xFF;

    enum class Gas : byte 
    {
        FORWARD,
        REVERSE
    };

    enum class SteerDir : byte
    {
        LEFT,
        STRAIGHT,
        RIGHT
    };

    enum class Speed : byte
    {
        LOW_SPEED,
        MEDIUM_SPEED,
        HIGH_SPEED
    };

    namespace
    {
        const char* gasField = "g";
        const char* steerField = "st";
        const char* speedField = "sp";
    }

    struct alignas(4) Move
    {
        Gas gas;
        SteerDir steer;
        Speed speed;

        JsonDocument ToJson() const
        {
            JsonDocument json;
            json[gasField] = static_cast<byte>(gas);
            json[steerField] = static_cast<byte>(steer);
            json[speedField] = static_cast<byte>(speed);
            
            return json;
        }
    };

    void initAM()
    {
        Wire.begin();
        Serial.begin(BAUD_RATE);
    }

    void reportMove(const Move& move) // A3 -> AM
    {
        slaveResponseBuffer[0] = static_cast<byte>(move.gas);
        slaveResponseBuffer[1] = static_cast<byte>(move.steer);
        slaveResponseBuffer[2] = static_cast<byte>(move.speed);
        slaveResponseBuffer[3] = 0;
    }

    void reportDone()
    {
        for (int i = 0; i < MESSAGE_SIZE; ++i)
        {
            slaveResponseBuffer[i] = WORK_DONE;
        }
    }

    namespace // slave event handlers
    {
        bool slaveResponseReady = false;
        bool slaveCommandRecv = false;

        void onSlaveCommandRecv(int bytesRecv) { // called when master sends slave a command to execute
            if (bytesRecv <= 0)
                return;

            if (!slaveResponseReady) // only carry out command if move isn't already ready
            {
                byte command = Wire.read();

                if (command == RUN_COMMAND) {
                    slaveCommandRecv = true;
                }
            }
        }

        void onSlaveResponseRequested() { // called when master requests response from slave
            if (slaveResponseReady) {
                Wire.write(slaveResponseBuffer, sizeof(slaveResponseBuffer));
                slaveResponseReady = false;
            } else {
                byte notReady[MESSAGE_SIZE] = {NOT_READY, NOT_READY, NOT_READY, NOT_READY};   
                Wire.write(notReady, MESSAGE_SIZE);
            }
        }
    }

    void runSlaveCommand(void (*func)())
    {
        if (slaveCommandRecv) {
            slaveCommandRecv = false;
            func();
            slaveResponseReady = true;
        }
    }

    void initSlave(const int address)
    {
        Wire.begin(address);
        Wire.onReceive(onSlaveCommandRecv);
        Wire.onRequest(onSlaveResponseRequested);
        pinMode(LED_BUILTIN, OUTPUT);
    }
}