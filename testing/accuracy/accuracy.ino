// Reed Foster
// 20 Jul 2017

#include <NMEAParser.hpp>
#include <Venus838.hpp>

NMEAParser parser;
Venus838 gps(Serial1, 115200, false);

long lastutc = 0;

void setup()
{
    gps.setBaudRate(115200, 0);
    gps.cfgNMEA(0b0000000, 0); // disable all NMEA sentences
    gps.cfgNMEA(NMEA_GGA, true, 0); // enable GGA, GSA, GSV, and RMC sentences
    gps.cfgNMEA(NMEA_GSA, true, 0); // these are the only sentences supported
    gps.cfgNMEA(NMEA_GSV, true, 0); // by the NMEAParser library
    gps.cfgNMEA(NMEA_RMC, true, 0);
    gps.setUpdateRate(20, 0);
    Serial.begin(115200);
}

void loop()
{

    bool newdata = false;
    char c;
    while (gps.available())
    {
        c = gps.read();
        newdata = parser.encode(c);
    }
    if (newdata)
    {
        
        long utc = parser.getTime();
        long latitude_u, latitude_l;
        parser.getLatitude(&latitude_u, &latitude_l);
        long longitude_u, longitude_l;
        parser.getLongitude(&longitude_u, &longitude_l);
        long altitude = parser.getAltitude();
        short pdop = parser.getPDOP();
        short hdop = parser.getHDOP();
        short vdop = parser.getVDOP();
        long snravg = parser.getSNR();

        // only print gps data if both a GSV, GGA/RMC, and GSA fix have been received
        if (utc > lastutc && utc != NMEAParser::GPS_INVALID_TIME && snravg != NMEAParser::GPS_INVALID_SNR && pdop != NMEAParser::GPS_INVALID_DOP)
        {
            Serial.printf("%d.%07d, %d.%07d, %d.%d,   %d.%d, %d.%d, %d.%d,   %d.%d\n", latitude_u, latitude_l, longitude_u, longitude_l, (long)(altitude / 100), (altitude < 0 ? -1 : 1) * altitude % 100, (long)(pdop / 100), pdop % 100, (long)(hdop / 100), hdop % 100, (long)(vdop / 100), vdop % 100,  (long)(snravg / 100), snravg % 100);
            lastutc = utc;
        }
    }
}
