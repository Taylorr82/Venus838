/*
    Venus838.cpp - Library for configuration of Skytraq's Venus838 chipset
    Reed A. Foster, June 2017.
*/

#include "Arduino.h"
#include "Venus838.hpp"

Venus838::Venus838(HardwareSerial &serial, int baudrate)
{
    _serial = serial;
    if (baudrate == DEFAULTBAUDRATE)
    {
        _serial.begin(baudrate);
    }
    else
    {
        // will add code to test each baud rate to autodetect
        // current configuration, for now just assume receiver
        // is configured with the default
        _serial.begin(DEFAULTBAUDRATE);
        reset(true);
        setBaudRate(baudrate, 0);
    }
    _nmeastate = 0b1111111; //enable all nmea messages;
}

// Attribute:
// 0 = only update RAM
// 1 = update RAM and flash
char Venus838::setBaudRate(int baudrate, char attribute)
{
    char messagebody[3];
    memset(messagebody, 0, 3);
    messagebody[0] = 0x00; //COM port 1
    bool baudrateset = false;
    for (int i = 0; i < 6; i++)
    {
        if (baudrate == _baudrates[i])
        {
            messagebody[1] = i;
            baudrateset = true;
        }
    }
    if (!baudrateset)
    {
        return INVALIDARG;
    }
    messagebody[2] = attribute;
    char code = _sendCommand(0x05, messagebody, 3); //messageid = 5
    if (code == NORMAL)
    {
        _serial.end();
        _serial.begin(baudrate); //restart the serial port to the new baudrate
        return code;
    }
    return code;
}

// Attribute:
// 0 = only update RAM
// 1 = update RAM and flash
char Venus838::setUpdateRate(int frequency, char attribute)
{
    char messagebody[2];
    memset(messagebody, 0, 2);
    messagebody[0] = frequency;
    messagebody[1] = attribute;
    return _sendCommand(0x0E, messagebody, 2);
}

char Venus838::reset(bool reboot)
{
    char messagebody[1];
    memset(messagebody, 0, 1);
    messagebody[0] = reboot ? 1 : 0;
    return _sendCommand(0x04, messagebody, 1, 10000);
}

// Attribute:
// 0 = only update RAM
// 1 = update RAM and flash
char Venus838::cfgNMEA(char messagename, bool enable, char attribute)
{
    if (enable)
        _nmeastate |= 1 << messagename;
    else
        _nmeastate &= ~(1 << messagename);
    return cfgNMEA(_nmeastate, attribute);
}

// Attribute:
// 0 = only update RAM
// 1 = update RAM and flash
// 2 = temporarily enabled
char Venus838::cfgPowerSave(bool enable, char attribute)
{
    char messagebody[2];
    memset(messagebody, 0, 2);
    messagebody[0] = enable ? 1 : 0;
    messagebody[1] = attribute;
    return _sendCommand(0x0C, messagebody, 2);
}

// Attribute:
// 0 = off
// 1 = on only when 3D fix
// 2 = on when at least 1 SV
char Venus838::cfgPPS(char mode, char attribute)
{
    char messagebody[2];
    memset(messagebody, 0, 2);
    messagebody[0] = mode;
    messagebody[1] = attribute;
    return _sendCommand(0x3E, messagebody, 2);
}

// Attribute:
// 0 = only update RAM
// 1 = update RAM and flash
char Venus838::cfgNMEA(char nmeabyte, char attribute)
{
    char messagebody[8];
    memset(messagebody, 0, 8);
    messagebody[0] = (nmeabyte >> GGA) & 1;
    messagebody[1] = (nmeabyte >> GSA) & 1;
    messagebody[2] = (nmeabyte >> GSV) & 1;
    messagebody[3] = (nmeabyte >> GLL) & 1;
    messagebody[4] = (nmeabyte >> RMC) & 1;
    messagebody[5] = (nmeabyte >> VTG) & 1;
    messagebody[6] = (nmeabyte >> ZDA) & 1;
    messagebody[7] = attribute;
    return _sendCommand(0x08, messagebody, 8);
}

bool Venus838::available()
{
    return _serial.available();
}

char Venus838::read()
{
    return _serial.read();
}

char Venus838::_sendCommand(char messageid, char* messagebody, int bodylen)
{
    return _sendCommand(messageid, messagebody, bodylen, TIMEOUTMS);
}

char Venus838::_sendCommand(char messageid, char* messagebody, int bodylen, uint timeout)
{
    // Assemble Packet
    int packetlength = 8 + bodylen;
    char packet[packetlength];
    memset(packet, 0, packetlength);

    packet[0] = 0xA0; //start sequence
    packet[1] = 0xA1;

    packet[2] = (char) ((bodylen + 1) >> 8); //payload length includes message id
    packet[3] = (char) bodylen + 1;

    packet[4] = messageid;

    // calculate checksum
    char checksum = messageid;
    for (int i = 5; i < packetlength - 3; i++)
    {
        packet[i] = messagebody[i - 5];
        checksum ^= packet[i];
    }
    packet[packetlength - 3] = checksum;

    packet[packetlength - 2] = 0x0D; //terminate command with crlf
    packet[packetlength - 1] = 0x0A;

    // Send Packet
    char c = 0;
    char last = 0;
    bool response = false;
    _serial.write(packet, packetlength);
    // wait for repsonse
    for(uint start = millis(); millis() - start < timeout;)
    {
        while (_serial.available())
        {
            c = _serial.read();
            if (last == 0xA0 and c == 0xA1 and response == false)
                response = true;
            if (response and last == 0x83)
            {
                if (c == messageid)
                    return NORMAL;
                else
                    return UNKNOWN;
            }
            else if (response and last == 0x84)
            {
                if (c == messageid)
                    return NACK;
                else
                    return UNKNOWN;
            }
            last = c;
        }
    }
    return TIMEOUT;
}

void Venus838::_printpacket(char* packet, int size)
{
    Serial.print("Assembled Packet: ");
    for (int i = 0; i < size; i++)
    {
        Serial.print(packet[i], 16);
        Serial.print(" ");
    }
    Serial.println("");
}
