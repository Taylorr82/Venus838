/*
    Venus838.hpp - Library for configuration of Skytraq's Venus838 chipset
    Reed A. Foster, June 2017.
*/

#ifndef Venus838_h
#define Venus838_h

#include "Arduino.h"

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

#define DEFAULTBAUDRATE 9600

class Venus838
{
public:

    Venus838(HardwareSerial &serial, int baudrate); //pass hardwareserial port by reference

    char setBaudRate(int baudrate, char attribute);

    char setUpdateRate(int frequency, char attribute);

    char reset(bool reboot);

    // enable/disable a single NMEA string
    char cfgNMEA(char messagename, bool enable, char attribute);

    // enable/disable all NMEA strings based on a single byte
    // bit 0: GGA, 1: GSA, 2: GSV, 3: GLL, 4: RMC, 5: VTG, 6: ZDA
    char cfgNMEA(char nmeabyte, char attribute);

    // enter/exit power-saving mode
    char cfgPowerSave(bool enable, char attribute);

    // configure behavior of the pulse-per-second pin
    char cfgPPS(char mode, char attribute);

    bool available();

    char read();

private:

    char _sendCommand(char messageid, char* messagebody, int bodylen); //uses default timeout specified by TIMEOUTMS
    char _sendCommand(char messageid, char* messagebody, int bodylen, uint timemout);
    const int _baudrates[6] = {4800, 9600, 19200, 38400, 57600, 115200};
    char _nmeastate; //stores current configuration of which NMEA strings are enabled
    HardwareSerial _serial;

    // debug
    void _printpacket(char* packet, int size);
};

#endif
