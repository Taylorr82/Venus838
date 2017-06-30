// Reed Foster
// 28 Jun 2017

#include <TinyGPS.h>

#define GPSSerial Serial1
#define USBSerial Serial

//nmeaconfig           start seq   payloadlen   id   gga   gsa   gsv   gll   rmc   vtg   zda   attr  csum   stop seq
char nmeaconfig[16] = {0xa0, 0xa1, 0x00, 0x09, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x0d, 0x0a};
//hzconfig           start seq   payloadlen   id   rate  attr  csum   stop seq
char hzconfig[10] = {0xa0, 0xa1, 0x00, 0x03, 0x0e, 0x14, 0x00, 0x1a, 0x0d, 0x0a};
//baudconfig           start seq   payloadlen   id   port  rate  attr  csum   stop seq
char baudconfig[11] = {0xa0, 0xa1, 0x00, 0x04, 0x05, 0x00, 0x05, 0x00, 0x00, 0x0d, 0x0a};
//reset
char reset[9] = {0xa0, 0xa1, 0x00, 0x02, 0x04, 0x01, 0x05, 0x0d, 0x0a};

TinyGPS gps;

void setup()
{
    GPSSerial.begin(9600);
    USBSerial.begin(115200);

    GPSSerial.write(reset, sizeof(reset));
    delay(5000);

    GPSSerial.write(baudconfig, sizeof(baudconfig));
    GPSSerial.end();
    GPSSerial.begin(115200);

    delay(100);
    GPSSerial.write(hzconfig, sizeof(hzconfig));
    delay(100);
    GPSSerial.write(nmeaconfig, sizeof(nmeaconfig));
}

void loop()
{
    char c;
    while(GPSSerial.available())
    {
        c = GPSSerial.read();
        USBSerial.write(c);
    }
}

void debug(char* command)
{
    char c, last;
    last = 0x00;
    bool msg = false;
    GPSSerial.write(command, sizeof(command));
    for(long start = millis(); millis() - start < 1000;)
    {
        while (GPSSerial.available())
        {
            c = GPSSerial.read();
            if (last == 0xA0 and c == 0xA1 and msg == false)
            {
                msg = true;
                USBSerial.print(last, 16);
                USBSerial.print(" ");
            }
            else if (msg == true and last == 0x0D and c == 0x0A)
            {
                msg = false;
                USBSerial.print(c, 16);
                USBSerial.print(" ");
            }
            if (msg)
            {
                USBSerial.print(c, 16);
                USBSerial.print(" ");
            }
            last = c;
        }
    }
}
