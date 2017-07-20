// Reed Foster
// 20 Jul 2017

#include <NMEAParser.hpp>
#include <Venus838.hpp>

NMEAParser parser;
Venus838 gps(Serial1, 19200, false);

String errorcodes[5] = {"NORMAL", "NACK", "TIMEOUT", "INVALIDARG", "UNKNOWN"};

long lastutc = 0; // used for data validity verification

void setup()
{
    Serial.begin(115200);

    Serial.println("Setting baud rate to 115200 baud");
    printResult(gps.setBaudRate(115200, 0));

    Serial.println("Setting position update rate to max value (20Hz)");
    printResult(gps.setUpdateRate(20, 0));

    Serial.println("Configuring which NMEA sentences the GPS provides");
    printResult(gps.cfgNMEA(0b0000000, 0));      // disable all strings
    printResult(gps.cfgNMEA(NMEA_GGA, true, 0)); // enable GGA, GSA, GSV, and RMC
    printResult(gps.cfgNMEA(NMEA_GSA, true, 0)); // (these are the only sentences that
    printResult(gps.cfgNMEA(NMEA_GSV, true, 0)); // NMEAParser decodes)
    printResult(gps.cfgNMEA(NMEA_RMC, true, 0));
}

void loop()
{

    bool newdata = false;
    char c;
    while (gps.available())
    {
        c = gps.read();
        newdata = parser.encode(c); // pass character to parser, which reads characters until a newline, then decodes a full line/sentence
    }
    if (newdata)
    {
        // Parsing latitude and longitude
        long latitude_u, latitude_l; // each variable is separated into two longs to improve precision
        long longitude_u, longitude_l;
        parser.getLatitude(&latitude_u, &latitude_l); // pass reference to latitude_u and latitude_l to getLatitude()
        parser.getLongitude(&longitude_u, &longitude_l);

        long utc = parser.getTime(); // getTime() returns UTC time in hundredths of a second

        long altitude = parser.getAltitude(); // altitude in cm
        short pdop = parser.getPDOP();
        short hdop = parser.getHDOP();
        short vdop = parser.getVDOP();
        long snravg = parser.getSNR();

        // additional variables for printing fixed point in decimal format
        long altitude_u = altitude / 100;
        long altitude_l = (altitude < 0 ? -1 : 1) * altitude % 100;
        short pdop_u = pdop / 100;
        short pdop_l = pdop % 100;
        short hdop_u = hdop / 100;
        short hdop_l = hdop % 100;
        short vdop_u = vdop / 100;
        short vdop_l = vdop % 100;
        long snr_u = snravg / 100;
        long snr_l = snravg % 100;

        if (utc > lastutc && utc != NMEAParser::GPS_INVALID_TIME && snravg != NMEAParser::GPS_INVALID_SNR && pdop != NMEAParser::GPS_INVALID_DOP)
        {
            Serial.printf("(%d.%07d, %d.%07d), %d.%d, %d.%d, %d.%d, %d.%d, %d.%d\n", latitude_u, latitude_l, longitude_u, longitude_l, altitude_u, altitude_l, pdop_u, pdop_l, hdop_u, hdop_l, vdop_u, vdop_l, snr_u, snr_l);
            lastutc = utc;
        }
    }
}

void printResult(char result)
{
    Serial.print("Response code: ");
    Serial.println(errorcodes[(int)result]);
}
