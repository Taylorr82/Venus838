# Venus838 Library

The Venus838 Library provides an interface for configuring the SkyTraq Venus838 GPS.

## Configuration Methods

Configuration methods allow the user to modify the operation of the GPS receiver.
Each method returns a <code>char</code> which represents an error/response code:<br>
<code>GPS_NORMAL</code> - configuration was successful (ACK received).<br>
<code>GPS_NACK</code> - configuration was unsuccessful (NACK recieved).<br>
<code>GPS_TIMEOUT</code> - configuration was unsuccessful (no response at all).<br>
<code>GPS_INVALIDARG</code> - an invalid argument was provided to the cfg function.<br>
<code>GPS_UNKNOWN</code> - ACK or NACK received, but message id of (N)ACK does not match message id of cfg message.

#### char setBaudRate(int baudrate, char attribute);

Set the baud rate of the GPS receiver's serial interface.<br>
<code>baudrate</code> must be one of the following:
4800, 9600, 19200, 38400, 57600, 115200.<br>
Returns <code>GPS_INVALIDARG</code> if <code>baudrate</code> is not valid.<br>
<code>attribute</code> 0 = update RAM only, 1 = update both RAM and flash.

#### char setUpdateRate(int frequency, char attribute);

Set the position update rate of the GPS receiver (how frequently it sends out NMEA sentences).<br>
<code>frequency</code> determines the update rate.<br>
<code>attribute</code> 0 = update RAM only, 1 = update both RAM and flash.

#### char resetReceiver(bool reboot);

Reset the configuration settings to factory defaults (change only takes effect after a reboot).<br>
<code>reboot</code> true to reboot receiver.

#### char querySoftwareVersion();

Query the current software version of the GPS receiver.<br>
Used for autodetection of the baud rate.<br>

#### char cfgNMEA(char messagename, bool enable, char attribute);

Enable/disable a single NMEA sentence.<br>
<code>messagename</code> id of the NMEA sentence to configure (<code>NMEA_GGA</code>, <code>NMEA_GSA</code>, <code>NMEA_GSV</code>, etc.).<br>
<code>enable</code> true will enable the NMEA sentence specified by <code>messagename</code>, false will disable it.<br>
<code>attribute</code> 0 = update RAM only, 1 = update both RAM and flash.

#### char cfgNMEA(char nmeabyte, char attribute);

Enable/disable all NMEA sentences with a single byte.<br>
<code>nmeabyte</code> each bit corresponds to a different NMEA sentence; set a bit to enable the sentence, clear it to disable the sentence.<br>
The corresponding bits for each NMEA sentence are as follows (from lsb to msb, bit 7 is unused): GGA, GSA, GSV, GLL, RMC, VTG, ZDA.<br>
<code>attribute</code> 0 = update RAM only, 1 = update both RAM and flash.

#### char cfgPowerSave(bool enable, char attribute);

Enable/disable receiver's power-save mode.<br>
<code>enable</code> true will enable power-saving mode.<br>
<code>attribute</code> 0 = update RAM only, 1 = update both RAM and flash, 2 = temporarily enabled.

#### char cfgPPS(char mode, char attribute);

Configure the function of the 1PPS (pulse-per-second) pin.<br>
<code>mode</code> 0 = off, 1 = only on when 3D fix, 2 = on when at least 1 SV.<br>
<code>attribute</code> 0 = update RAM only, 1 = update both RAM and flash.


## Wrapper Methods

#### bool available();

Wrapper method for private \_serial object.<br>
Returns <code>\_serial.available();</code>

#### char read();

Wrapper method for private \_serial object.<br>
Returns <code>\_serial.read();</code>


## Private Methods

#### int \_getBaudRate();
#### char \_sendCommand(char messageid, char \*messagebody, int bodylen);
#### char \_sendCommand(char messageid, char \*messagebody, int bodylen, uint timemout);
#### char \_sendPacket(char \*packet, int size, uint timeout);   
#### void \_printPacket(char \*packet, int size);
#### void \_debug(const char \*message);
#### void \_debug(int number);
