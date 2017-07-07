/*
    NMEAParser.hpp - Library for parsing NMEA strings from a GPS
    Reed A. Foster, July 2017.
*/

#ifndef NMEAParser_h
#define NMEAParser_h

#include "Arduino.h"
#include "Venus838.hpp"

#define NMEATERM_MAXLENGTH 15

class NMEAParser
{
public:
    NMEAParser();

    // returns true if a NMEA sentence has successfully been decoded
    bool encode(char c);

    inline unsigned long getTime() {return _time;}
    inline unsigned short getDate() {return _date;}
    inline long getLatitude() {return _latitude;}
    inline long getLongitude() {return _longitude;}
    inline long getAltitude() {return _altitude;}
    inline short getPDOP() {return _pdop;}
    inline short getVDOP() {return _vdop;}
    inline short getHDOP() {return _hdop;}
    inline char getNSats() {return _numsats;}
    inline char getFixQuality() {return _fixquality;}
    inline char getFixType() {return _fixtype;}
    inline long getSpeed() {return _speed;}
    inline short getCourse() {return _course;}
    inline long timeAge() {return _last_time_fix;}
    inline long positionAge() {return _last_position_fix;}

private:

    int _hexToInt(char c);
    int _strcmp(const char *str1, const char *str2);
    long _parse_decimal();
    long _parse_degrees();
    void _logTerm();

    char _term[NMEATERM_MAXLENGTH];
    int _term_offset;
    int _term_number;
    bool _is_checksum_term;

    int _sentence_type;

    int _parity;

    const char _GPGGA_TERM[7] = "$GPGGA";
    const char _GPGLL_TERM[7] = "$GPGLL";
    const char _GPGSA_TERM[7] = "$GPGSA";
    const char _GPGSV_TERM[7] = "$GPGSV";
    const char _GPRMC_TERM[7] = "$GPRMC";
    const char _GPVTG_TERM[7] = "$GPVTG";
    const char _GPZDA_TERM[7] = "$GPZDA";

    unsigned long   _time, _new_time; // UTC time in hundredths of a second
    unsigned short  _date, _new_date; // UTC date
    long            _latitude, _new_latitude; // latitude in hundred thousandths of a degree
    long            _longitude, _new_longitude; // longitude in hundred thousandths of a degree
    long            _altitude, _new_altitude; // altitude in centimeters
    unsigned long   _speed, _new_speed; // speed in hundredths of kph
    unsigned short  _course, _new_course; // course in hundredths of a degree
    unsigned short  _hdop, _new_hdop; // hdop (scaled by 100, i.e. 120 corresponds to a dop of 1.2)
    unsigned short  _vdop, _new_vdop; // vdop (same scale as hdop)
    unsigned short  _pdop, _new_pdop; // pdop (same scale as hdop)
    unsigned char   _numsats, _new_numsats; // number of satellites

    unsigned char   _fixquality, _new_fixquality;
    unsigned char   _fixtype, _new_fixtype;

    unsigned long   _last_time_fix, _new_time_fix;
    unsigned long   _last_position_fix, _new_position_fix;
};

#endif
