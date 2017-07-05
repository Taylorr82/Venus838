/*
    Venus838.hpp - Library for configuration of Skytraq's Venus838 chipset
    Reed A. Foster, June 2017.
*/

#ifndef Venus838_h
#define Venus838_h

#include "Arduino.h"

#define GPS_DEBUG_BAUDRATE 115200

// NMEA strings enum definitions
#define GGA 0
#define GSA 1
#define GSV 2
#define GLL 3
#define RMC 4
#define VTG 5
#define ZDA 6

// Return status/error codes
#define NORMAL     0
#define NACK       1
#define TIMEOUT    2
#define INVALIDARG 3
#define UNKNOWN    4

#define TIMEOUTMS 1000 //default wait time for how long the sender should wait for ack

#define DEFAULTBAUDRATE 9600 //default baud rate of GPS receiver (should be 9600)

class Venus838
{
public:

    Venus838(HardwareSerial &serial, int baudrate, bool reset); //pass hardwareserial port by reference

    // Receiver configuration/command methods
    char setBaudRate(int baudrate, char attribute);
    char setUpdateRate(int frequency, char attribute);
    char resetReceiver(bool reboot);
    char querySoftwareVersion();
    char cfgNMEA(char messagename, bool enable, char attribute);
    char cfgNMEA(char nmeabyte, char attribute);
    char cfgPowerSave(bool enable, char attribute);
    char cfgPPS(char mode, char attribute);

    //Serial wrapper methods
    bool available();
    char read();

private:

    const int _baudrates[6] = {4800, 9600, 19200, 38400, 57600, 115200};
    char _nmeastate; //stores current configuration of which NMEA strings are enabled
    HardwareSerial _serial;

    int _getBaudRate(HardwareSerial &serial);
    char _sendCommand(char messageid, char* messagebody, int bodylen); //uses default timeout specified by TIMEOUTMS
    char _sendCommand(char messageid, char* messagebody, int bodylen, uint timemout);
    char _sendPacket(char* packet, int size, uint timeout);

    // debug
    void _printPacket(char* packet, int size);
    void _debug(const char* message);
    void _debug(int number);
};

#endif
