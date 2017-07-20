// Reed Foster
// 28 Jun 2017

#include <NMEAParser.hpp>
#include <Venus838.hpp>

NMEAParser parser;
Venus838 gps(Serial1, 115200, false);

String errorcodes[5] = {"NORMAL", "NACK", "TIMEOUT", "INVALIDARG", "UNKNOWN"};

void setup()
{
    //while (!gps.available());
    //Serial.println(errorcodes[(int)gps.resetReceiver(true)]);
    //delay(1000);
    //while (!gps.available());
    //Serial.println(errorcodes[(int)gps.setBaudRate(19200, 1)]);
    //Serial.println("exiting setup");
    //Serial1.begin(9600);
    gps.setBaudRate(115200, 0);
    gps.cfgNMEA(0b0000000, 0);
    gps.cfgNMEA(NMEA_GGA, true, 0);
    gps.cfgNMEA(NMEA_GSA, true, 0);
    gps.cfgNMEA(NMEA_GSV, true, 0);
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
        //Serial.write(c);
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
        if (utc > lastutc && utc != NMEAParser::GPS_INVALID_TIME && snravg != NMEAParser::GPS_INVALID_SNR)
        {
            Serial.printf("%d.%07d, %d.%07d, %d.%d,   %d.%d, %d.%d, %d.%d,   %d.%d\n", latitude_u, latitude_l, longitude_u, longitude_l, (long)(altitude / 100), (altitude < 0 ? -1 : 1) * altitude % 100, (long)(pdop / 100), pdop % 100, (long)(hdop / 100), hdop % 100, (long)(vdop / 100), vdop % 100,  (long)(snravg / 100), snravg % 100);
            lastutc = utc;
        }
    }
}
