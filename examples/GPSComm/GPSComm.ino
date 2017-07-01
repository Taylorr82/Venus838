// Reed Foster
// 28 Jun 2017

//#include <TinyGPS.h>
#include <Venus838.hpp>

//TinyGPS gps;
Venus838 gps(Serial1, 115200);

String errorcodes[5] = {"NORMAL", "NACK", "TIMEOUT", "INVALIDARG", "UNKNOWN"};

void setup()
{
    Serial.begin(115200);

    while (!gps.dataAvailable());
    Serial.println(errorcodes[(int)gps.reset(true)]);
    delay(100);
    while (!gps.dataAvailable());
    Serial.println(errorcodes[(int)gps.setBaudRate(9600, 0)]);
    Serial.println("exiting setup");
    for (long start = millis(); millis() - start < 2000;)
    {
        char c;
        while (Serial1.available())
        {
            c = Serial1.read();
            Serial.write(c);
        }
    }
}

void loop()
{

}
