#!/usr/bin/python

import serial

usbPort = "/dev/ttyACM0"
arduino = serial.Serial(usbPort, 9600)
arduino.flushInput()
arduino.flushOutput()

while True:
	line = arduino.readline()
	line = line.rstrip('\r\n')
	print "LINE: " + line

arduino.close()
