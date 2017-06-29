// Reed Foster
// 28 Jun 2017

#include <TinyGPS.h>

#define GPSSerial Serial1
#define USBSerial Serial

#define refreshperiod_s 0.5

//nmeaconfig           start seq   payloadlen   id   gga   gsa   gsv   gll   rmc   vtg   zda   attr  csum   stop seq
char nmeaconfig[16] = {0xa0, 0xa1, 0x00, 0x09, 0x08, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x09, 0x0d, 0x0a};
//hzconfig           start seq   payloadlen   id   rate  attr  csum   stop seq
char hzconfig[10] = {0xa0, 0xa1, 0x00, 0x03, 0x0e, 0x14, 0x00, 0x1a, 0x0d, 0x0a};
//baudconfig           start seq   payloadlen   id   port  rate  attr  csum   stop seq
char baudconfig[11] = {0x0a, 0xa1, 0x00, 0x04, 0x05, 0x00, 0x01, 0x00, 0x04, 0x0d, 0x0a};

//reset
char reset[9] = {0xa0, 0xa1, 0x00, 0x02, 0x04, 0x01, 0x05, 0x0d, 0x0a};

TinyGPS gps;

void setup()
{
    GPSSerial.begin(9600);
    USBSerial.begin(115200);

    delay(20);

    for (int i = 0; i < sizeof(reset); i++)
    {
        GPSSerial.write(reset[i]);
    }

    for (int i = 0; i < sizeof(nmeaconfig); i++)
    {
        //GPSSerial.write(nmeaconfig[i]);
    }
    for (int i = 0; i < sizeof(hzconfig); i++)
    {
        //GPSSerial.write(hzconfig[i]);
    }

    for (int i = 0; i < sizeof(baudconfig); i++)
    {
        //GPSSerial.write(baudconfig[i]);
    }

    GPSSerial.flush();
    delay(2);
    GPSSerial.begin(9600);
}

void loop()
{
    char c;
    while (GPSSerial.available())
    {
        c = GPSSerial.read();
        USBSerial.write(c);
    }
}
