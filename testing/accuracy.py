import math
from sklearn import linear_model
import numpy as np
from pandas.plotting import scatter_matrix
import pandas as pd
import serial
import glob
from os import listdir
import sys

baudrate = 9600
sample_size = 50
samples = 100

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

# Telemetry will be stored in an array
training_data = []
expected_outputs = []

raw_input("Preparing to collect training data and expected outputs, please ensure the antenna is stationary.\nPress enter to continue ... ")

while True:
    msg = ser.readline().strip().split(',')
    if msg[0] != "999999999.999999999": #wait until there is valid position data
        break

new_mean = lambda old_mean, new_value, total: old_mean + (float(new_value) - old_mean) / total

for i in xrange(samples):

    pvar_lat = 0
    mean_lat = 0
    pvar_lon = 0
    mean_lon = 0
    pvar_alt = 0
    mean_alt = 0

    mean_pdop = 0
    mean_hdop = 0
    mean_vdop = 0
    mean_numsats = 0
    mean_numsatsvisible = 0
    mean_snr = 0
    mean_nsnr = 0

    for n in xrange(1, sample_size + 1):
        sys.stdout.write('\rExperiment {0} Progress: {1}% [{2}]'.format(i + 1, n * 100 / sample_size, '=' * (n * 50 / sample_size)))
        sys.stdout.flush()
        msg = ser.readline().strip().split(',')
        mean_pdop = new_mean(mean_pdop, float(msg[3]) / 100.0, n)
        mean_hdop = new_mean(mean_hdop, float(msg[4]) / 100.0, n)
        mean_vdop = new_mean(mean_vdop, float(msg[5]) / 100.0, n)
        mean_numsats = new_mean(mean_numsats, msg[6], n)
        mean_numsatsvisible = new_mean(mean_numsatsvisible, msg[7], n)
        mean_snr = new_mean(mean_snr, msg[8], n)
        mean_nsnr = new_mean(mean_nsnr, msg[9], n)

        lat = float(msg[0])
        lon = float(msg[1])
        alt = float(msg[2]) / 100.0
        mean_lat_prev = mean_lat
        mean_lon_prev = mean_lon
        mean_alt_prev = mean_alt

        mean_lat = new_mean(mean_lat, lat, n)
        mean_lon = new_mean(mean_lon, lon, n)
        mean_alt = new_mean(mean_alt, alt, n)
        pvar_lat = pvar_alt + (lat - mean_lat) * (lat - mean_lat_prev)
        pvar_lon = pvar_lon + (lon - mean_lon) * (lon - mean_lon_prev)
        pvar_alt = pvar_alt + (alt - mean_alt) * (alt - mean_alt_prev)

    training_data.append([mean_pdop, mean_hdop, mean_vdop, mean_numsats, mean_numsatsvisible, mean_snr, mean_nsnr, mean_pdop / mean_snr, mean_hdop / mean_snr, mean_vdop / mean_snr])
    expected_outputs.append(1.122 * (math.sqrt(pvar_lat / sample_size) + math.sqrt(pvar_lon / sample_size) + math.sqrt(pvar_alt / sample_size)))

    raw_input("\nExperiment {0} Complete; please move to a new location.\nPress Enter to begin a new experiment.".format(i + 1))

clf = linear_model.LinearRegression()
x_ = np.array(training_data)
y_ = np.array(expected_outputs)
clf.fit(x_, y_)
print ""
print "r^2 =", clf.score(x_,y_)
error_str = str(clf.intercept_) + " + " + str(clf.coef_[0]) + "*pdop + " + str(clf.coef_[1]) + "*hdop + " + str(clf.coef_[2]) + "*vdop + "
error_str += str(clf.coef_[3]) + "*nsats + " + str(clf.coef_[4]) + "*nsatsvis + " + str(clf.coef_[5]) + "*snr + " + str(clf.coef_[6]) + "*nsnrsats + "
error_str += str(clf.coef_[7]) + "*pdop/snr + " + str(clf.coef_[8]) + "*hdop/snr + " + str(clf.coef_[9]) + "*vdop/snr"
print "position error ~ " + error_str
print "mean squared residual error = " + str(np.mean((clf.predict(training_data) - expected_outputs) ** 2))


#print "Training Data:", training_data
#print "Expected Outputs:", expected_outputs

print "Average of Expected Outputs:", sum(expected_outputs)/len(expected_outputs)


'''
Best Model so far (probably is only accurate in the same location on the stairs)
r^2 = 0.844873462104
position error ~ 108.563753131 + -4.51047055041*pdop + 9.16937525521*hdop + 9.78784105119*vdop + 9.4089397195*nsats + 6.39488
462184e-14*nsatsvis + -0.0524476990033*snr + -17.3488873339*nsnrsats
mean squared error = 1.07971906724
Average of Expected Outputs: 1.72019234181

another model (poor accuracy)
r^2 = 1.0
position error ~ -8116.8042914 + -383.250083143*pdop + -3121.80865776*hdop + 379.392802833*vdop + -497.791074802*nsats + -1.2
2271894725e-05*nsatsvis + 3.26136477417*snr + -212.207171387*nsnrsats + 1761963.90775*pdop/snr + 9455269.83931*hdop/snr + -12
77396.54818*vdop/snr
mean squared residual error = 7.43505608246e-20
Average of Expected Outputs: 321.023412359
'''
