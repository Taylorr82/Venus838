# Venus838 and NMEAParser libraries

The Venus838 and NMEAParser libraries provide software for configuring the SkyTraq Venus838 GPS receiver and parsing NMEA strings (the standard format for receiver position and status/health data).<br>
The NMEAParser library, as its name suggests, allows for parsing of NMEA sentences; Venus838 provides a simple interface for configuring the Venus838 receiver.

## Testing

The main requirement for this project was to create a library to interface with the GPS receiver. This was achieved with the Venus838 library. An additional goal for this project was to develop a method for continuously determine instantaneous accuracy of the position data of the GPS. This is necessary for reporting dynamic accuracy "measurements" while the receiver is moving at high speeds.<br>

In order to determine an approximate model for position accuracy, a decoding library was created (NMEAParser), to allow for regression of various measurements against positional error measurements (taken while the receiver was stationary).<br>

Several studies were conducted to measure positional error in an attempt to create the accuracy model. A python script was created to read serial data from a Teensy 3.2 running both the Venus838 and NMEAParser libraries with a small arduino sketch to relay decoded telemetry data to the computer. The python script performed regression analysis on the telemetry data, reporting the equation of the least-squares regression line and the coefficient of determination (r^2).<br>

Unfortunately, the data gathered by the GPS rarely had a high coefficient of determination coupled with good (~3m) accuracy measurements and good variety of data. However, the positional accuracy can still be approximated using a constant offset of 3-5m.<br>

One observation of this study of note (which isn't too surprising) is that vertical (altitude) accuracy is relatively poor in comparison to horizontal accuracy (altitude error generally contributed a major fraction of the overall 3-D positional error).

## Using the Libraries

Documentation is provided for using each file in [Venus838.md](Venus838.md) and [NMEAParser.md](NMEAParser.md).<br>
In addition, a sample sketch detailing the usage of each library is provided.
