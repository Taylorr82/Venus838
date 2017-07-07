# NMEA messages

NMEA messages supply information about the GPS receiver's current location, bearing, speed, satellite count, precision, etc.

NMEA messages begin with the '$' character followed by the message title/id (GPGGA, GPGLL, etc.). The message ends with a crlf preceeded by a checksum (two characters displaying the hex representation of the 8-bit XOR of all characters between (exclusive) the '$' and '\*' characters)

#### GGA - Fix information

```
$GPGGA,hhmmss.s,ddmm.m,i,dddmm.m,i,q,nn,h.h,mmm.m,M,m.m,M,,xxxx*cc

Where:
    hhmmss.s    UTC Time of Fix
    ddmm.m,i    Latitude of dd deg mm.mmm' i (N/S)
    dddmm.m,i   Longitude of ddd deg mm.mmm' i (E/W)
    q           Fix quality: 0 = invalid
                             1 = GPS fix (SPS)
                             2 = DGPS fix
                             3 = PPS fix
    	                     4 = Real Time Kinematic
    	                     5 = Float RTK
                             6 = estimated (dead reckoning) (2.3 feature)
	                         7 = Manual input mode
	                         8 = Simulation mode
    nn          Number of satellites being tracked
    h.h         Horizontal dilution of position
    m.m,M       Altitude, Meters, above mean sea level
    m.m,M       Height of geoid (mean sea level) above WGS84 ellipsoid
    xxxx        DGPS Station ID Number
    *cc         Checksum
```

#### GLL - Latitude/Longitude

```
$GPGLL,ddmm.m,i,dddmm.m,i,hhmmss.s,s,i*cc

Where:
    ddmm.m,i    Latitude of dd deg mm.m' i (N/S)
    dddmm.m,i   Longitude of ddd deg mm.m' (E/W)
    hhmmss.s    UTC Time of Fix
    s           Status: 'A' = valid, 'V' = invalid
    i           Mode Indicator: 'N' = data invalid
                                'A' = autonomous mode
                                'D' = differential mode
                                'E' = dead reckoning mode
                                'M' = manual input mode
                                'S' = simulator mode
    *cc         Checksum
```

#### GSA - DOP and Active Satellites

```
$GPGSA,m,f,sv,sv,sv,sv,sv,sv,sv,sv,sv,sv,sv,sv,p.p,h.h,v.v*cc

Where:
    m        Mode: 'M' = Manual, 'A' = Automatic
    f        Fix Type:  1 = no fix
                        2 = 2D fix
                        3 = 3D fix
    sv,sv... PRNs of satellites used for fix (space for 12)
    p.p      PDOP (dilution of precision)
    h.h      Horizontal dilution of precision (HDOP)
    v.v      Vertical dilution of precision (VDOP)
    *cc      Checksum
```

#### GSV - Satellites in View

```
$GPGSV,n,s,ss,id,dd,ddd,xx,id,dd,ddd,xx,id,dd,ddd,xx,id,dd,ddd,xx*75

Where:
    n            Number of sentences for full data
    s            Sentence number
    ss           Number of satellites in view

    id           Satellite PRN number
    dd           Elevation, degrees
    ddd          Azimuth, degrees
    xx           SNR - higher is better
    *cc          Checksum
```

#### RMC - Recommended Minimum

```
$GPRMC,hhmmss.s,s,ddmm.m,i,dddmm.m,i,sss.s,ccc.c,ddmmyy,,,m*cc

Where:
    hhmmss.s    UTC Time of Fix
    s           Status 'A' = valid or 'V' = invalid.
    ddmm.m,i    Latitude of dd deg mm.m' i (N/S)
    dddmm.m,i   Longitude ddd deg mm.m' i (E/W)
    sss.s       Speed over the ground in knots
    ccc.c       Course over the ground in degrees
    ddmmyy      UTC Date
    m           Mode Indicator: 'N' = data invalid
                                'A' = autonomous mode
                                'D' = differential mode
                                'E' = dead reckoning mode
                                'M' = manual input mode
                                'S' = simulator mode
    *cc         Checksum
```

#### VTG - Course and Speed

```
$GPVTG,ccc.c,T,,M,sss.s,N,sss.s,K,m*cc

Where:
    ccc.c   True track made good (degrees)
    sss.s   Ground speed, knots
    sss.s   Ground speed, Kilometers per hour
    m       Mode Indicator: 'N' = data invalid
                            'A' = autonomous mode
                            'D' = differential mode
                            'E' = dead reckoning mode
                            'M' = manual input mode
                            'S' = simulator mode
    *cc     Checksum
```

#### ZDA - Date and Time

```
$GPZDA,hhmmss.ss,dd,mm,yyyy,hh,mm*cc

Where:
    hhmmss  UTC Time
    dd      Day
    mm      Month
    yyyy    Year
    hh      Local zone hours -13..13
    mm      Local zone minutes 0..59
    *cc     Checksum
```
