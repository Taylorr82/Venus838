import math
from scipy.stats import t
import matplotlib.pyplot as plt
import serial
import glob
from os import listdir
import sys

baudrate = 9600
confidence = 0.95

setup_packet_count = 2500
measurement_packet_count = 1200

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
    sys.stdout.write('\r[{0}] {1}%'.format('#' * (i * 50 / setup_packet_count), i * 100 / setup_packet_count))
    sys.stdout.flush()
    msg = ser.readline().strip().split(',')
    true_lat = (true_lat * (i - 1) + int(msg[1])) / i
    true_lon = (true_lon * (i - 1) + int(msg[2])) / i
    true_alt = (true_alt * (i - 1) + int(msg[3])) / i

print "\nTrue location =", true_lat / 10000000.0, ",", true_lon / 10000000.0, ",", true_alt / 100.0

raw_input("Leaving the antenna in the same location, rotate it slowly about each axis.\nPress enter to continue")

lat_err_list = []
lon_err_list = []
alt_err_list = []
pdop_list = []
snr_list = []
snrpdop_list = []

for i in range(measurement_packet_count):
    sys.stdout.write('\r[{0}] {1}%'.format('#' * (i * 50 / measurement_packet_count), i * 100 / measurement_packet_count))
    sys.stdout.flush()
    msg = ser.readline().strip().split(',')
    lat_err_list.append((int(msg[1]) - true_lat) ** 2)
    lon_err_list.append((int(msg[2]) - true_lon) ** 2)
    alt_err_list.append((int(msg[3]) - true_alt) ** 2)
    pdop_list.append(int(msg[4]))
    snr_list.append(int(msg[9]))
    snrpdop_list.append(int(msg[4]) * int(msg[9]))

f, ((lt_v_sp, lt_v_p, lt_v_s), (ln_v_sp, ln_v_p, ln_v_s), (at_v_sp, at_v_p, at_v_s)) = plt.subplots(3, 3, sharex='col', sharey='row')

lt_v_sp.scatter(snrpdop_list, lat_err_list)
lt_v_sp.set_title('Latitude vs. SNR * PDOP')
lt_v_p.scatter(pdop_list, lat_err_list)
lt_v_p.set_title('Latitude vs. PDOP')
lt_v_s.scatter(snr_list, lat_err_list)
lt_v_s.set_title('Latitude vs. SNR')
ln_v_sp.scatter(snrpdop_list, lon_err_list)
ln_v_sp.set_title('Longitude vs. SNR * PDOP')
ln_v_p.scatter(pdop_list, lon_err_list)
ln_v_p.set_title('Longitude vs. PDOP')
ln_v_s.scatter(snr_list, lon_err_list)
ln_v_s.set_title('Longitude vs. SNR')
at_v_sp.scatter(snrpdop_list, alt_err_list)
at_v_sp.set_title('Altitude vs. SNR * PDOP')
at_v_p.scatter(pdop_list, alt_err_list)
at_v_p.set_title('Altitude vs. PDOP')
at_v_s.scatter(snr_list, alt_err_list)
at_v_s.set_title('Altitude vs. SNR')
plt.show()
