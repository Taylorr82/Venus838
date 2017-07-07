// Reed Foster
// 28 Jun 2017

#include <NMEAParser.hpp>
#include <Venus838.hpp>

NMEAParser parser;
//Venus838 gps(Serial1, 115200, false);

String errorcodes[5] = {"NORMAL", "NACK", "TIMEOUT", "INVALIDARG", "UNKNOWN"};

void setup()
{
    //while (!gps.available());
    //Serial.println(errorcodes[(int)gps.resetReceiver(true)]);
    //delay(1000);
    //while (!gps.available());
    //Serial.println(errorcodes[(int)gps.setBaudRate(19200, 1)]);
    //Serial.println("exiting setup");
    Serial1.begin(9600);
    Serial.begin(9600);
}

void loop()
{
    bool newdata = false;
    char c;
    while (Serial1.available())
    {
        c = Serial1.read();
        Serial.write(c);
        newdata = parser.encode(c);
    }
    if (newdata)
    {
        long latitude = parser.getLatitude();
        long longitude = parser.getLongitude();
        long altitude = parser.getAltitude();
        short pdop = parser.getPDOP();
        short hdop = parser.getHDOP();
        short vdop = parser.getVDOP();
        char numsats = parser.getNSats();
        Serial.printf("%d,%d,%d,%d,%d,%d,%d\n", latitude, longitude, altitude, pdop, hdop, vdop, numsats);
    }
}
