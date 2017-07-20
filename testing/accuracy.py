#!/usr/bin/python

import math
from sklearn import linear_model
from sklearn.preprocessing import PolynomialFeatures
from geopy.distance import vincenty

import serial
import glob
from os import listdir
import sys
import time

animation = "|/-\\"

baudrate = 115200
sample_size = 20
samples = 5
if len(sys.argv) == 3:
    sample_size = int(sys.argv[1])
    samples = int(sys.argv[2])

# Verify that Teensy is plugged in
try:
    devices = [device for device in listdir("/dev/serial/by-id")]
except OSError:
    print "No serpial devices are connected, please plug in the teensy"
    sys.exit()

teensycount = sum(["Teensy" in device for device in devices])

if teensycount > 1:
    print "Too many teensys detected, please only use one at a time"
    sys.exit()
elif teensycount < 1:
    print "No teensys detected, please plug in the teensy"
    sys.exit()

# Open serial port
ports = [device for device in glob.glob("/dev/tty[A-Z]*")]
port = "".join([mask * name for mask, name in zip(["Teensy" in device for device in devices], ports)])
ser = serial.Serial(port, baudrate)

# Telemetry will be stored in an array
explanatory = []
response = []
telemetry = []

raw_input("Preparing to collect training data and expected outputs, please ensure the antenna is stationary.\nPress enter to continue ... ")
print ""

#wait until there is valid position data
while True:
    msg = ser.readline().strip().split(',')
    if msg[0] != "999999999.999999999":
        break

new_mean = lambda old_mean, new_value, total: old_mean + (float(new_value) - old_mean) / total

def dist(lat1, lon1, lat2, lon2):
    dLon = lon2 * math.pi / 180 - lon1 * math.pi/180
    y = math.sin(dLon) * math.cos(lat2)
    x = math.cos(lat1) * math.sin(lat2) - math.sin(lat1) * math.cos(lat2) * math.cos(dLon)
    brng = math.atan2(y, x)
    if brng < 0:
        brng += 2 * math.pi
    dist = vincenty((lat1, lon1), (lat2, lon2)).meters
    dx = dist * math.sin(brng)
    dy = dist * math.cos(brng)
    return dx, dy

try:
    for i in xrange(samples):

        pvar_lat = 0
        mean_lat = 0
        mean_lat_prev = 0
        pvar_lon = 0
        mean_lon = 0
        mean_lon_prev = 0
        pvar_alt = 0
        mean_alt = 0
        mean_alt_prev = 0

        mean_pdop = 0
        mean_hdop = 0
        mean_vdop = 0
        mean_snr = 0

        for n in xrange(0, sample_size + 1):

            msg = ser.readline().strip().split(',')

            if n == 0:
                mean_lat = float(msg[0])
                mean_lon = float(msg[1])
                mean_alt = float(msg[2])
            elif n > 0:
                mean_pdop = new_mean(mean_pdop, float(msg[3]), n)
                mean_hdop = new_mean(mean_hdop, float(msg[4]), n)
                mean_vdop = new_mean(mean_vdop, float(msg[5]), n)
                mean_snr = new_mean(mean_snr, float(msg[6]), n)

                lat = float(msg[0])
                lon = float(msg[1])
                alt = float(msg[2])
                mean_lat_prev = mean_lat
                mean_lon_prev = mean_lon
                mean_alt_prev = mean_alt

                mean_lat = new_mean(mean_lat, lat, n)
                mean_lon = new_mean(mean_lon, lon, n)
                mean_alt = new_mean(mean_alt, alt, n)
                d1x, d1y = dist(lat, lon, mean_lat, mean_lon)
                d2x, d2y = dist(lat, lon, mean_lat_prev, mean_lon_prev)
                pvar_lat = pvar_lat + d1x * d2x
                pvar_lon = pvar_lon + d1y * d2y
                pvar_alt = pvar_alt + (alt - mean_alt) * (alt - mean_alt_prev)

                # Update status
                percentcplt = (i * sample_size + n) * 10000 / (samples * sample_size)
                sys.stdout.write('\033[FProgress: {0}.{1:0>2}% [{2}]\r\nMessage:{3}'.format(percentcplt / 100, percentcplt % 100, animation[n % 4], ','.join(msg)))
                sys.stdout.flush()

        var_lat = pvar_lat / sample_size
        var_lon = pvar_lon / sample_size
        var_alt = pvar_alt / sample_size

        poserr3d = 1.122 * (math.sqrt(var_lat) + math.sqrt(var_lon) + math.sqrt(var_alt))
        poserrhoriz = 2 * math.sqrt(var_lat + var_lon)
        telemetry.append([mean_pdop, mean_hdop, mean_vdop, mean_snr, mean_pdop / mean_snr, mean_hdop / mean_snr, mean_vdop / mean_snr, poserr3d, poserrhoriz, var_lat, var_lon, var_alt, sample_size])
        p = mean_pdop
        h = mean_hdop
        v = mean_vdop
        s = mean_snr
        explanatory.append([p, h, v, s, p**2, h**2, v**2, p*h*v, p*p/s, h*h/s, v*v/s, p/s, h/s, v/s, (p/s)**2, (h/s)**2, (v/s)**2])
        response.append(poserr3d)

except KeyboardInterrupt:
    response = raw_input("\n\nKeyboard Interrupt detected, write collected data to logfile? (y/n) ")
    if response == 'y' or response == 'Y' or response == "yes":
        pass
    else:
        sys.exit()

print "\n"
f = open('logs/log' + time.strftime('%Y%m%d_%H%M%S') + '.csv', 'a')
f.write("PDOP,HDOP,VDOP,SNR,PDOP/SNR,HDOP/SNR,VDOP/SNR,3D Error,2D Error,VAR LAT,VAR LON,VAR ALT, N\n")
for line in telemetry:
    for item in line:
        f.write(str(item) + ",")
    f.write('\n')
f.close()

explanatorynames = ["PDOP", "HDOP", "VDOP", "SNR", "PDOP^2", "HDOP^2", "VDOP^2", "PDOP*HDOP*VDOP", "PDOP^2/SNR", "HDOP^2/SNR", "VDOP^2/SNR", "PDOP/SNR", "HDOP/SNR", "VDOP/SNR", "(PDOP/SNR)^2", "(HDOP/SNR)^2", "(VDOP/SNR)^2"]

clf = linear_model.LinearRegression()
clf.fit(explanatory, response)
print "3D Error ~ " + '{:.3f}'.format(clf.intercept_) + "".join(["{0} {1:.3f}*{2}".format(" -" if clf.coef_[i] < 0 else " +", clf.coef_[i] *(-1 if clf.coef_[i] < 0 else 1), explanatorynames[i]) for i in xrange(len(clf.coef_))])
print "r^2 = ", clf.score(explanatory, response)
print "Average position error = ", sum(response) / len(response)
