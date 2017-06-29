# Commanding the SkyTraq Venus 838 GPS

## Command format

Command packets are issued over the UART serial interface with a specific format

| Start Sequence      | Payload Length | Payload           | Checksum | Stop Sequence        |
| ------------------- | -------------- | ----------------- | -------- | -------------------- |
| 2 bytes (0xA0, 0xA1)| 2 bytes        | 1 to 65,536 bytes | 1 byte   | 2 bytes (0x0D, 0x0A) |

Checksum calculation:

```python
checksum = 0
for byte in payload:
    checksum = checksum ^ byte
```

## Commands (more can be found [here](https://cdn.sparkfun.com/datasheets/Sensors/GPS/Venus/638/doc/AN0003_v1.4.19.pdf))

#### Set Factory Defaults
Reset internal parameters to factory defaults and reboot.<br>
<code>| A0 A1 | 00 02 | 04 01 | 05 | 0D 0A |</code><br>
Payload byte 1: Message ID = 04<br>
Payload byte 2: Mode = 01 (reboot after setting to factory defaults)

#### Configure Serial port
Configure the baud rate of the GPS receiver.<br>
<code>| A0 A1 | 00 04 | 05 00 01 00 | 04 | 0D 0A |</code><br>
Payload byte 1: Message ID = 05<br>
Payload byte 2: COM port = 00 (COM 1)<br>
Payload byte 3: Baud rate = 01 (0: 4800, 1: 9600 (Default), 2: 19200, 3: 38400, 4: 57600, 5: 115200)<br>
Payload byte 4: Attributes = 00 (00: normal update (RAM only), 01: update RAM and flash)

#### Configure NMEA message
Configure interval of each NMEA message.<br>
<code>| A0 A1 | 00 09 | 08 01 01 01 00 01 00 00 00 | 08 | 0D 0A |</code><br>
Payload byte 1: Message ID = 08<br>
Payload byte 2: GGA interval = 01 (0 - 255, 0: disable, 1: default)<br>
Payload byte 3: GSA interval = 01 (0 - 255, 0: disable, 1: default)<br>
Payload byte 4: GSV interval = 01 (0 - 255, 0: disable, 1: default)<br>
Payload byte 5: GLL interval = 00 (0 - 255, 0: disable (default))<br>
Payload byte 6: RMC interval = 01 (0 - 255, 0: disable, 1: default)<br>
Payload byte 7: VTG interval = 00 (0 - 255, 0: disable (default))<br>
Payload byte 8: ZDA interval = 00 (0 - 255, 0: disable (default))<br>
Payload byte 9: Attributes = 00 (00: normal update (RAM only), 01: update RAM and flash)

#### Configure Power Mode
Configure the system power mode<br>
<code>| A0 A1 | 00 03 | 0C 00 00 | 0C | 0D 0A |</code><br>
Payload byte 1: Message ID = 0C<br>
Payload byte 2: Mode = 00 (00: Normal, 01: Power Save)<br>
Payload byte 3: Attributes = 00 (00: RAM only, 01: RAM and flash, 02: temporarily enabled)

#### Configure Position/Update Frequency
Configure the update rate of the receiver. 1 - 2 Hz require 9600 Baud, 4 - 10 require 38400 Baud, 20 requires 115200 Baud.<br>
<code>| A0 A1 | 00 03 | 0E 01 00 | 0F | 0D 0A |</code><br>
Payload byte 1: Message ID = 0E<br>
Payload byte 2: Rate = 01 (01: 1 Hz (default), 02: 2 Hz, 04: 4 Hz, 05: 5 Hz, 08: 8 Hz, 0A, 10 Hz, 14: 20 Hz)<br>
Payload byte 3: Attributes = 00 (00: RAM only, 01: RAM and flash)

#### Configure 1PPS Mode
Configure the mode of the pulse-per-second pin.<br>
<code>| A0 A1 | 00 03 | 3E 00 00 | 3E | 0D 0A |</code><br>
Payload byte 1: Message ID = 3E<br>
Payload byte 2: 1PPS Mode = 00 (00: off, 01: on when 3D fix, 02: on when 1 SV)<br>
Payload byte 3: Attributes = 00 (00: RAM only, 01: RAM and flash)
