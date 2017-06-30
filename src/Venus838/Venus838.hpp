#ifndef Venus838_h
#define Venus838_h

#include "Arduino.h"

#define GGA 0
#define GSA 1
#define GSV 2
#define GLL 3
#define RMC 4
#define VTG 5
#define ZDA 6

//Return status/error codes
#define NORMAL     0
#define NACK       1
#define TIMEOUT    2
#define INVALIDARG 3
#define UNKNOWN    4

#define TIMEOUTMS 1000 //how long the sender should wait for ack

class Venus838
{
public:

    Venus838(HardwareSerial &serial = Serial1, int baudrate = 9600);

    char setBaudRate(char baudrate, char attribute);

    char setUpdateRate(int frequency, char attribute);

    char reset(bool reboot);

    char cfgNMEA(char messagename, bool enable, char attribute);

    char cfgPowerSave(bool enable, char attribute);

    char cfgPPS(char mode, char attribute);

    bool dataAvailable();

    char read();

    char cfgEchoMSG(bool enable);

private:

    char _sendCommand(char* messagebody, int bodylen);
    int _baudrates[6] = {4800, 9600, 19200, 38400, 57600, 115200};
    char _nmeastate;
    HardwareSerial _serial
};

#endif
