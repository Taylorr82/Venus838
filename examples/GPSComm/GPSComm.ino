// Reed Foster
// 28 Jun 2017

//#include <TinyGPS.h>
#include <Venus838.hpp>

//TinyGPS gps;
Venus838 gps(Serial1, 115200, true);

String errorcodes[5] = {"NORMAL", "NACK", "TIMEOUT", "INVALIDARG", "UNKNOWN"};

void setup()
{
    while (!gps.available());
    Serial.println(errorcodes[(int)gps.resetReceiver(true)]);
    //delay(1000);
    while (!gps.available());
    Serial.println(errorcodes[(int)gps.setBaudRate(19200, 1)]);
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
