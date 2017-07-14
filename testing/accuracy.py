import math
from scipy.stats import t
import matplotlib.pyplot as plt
import serial
import glob
from os import listdir
import sys

baudrate = 9600
confidence = 0.95
experiment_size = 5
experiment_count = 5

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

lat_list = []
lon_list = []

for i in range(5000):
    msg = ser.readline().strip().split(',')
    print "Message", i, ', '.join(msg)
    lat_list.append(msg[1])
    lon_list.append(msg[2])


plt.scatter(lat_list, lon_list)
plt.show()

'''ME_lat_list = []
ME_lon_list = []
ME_alt_list = []
PDOP_bar_list = []
SNR_bar_list = []

for experiment in range(experiment_count):

    lat_list = []
    lon_list = []
    alt_list = []
    pdop_list = []
    snr_list = []

    for n in range(0, experiment_size):
            msg = ser.readline().strip().split(',')
            print ', '.join(msg)
            lat = int(msg[1])
            lon = int(msg[2])
            alt = int(msg[3])
            pdop = int(msg[4])
            snr = int(msg[9])

            lat_list.append(lat)
            lon_list.append(lon)
            alt_list.append(alt)
            pdop_list.append(pdop)
            snr_list.append(snr)

    lat_bar = sum(lat_list) / experiment_size
    lon_bar = sum(lon_list) / experiment_size
    alt_bar = sum(alt_list) / experiment_size
    pdop_bar = sum(pdop_list) / experiment_size
    snr_bar = sum(snr_list) / experiment_size

    t_critical = t.ppf(1 - (1 - confidence) / 2, experiment_size - 1)

    # ME = t_crit * std_dev / sqrt(n)
    ME_lat = t_critical * 1000000 * math.sqrt(sum([(lat - lat_bar) ** 2 for lat in lat_list]) / ((experiment_size - 1) * experiment_size))
    ME_lon = t_critical * 1000000 * math.sqrt(sum([(lon - lon_bar) ** 2 for lon in lon_list]) / ((experiment_size - 1) * experiment_size))
    ME_alt = t_critical * 1000000 * math.sqrt(sum([(alt - alt_bar) ** 2 for alt in alt_list]) / ((experiment_size - 1) * experiment_size))

    ME_lat_list.append(ME_lat)
    ME_lon_list.append(ME_lon)
    ME_alt_list.append(ME_alt)
    PDOP_bar_list.append(pdop_bar)
    SNR_bar_list.append(snr_bar)

    x = raw_input("Experiment " + str(experiment) + " complete,\nPlease move the GPS antenna or change its attitude for additional experiments.\nPress Enter to continue")

    ser.flushInput()

plt.close('all')

f, ((lat__snr_pdop, lat__pdop, lat__snr), (lon__snr_pdop, lon__pdop, lon__snr), (lat__snr_pdop, lat__pdop, lat__snr)) = plt.subplots(3, 3, sharex='col', sharey='row')

snr_pdop_prod = [snr * pdop for snr, pdop in zip(SNR_bar_list, PDOP_bar_list)]

lat__snr_pdop.scatter(snr_pdop_prod, ME_lat_list)
lat__snr_pdop.set_title('latitude vs. SNR * PDOP')
lat__pdop.scatter(PDOP_bar_list, ME_lat_list)
lat__pdop.set_title('latitude vs. PDOP')
lat__snr.scatter(SNR_bar_list, ME_lat_list)
lat__snr.set_title('latitude vs. SNR')

lon__snr_pdop.scatter(snr_pdop_prod, ME_lon_list)
lon__snr_pdop.set_title('Longitude vs. SNR * PDOP')
lon__pdop.scatter(PDOP_bar_list, ME_lon_list)
lon__pdop.set_title('Longitude vs. PDOP')
lon__snr.scatter(SNR_bar_list, ME_lon_list)
lon__snr.set_title('Longitude vs. SNR')

alt__snr_pdop.scatter(snr_pdop_prod, ME_alt_list)
alt__snr_pdop.set_title('Altitude vs. SNR * PDOP')
alt__pdop.scatter(PDOP_bar_list, ME_alt_list)
alt__pdop.set_title('Altitude vs. PDOP')
alt__snr.scatter(SNR_bar_list, ME_alt_list)
alt__snr.set_title('Altitude vs. SNR')

plt.show()'''
