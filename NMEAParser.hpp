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

    enum {
        GPS_INVALID_DOP = 0xFFFFFFFF,       GPS_INVALID_ANGLE = 999999999,
        GPS_INVALID_ALTITUDE = 999999999,   GPS_INVALID_DATE = 0,
        GPS_INVALID_TIME = 0xFFFFFFFF,      GPS_INVALID_SPEED = 999999999,
        GPS_INVALID_FIX_TIME = 0xFFFFFFFF,  GPS_INVALID_SATELLITES = 0xFF,
        GPS_INVALID_AGE = 0xFFFFFFFF
    };

    NMEAParser();

    // returns true if a NMEA sentence has successfully been decoded
    bool encode(char c);

    inline unsigned long getTime() {return _time;}
    inline unsigned short getDate() {return _date;}
    inline long getLatitude() {return _latitude;}
    inline long getLongitude() {return _longitude;}
    inline long getAltitude() {return _altitude;}
    inline unsigned short getPDOP() {return _pdop;}
    inline unsigned short getVDOP() {return _vdop;}
    inline unsigned short getHDOP() {return _hdop;}
    inline unsigned char getNSats() {return _numsats;}
    inline unsigned char getFixQuality() {return _fixquality;}
    inline unsigned char getFixType() {return _fixtype;}
    inline unsigned long getSpeed() {return _speed;}
    inline unsigned short getCourse() {return _course;}
    inline unsigned long timeAge() {return _last_time_fix;}
    inline unsigned long positionAge() {return _last_position_fix;}
    inline unsigned char getNSatsVisible() {return _numsats_visible;}
    inline unsigned long getSNR() {return _snr_avg;}

private:

    int _hexToInt(char c);
    long _decStringToInt(char *c);
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
    bool _gps_data_good;

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
    unsigned char   _numsats, _new_numsats; // number of satellites used for fix
    unsigned char   _numsats_visible, _new_numsats_visible; // number of satellites visible to gps
    unsigned char   _gsv_sentence, _gsv_sentences; // counter and total for gsv messages
    unsigned long   _snr_total; // sum of snr from all satellites
    unsigned long   _snr_avg; // average snr of gsv message (over all sentences) scaled by 100 (i.e. 4500 corresponds to an average SNR of 45)

    unsigned char   _fixquality, _new_fixquality;
    unsigned char   _fixtype, _new_fixtype;

    unsigned long   _last_time_fix, _new_time_fix;
    unsigned long   _last_position_fix, _new_position_fix;
};

#endif
