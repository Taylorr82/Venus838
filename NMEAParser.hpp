/*
    NMEAParser.hpp - Library for parsing NMEA strings from a GPS
    Reed A. Foster, July 2017.
*/

#define NMEATERM_MAXLENGTH 15

class NMEAParser
{
public:
    NMEAParser();

    bool encode(char c);

private:

    int _hexToInt(char c);

    char _term[NMEASENTENCE_MAXLENGTH];
    int _termidx;

    const char _GPGGA_TERM[] = "GPGGA";
    const char _GPGLL_TERM[] = "GPGLL";
    const char _GPGSA_TERM[] = "GPGSA";
    const char _GPGSV_TERM[] = "GPGSV";
    const char _GPRMC_TERM[] = "GPRMC";
    const char _GPVTG_TERM[] = "GPVTG";
    const char _GPZDA_TERM[] = "GPZDA";

    unsigned long _time, _date
}
