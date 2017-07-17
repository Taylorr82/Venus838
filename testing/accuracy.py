import math
import matplotlib.pyplot as plt
from scipy import stats
import serial
import glob
from os import listdir
import sys
import time

# According to UTDallas paper
#
# accuracy = sqrt(var(actual_range_err)) * GDOP
#
#        sqrt(var(lat) + var(lon) + var(alt) + (c*dt)^2)
# GDOP = -----------------------------------------------
#                      sqrt(var(range))
#
# PDOP = sqrt(var(lat) + var(lon) + var(alt)) / actual_range_err
# HDOP = sqrt(var(lat) + var(lon)) / actual_range_err
# VDOP = sqrt(var(alt)) / actual_range_err
#

baudrate = 9600
confidence = 0.95

setup_packet_count = 100
measurement_packet_count = 40
super_sample_size = 10

try:
    devices = [device for device in listdir("/dev/serial/by-id")]
except OSError:
    print "No serial devices are connected, please plug in the teensy"
    sys.exit()

teensycount = sum(["Teensy" in device for device in devices])

if teensycount > 1:
    print "Too many teensys detected, please only use one at a time"
    sys.exit()
elif teensycount < 1:
    print "No teensys detected, please plug in the teensy"
    sys.exit()

ports = [device for device in glob.glob("/dev/tty[A-Z]*")]

port = "".join([mask * name for mask, name in zip(["Teensy" in device for device in devices], ports)])

ser = serial.Serial(port, baudrate)

true_lat = 0
true_lon = 0
true_alt = 0

raw_input("Measuring true location, please ensure the antenna is stationary.\nPress enter to continue")

for i in range(1, setup_packet_count + 1):
    sys.stdout.write('\r[{0}] {1}%'.format('=' * (i * 50 / setup_packet_count), i * 100 / setup_packet_count))
    sys.stdout.flush()
    msg = ser.readline().strip().split(',')
    true_lat = (true_lat * (i - 1) + float(msg[1])) / i
    true_lon = (true_lon * (i - 1) + float(msg[2])) / i
    true_alt = (true_alt * (i - 1) + float(msg[3])) / i

print "\nTrue location =", true_lat, ",", true_lon, ",", true_alt / 100.0

raw_input("Leaving the antenna in the same location, rotate it slowly about each axis.\nPress enter to continue")


def mean(sample):
    return sum(sample)/len(sample)

def std_dev(sample, mean):
    return math.sqrt((1.0 / (len(sample) - 1)) * sum([(x - mean) ** 2 for x in sample]))

def var(sample, mean):
    return (1.0 / (len(sample) - 1)) * sum([(x - mean) ** 2 for x in sample])

rss_latlonalt = []
rss_latlon = []
rss_alt = []
err_list = []
pdop_list = []
hdop_list = []
vdop_list = []
snr_list = []

for i in range(measurement_packet_count):
    sys.stdout.write('\r[{0}] {1}%'.format('=' * (i * 50 / measurement_packet_count), i * 100 / measurement_packet_count))
    sys.stdout.flush()
    lat = []
    lon = []
    alt = []
    pdop = []
    hdop = []
    vdop = []
    snr = []
    for j in range(super_sample_size):
        msg = ser.readline().strip().split(',')
        lat.append(float(msg[1]))
        lon.append(float(msg[2]))
        alt.append(float(msg[3]))
        pdop.append(float(msg[4]))
        hdop.append(float(msg[5]))
        vdop.append(float(msg[6]))
        snr.append(float(msg[9]))

    rss_latlonalt.append(math.sqrt(var(lat, true_lat) + var(lon, true_lon) + var(alt, true_alt)))
    rss_latlon.append(math.sqrt(var(lat, true_lat) + var(lon, true_lon)))
    rss_alt.append(std_dev(alt, true_alt))

    pdop_list.append(sum(pdop) / super_sample_size)
    hdop_list.append(sum(hdop) / super_sample_size)
    vdop_list.append(sum(vdop) / super_sample_size)

    err_list.append(1.122 * (std_dev(lat, true_lat) + std_dev(lon, true_lon) + std_dev(alt, true_alt)))

    snr_list.append(sum(snr) / super_sample_size)


def makeplot(x, y, axis, name):
    slope, intercept, r_value, p_value, std_err = stats.linregress(x, y)
    f = lambda x: slope * x + intercept
    axis.plot((min(x), max(x)), (f(min(x)), f(max(x))), c="red")
    axis.scatter(x, y)
    axis.set_title(name + ' (r = ' + str(r_value) + ')')

# error vs. sqrt(var(lat) + var(lon) + var(alt)) / pdop
# error vs. sqrt(var(lat) + var(lon)) / hdop
# error vs. sqrt(var(alt)) / vdop

print "Error list:", err_list
print "rss_latlonalt:", rss_latlonalt
print "rss_latlon:", rss_latlon
print "rss_alt:", rss_alt

f, (err_v_latlonalt_pdop, err_v_latlon_hdop, err_v_alt_vdop) = plt.subplots(1, 3, sharey=True)
makeplot(err_list, rss_latlonalt, err_v_latlonalt_pdop, "Error vs. rss(Lat, Lon, Alt)/PDOP")
makeplot(err_list, rss_latlon, err_v_latlon_hdop, "Error vs. rss(Lat, Lon)/HDOP")
makeplot(err_list, rss_alt, err_v_alt_vdop, "Error vs. rss(Alt)/VDOP")

plt.show()

'''
f, ((lt_v_sp, lt_v_p, lt_v_s), (ln_v_sp, ln_v_p, ln_v_s), (at_v_sp, at_v_p, at_v_s)) = plt.subplots(3, 3, sharex='col', sharey='row')

makeplot(snrpdop_list, lat_err_list, lt_v_sp, "Latitude vs. SNR / PDOP")
makeplot(pdop_list, lat_err_list, lt_v_p, "Latitude vs. PDOP")
makeplot(snr_list, lat_err_list, lt_v_s, "Latitude vs. SNR")

makeplot(snrpdop_list, lon_err_list, ln_v_sp, "Longitude vs. SNR / PDOP")
makeplot(pdop_list, lon_err_list, ln_v_p, "Longitude vs. PDOP")
makeplot(snr_list, lon_err_list, ln_v_s, "Longitude vs. SNR")

makeplot(snrpdop_list, alt_err_list, at_v_sp, "Altitude vs. SNR / PDOP")
makeplot(pdop_list, alt_err_list, at_v_p, "Altitude vs. PDOP")
makeplot(snr_list, alt_err_list, at_v_s, "Altitude vs. SNR")
'''
