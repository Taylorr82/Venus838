# Venus838 Library

The Venus838 Library provides an interface for configuring the SkyTraq Venus838 GPS.

## Configuration Methods

Configuration methods allow the user to modify the operation of the GPS receiver.
Each method returns a `char` which represents an error/response code:<br>
`GPS_NORMAL` - configuration was successful (ACK received).<br>
`GPS_NACK` - configuration was unsuccessful (NACK recieved).<br>
`GPS_TIMEOUT` - configuration was unsuccessful (no response at all).<br>
`GPS_INVALIDARG` - an invalid argument was provided to the cfg function.<br>
`GPS_UNKNOWN` - ACK or NACK received, but message id of (N)ACK does not match message id of cfg message.

#### char setBaudRate(int baudrate, char attribute);

Set the baud rate of the GPS receiver's serial interface.<br>
`baudrate` must be one of the following:
4800, 9600, 19200, 38400, 57600, 115200.<br>
Returns `GPS_INVALIDARG` if `baudrate` is not valid.<br>
`attribute` 0 = update RAM only, 1 = update both RAM and flash.

#### char setUpdateRate(int frequency, char attribute);

Set the position update rate of the GPS receiver (how frequently it sends out NMEA sentences).<br>
`frequency` determines the update rate (1Hz, 2Hz, 4Hz, 5Hz, 8Hz, 10Hz, 20Hz). Higher frequencies require higher baud rates; 4-10Hz requires 38400 baud and 20Hz requires 115200 baud<br>
`attribute` 0 = update RAM only, 1 = update both RAM and flash.

#### char resetReceiver(bool reboot);

Reset the configuration settings to factory defaults (change only takes effect after a reboot).<br>
`reboot` true to reboot receiver.

#### char querySoftwareVersion();

Query the current software version of the GPS receiver.<br>
Used for autodetection of the baud rate.<br>

#### char cfgNMEA(char messagename, bool enable, char attribute);

Enable/disable a single NMEA sentence.<br>
`messagename` id of the NMEA sentence to configure (`NMEA_GGA`, `NMEA_GSA`, `NMEA_GSV`, etc.).<br>
`enable` true will enable the NMEA sentence specified by `messagename`, false will disable it.<br>
`attribute` 0 = update RAM only, 1 = update both RAM and flash.

#### char cfgNMEA(char nmeabyte, char attribute);

Enable/disable all NMEA sentences with a single byte.<br>
`nmeabyte` each bit corresponds to a different NMEA sentence; set a bit to enable the sentence, clear it to disable the sentence.<br>
The corresponding bits for each NMEA sentence are as follows (from lsb to msb, bit 7 is unused): GGA, GSA, GSV, GLL, RMC, VTG, ZDA.<br>
`attribute` 0 = update RAM only, 1 = update both RAM and flash.

#### char cfgPowerSave(bool enable, char attribute);

Enable/disable receiver's power-save mode.<br>
`enable` true will enable power-saving mode.<br>
`attribute` 0 = update RAM only, 1 = update both RAM and flash, 2 = temporarily enabled.

#### char cfgPPS(char mode, char attribute);

Configure the function of the 1PPS (pulse-per-second) pin.<br>
`mode` 0 = off, 1 = only on when 3D fix, 2 = on when at least 1 SV.<br>
`attribute` 0 = update RAM only, 1 = update both RAM and flash.


## Wrapper Methods

#### bool available();

Wrapper method for private `_gpsSerial` object.<br>
Returns `_gpsSerial.available();`

#### char read();

Wrapper method for private `_gpsSerial` object.<br>
Returns `_gpsSerial.read();`


## Private Methods

#### int \_getBaudRate();

Auto-detects the current baud rate of the GPS receiver by querying its software version and waiting for an ACK with various different baud rates.
Returns the integer baud rate of the GPS receiver.

#### char \_sendCommand(char messageid, char \*messagebody, int bodylen);

Send command with the default timeout.<br>
See `_sendCommand(char messageid, char *messagebody, int bodylen, uint timemout)` for more information on the `_sendCommand` method.

#### char \_sendCommand(char messageid, char \*messagebody, int bodylen, uint timemout);

Assembles a command packet, sends it (using `_sendPacket`), returns result from `_sendPacket`.<br>

#### char \_sendPacket(char \*packet, int size, uint timeout);

Writes packet to `_gpsSerial` and waits for a response or timeout.

#### void \_printPacket(char \*packet, int size);

Debug method, uses `Serial` to print the provided packet (in hex).

#### void \_debug(const char \*message);

Debug method, wrapper for `Serial.print`.

#### void \_debug(int number);

Debug method, wrapper for `Serial.println`.

## Extending Venus838

Additional information on the configuration of the GPS receiver can be found [here](doc/gpscommands.md)(md) and [here](https://cdn.sparkfun.com/datasheets/Sensors/GPS/Venus/638/doc/AN0003_v1.4.19.pdf)(pdf)

### Adding Configuration Methods

Each configuration method follows a similar structure, it prints a statement (using `_debug`) that indicates it has been called.<br>
Next, an array is allocated for storing the body of the message.<br>
`messagebody` is then filled with command data specific to the command.<br>
Finally, the command is actually sent using `_sendCommand`.
```c++
char messagebody[sizeofmessage];
memset(messagebody, 0, sizeofmessage);
// initialize messagebody
return _sendCommand(messageid, messagebody, sizeofmessage);
```
