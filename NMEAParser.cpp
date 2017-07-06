/*
    NMEAParser.cpp - Library for parsing NMEA strings from a GPS
    Reed A. Foster, July 2017.
*/

#include "NMEAParser.hpp"

NMEAParser::NMEAParser()
{

}

bool NMEAParser::encode(char c)
{
    bool valid_sentence = false;
    switch (c)
    {
        case '$': //start character
            memset(_sentence, 0, sizeof(_sentence));
            _sentenceidx = 0;
            _sentence[_sentenceidx++] = c;
            return false;
        case '\n': //end character
            if (_sentence[_sentenceidx - 4] == '*') //checksum exists
            {
                int checksum = _hexToInt(_sentence[_sentenceidx - 3]) * 16 + _hexToInt(_sentence[_sentenceidx]);
                int parity = 0;
                for (int i = 1; i < _sentenceidx - 4; i++)
                {
                    parity ^= _sentence[i];
                }
                if (parity != checksum) //invalid checksum
                    return false;
            }
            else
                return false;
    }
    if (_sentenceidx < sizeof(sentence) - 1)
        _sentence[_sentenceidx++] = c;

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
