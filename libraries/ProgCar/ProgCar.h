#include <Arduino.h>
#include <Wire.h>

namespace pgc
{
    constexpr int BAUD_RATE = 9600;

    constexpr int A1_ADDRESS = 8;
    constexpr int A2_ADDRESS = 9;
    constexpr int A3_ADDRESS = 10;

    constexpr int MAX_MOVES = 10;

    constexpr int MESSAGE_SIZE = 4;
    byte slaveResponseBuffer[MESSAGE_SIZE];

    /* Protocol response codes */
    constexpr byte RUN_COMMAND = 1;
    constexpr byte WORK_DONE = 0xEE;
    constexpr byte NOT_READY = 0xFF;
    constexpr byte NULL_RESPONSE = 0x00;

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

    struct alignas(4) Move
    {
        Gas gas;
        SteerDir steer;
        Speed speed;
    };
    
    void reportToMaster(const Move& move)
    {
        slaveResponseBuffer[0] = static_cast<byte>(move.gas);
        slaveResponseBuffer[1] = static_cast<byte>(move.steer);
        slaveResponseBuffer[2] = static_cast<byte>(move.speed);
        slaveResponseBuffer[3] = 0; // junk byte
    }

    void reportToMaster(const byte status)
    {
        for (int i = 0; i < MESSAGE_SIZE; ++i)
        {
            slaveResponseBuffer[i] = status;
        }
    }

    namespace // Slave event handlers
    {
        bool slaveResponseReady = false;
        bool slaveCommandShouldRun = false;

        /* Called when master sends slave a command to execute */
        void onSlaveCommandRecv(int bytesRecv) {
            if (bytesRecv <= 0)
                return;

            if (!slaveResponseReady) // Only carry out command if move isn't already ready
            {
                byte command = Wire.read();

                if (command == RUN_COMMAND) {
                    slaveCommandShouldRun = true;
                }
            }
        }

        /* Called when master requests a response from slave */
        void onSlaveResponseRequested() {
            if (slaveResponseReady) {
                Wire.write(slaveResponseBuffer, sizeof(slaveResponseBuffer));
                slaveResponseReady = false;
            } else {
                byte notReady[MESSAGE_SIZE] = {NOT_READY, NOT_READY, NOT_READY, NOT_READY};   
                Wire.write(notReady, MESSAGE_SIZE);
            }
        }
    }

    /* Will call func if master has requested slave to run */
    void runSlaveCommand(void (*func)())
    {
        if (slaveCommandShouldRun) {
            slaveCommandShouldRun = false;
            for (int i = 0; i < MESSAGE_SIZE; ++i)
            {
                slaveResponseBuffer[i] = NULL_RESPONSE;
            }
            func();
            slaveResponseReady = true;
        }
    }

    void initAM()
    {
        Wire.begin();
        Serial.begin(BAUD_RATE);
    }

    void initSlave(const int address)
    {
        Wire.begin(address);
        Wire.onReceive(onSlaveCommandRecv);
        Wire.onRequest(onSlaveResponseRequested);
        pinMode(LED_BUILTIN, OUTPUT);
    }
}
