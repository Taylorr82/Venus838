/*
    NMEAParser.cpp - Library for parsing NMEA strings from a GPS (slight modification of TinyGPS)
    Decodes GGA, RMC, GSA, and GSV strings
    Reed A. Foster, July 2017.
*/

#include "NMEAParser.hpp"

NMEAParser::NMEAParser() :
    _term_offset(0),
    _term_number(0),
    _is_checksum_term(false),
    _sentence_type(NMEA_UNKNOWN),
    _parity(0),
    _gps_data_good(false),
    _time(GPS_INVALID_TIME),
    _date(GPS_INVALID_DATE),
    _latitude(GPS_INVALID_ANGLE),
    _longitude(GPS_INVALID_ANGLE),
    _altitude(GPS_INVALID_ALTITUDE),
    _speed(GPS_INVALID_SPEED),
    _course(GPS_INVALID_ANGLE),
    _hdop(GPS_INVALID_DOP),
    _vdop(GPS_INVALID_DOP),
    _pdop(GPS_INVALID_DOP),
    _numsats(GPS_INVALID_SATELLITES),
    _numsats_visible(GPS_INVALID_SATELLITES),
    _gsv_sentence(0),
    _gsv_sentences(0),
    _snr_total(0),
    _fixquality(0),
    _fixtype(0),
    _last_time_fix(GPS_INVALID_FIX_TIME),
    _last_position_fix(GPS_INVALID_FIX_TIME)
{
    _term[0] = 0;
}

bool NMEAParser::encode(char c)
{
    switch (c)
    {
        case ',':
        case '*':
        case '\n':
            _is_checksum_term = c == '*';
            _term[_term_offset] = 0;
            _logTerm();
            _term_number++;
            _term_offset = 0;
            if (_term[0] == '*')
            {
                _is_checksum_term = true;
                int checksum = _hexToInt(_term[1]) * 16 + _hexToInt(_term[2]);
                if (checksum == _parity)
                {
                    if ((_sentence_type == NMEA_RMC && _gps_data_good) ||
                        (_sentence_type == NMEA_GGA && _new_fixquality > 0) || // fix quality > invalid
                        (_sentence_type == NMEA_GSA && _new_fixtype > 1) || // fix type is 2D or 3D
                        (_sentence_type == NMEA_GSV))
                    {
                        if (_sentence_type == NMEA_GGA || _sentence_type == NMEA_RMC)
                        {
                            _last_time_fix = _new_time_fix;
                            _last_position_fix = _new_position_fix;
                        }
                        switch (_sentence_type)
                        {
                            case NMEA_GGA:
                                _time = _new_time;
                                _latitude = _new_latitude;
                                _longitude = _new_longitude;
                                _fixquality = _new_fixquality;
                                _numsats = _new_numsats;
                                _hdop = _new_hdop;
                                _altitude = _new_altitude;
                                break;

                            case NMEA_GSA:
                                _fixtype = _new_fixtype;
                                _pdop = _new_pdop;
                                _hdop = _new_hdop;
                                _vdop = _new_vdop;
                                break;

                            case NMEA_RMC:
                                _time = _new_time;
                                _latitude = _new_latitude;
                                _longitude = _new_longitude;
                                _speed = _new_speed;
                                _course = _new_course;
                                _date = _new_date;
                                break;

                            case NMEA_GSV:
                                if (_gsv_sentence == _gsv_sentences)
                                {
                                    _numsats_visible = _new_numsats_visible;
                                    _snr_avg = _snr_total * 100L / _numsats_visible;

                                    _new_numsats_visible = 0;
                                    _snr_total = 0;
                                }
                                else
                                {
                                    return false;
                                }
                                break;

                        }
                        return true;
                    }
                    return false;
                }
            }
            break;
        case '$':
            _term_number = 0;
            _parity = 0;
            _is_checksum_term = false;
            _gps_data_good = false;
            _sentence_type = NMEA_UNKNOWN;
            break;
    }

    if (c != ',')
        _term[_term_offset++] = c;
    if (!_is_checksum_term && c != '$')
        _parity ^= c;
    return false;
}

int NMEAParser::_hexToInt(char c)
{
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else
        return c - '0';
}

int NMEAParser::_strcmp(const char *str1, const char *str2)
{
    while (*str1 && *str1 == *str2)
        str1++, str2++;
    return (*str1 > *str2) - (*str2 > *str1);
}

long NMEAParser::_parse_decimal()
{
    char *p = _term;
    bool neg = *p == '-';
    if (neg) p++;
    long ret = 0L;
    while (*p >= '0' && *p <= '9')
        ret = ret * 10L + *p++ - '0';
    ret *= 100L;
    if (*p++ == '.')
    {
        if (*p >= '0' && *p <= '9')
        {
            ret += 10L * (*p++ - '0');
            if (*p >= '0' && *p <= '9')
                ret += *p - '0';
        }
    }
    return neg ? -ret : ret;
}

long NMEAParser::_parse_degrees()
{
    char *p = _term;
    long deg = 0L;
    while ((*(p + 2) != '.') && (*p >= '0' && *p <= '9'))
        deg = deg * 10L + *p++ - '0';
    deg *= 100000L;
    long min = (*p++ - '0') * 10;
    min += *p++ - '0';
    p++;
    while ((*p >= '0' && *p <= '9'))
        min = min * 10L + *p++ - '0';
    deg += min / 6;
    return deg;
}

long NMEAParser::_decStringToInt(char *c)
{
    long i = 0;
    while (*c)
        i = i * 10 + *c++ - '0';
    return i;
}

#define GPS_COMBINE(type, number) (((unsigned)(type) << 6) | number)

void NMEAParser::_logTerm()
{
    if (_term_number == 0)
    {
        if (!_strcmp(_term, _GPGGA_TERM))
            _sentence_type = NMEA_GGA;
        else if (!_strcmp(_term, _GPGSA_TERM))
            _sentence_type = NMEA_GSA;
        else if (!_strcmp(_term, _GPRMC_TERM))
            _sentence_type = NMEA_RMC;
        else if (!_strcmp(_term, _GPGSV_TERM))
            _sentence_type = NMEA_GSV;
        else
            _sentence_type = NMEA_UNKNOWN;
        return;
    }
    if (_sentence_type != NMEA_UNKNOWN);
    {
        switch (GPS_COMBINE(_sentence_type, _term_number))
        {
            case GPS_COMBINE(NMEA_GGA, 1): // UTC Time
            case GPS_COMBINE(NMEA_RMC, 1): // UTC Time
                _new_time_fix = millis();
                _new_time = _parse_decimal();
                break;

            case GPS_COMBINE(NMEA_GGA, 2): // Latitude
            case GPS_COMBINE(NMEA_RMC, 3): // Latitude
                _new_position_fix = millis();
                _new_latitude = _parse_degrees();
                break;

            case GPS_COMBINE(NMEA_GGA, 3): // N/S Indicator
            case GPS_COMBINE(NMEA_RMC, 4): // N/S Indicator
                _new_latitude *= _term[0] == 'S' ? -1 : 1;
                break;

            case GPS_COMBINE(NMEA_GGA, 4): // Longitude
            case GPS_COMBINE(NMEA_RMC, 5): // Longitude
                _new_longitude = _parse_degrees();
                break;

            case GPS_COMBINE(NMEA_GGA, 5): // E/W Indicator
            case GPS_COMBINE(NMEA_RMC, 6): // E/W Indicator
                _new_longitude *= _term[0] == 'W' ? -1 : 1;
                break;

            case GPS_COMBINE(NMEA_GGA, 6): // Fix Quality
                _new_fixquality = _hexToInt(_term[0]);
                break;

            case GPS_COMBINE(NMEA_GGA, 7): // Number of Satellites (fix only)
                _new_numsats = _decStringToInt(_term);
                break;

            case GPS_COMBINE(NMEA_GGA, 8): // HDOP
            case GPS_COMBINE(NMEA_GSA, 16): // HDOP
                _new_hdop = (short) _parse_decimal();
                break;

            case GPS_COMBINE(NMEA_GGA, 9): // Altitude
                _new_altitude = _parse_decimal();
                break;

            case GPS_COMBINE(NMEA_GSA, 2):  // Fix Type
                _new_fixtype = _term[0] - '0';
                break;

            case GPS_COMBINE(NMEA_GSA, 15): // PDOP
                _new_pdop = (short) _parse_decimal();
                break;

            case GPS_COMBINE(NMEA_GSA, 17): // VDOP
                _new_vdop = (short) _parse_decimal();
                break;

            case GPS_COMBINE(NMEA_RMC, 7): // Speed
                _new_speed = _parse_decimal();
                break;

            case GPS_COMBINE(NMEA_RMC, 8): // Course
                _new_speed = _parse_decimal();
                break;

            case GPS_COMBINE(NMEA_RMC, 9): // UTC Date
                _new_date = _parse_decimal();
                break;

            case GPS_COMBINE(NMEA_GSV, 1): // Number of GSV sentences (usually 3)
                _gsv_sentences = _decStringToInt(_term);
                break;

            case GPS_COMBINE(NMEA_GSV, 2): // GSV sentence number
                _gsv_sentence = _decStringToInt(_term);
                break;

            case GPS_COMBINE(NMEA_GSV, 3): // Number of Visible Satellites
                _new_numsats_visible = _decStringToInt(_term);
                break;

            case GPS_COMBINE(NMEA_GSV, 7):
            case GPS_COMBINE(NMEA_GSV, 11):
            case GPS_COMBINE(NMEA_GSV, 15):
            case GPS_COMBINE(NMEA_GSV, 19):
                _snr_total += _decStringToInt(_term);
                break;
        }
    }
}
