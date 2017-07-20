#!/usr/bin/python

# regressionfromlog.py
# This python script allows for regression of data stored
# in logfiles generated in the same format that accuracy.py generates

import pandas as pd
from sklearn import linear_model

filename = raw_input("Please enter a file name: ")

explanatory = []
response = []

f = open("logs/"+filename, 'r')
f.readline()
for line in f:
    msg = line.split(',')
    p = float(msg[0])
    h = float(msg[1])
    v = float(msg[2])
    s = float(msg[3])
    poserr3d = float(msg[7])
    explanatory.append([p, h, v, s, p**2, h**2, v**2, p*h*v, p*p/s, h*h/s, v*v/s, p/s, h/s, v/s, (p/s)**2, (h/s)**2, (v/s)**2])
    response.append(poserr3d)

f.close()

explanatorynames = ["PDOP", "HDOP", "VDOP", "SNR", "PDOP^2", "HDOP^2", "VDOP^2", "PDOP*HDOP*VDOP", "PDOP^2/SNR", "HDOP^2/SNR", "VDOP^2/SNR", "PDOP/SNR", "HDOP/SNR", "VDOP/SNR", "(PDOP/SNR)^2", "(HDOP/SNR)^2", "(VDOP/SNR)^2"]

clf = linear_model.LinearRegression()
clf.fit(explanatory, response)
print "3D Error ~ " + '{:.3f}'.format(clf.intercept_) + "".join(["{0} {1:.3f}*{2}".format(" -" if clf.coef_[i] < 0 else " +", clf.coef_[i] *(-1 if clf.coef_[i] < 0 else 1), explanatorynames[i]) for i in xrange(len(clf.coef_))])
print "r^2 = ", clf.score(explanatory, response)
print "Average position error = ", sum(response) / len(response)
