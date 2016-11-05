#!/usr/bin/env python

#===============================================================================
# the following snippet will include modules in subdirectory.
# make sure in the subdirectory there's an __init__.py file
#===============================================================================
import os, sys, inspect
# realpath() will make your script run, even if you symlink it :)
cmd_folder = os.path.realpath(os.path.abspath(os.path.split(inspect.getfile( inspect.currentframe() ))[0]))
if cmd_folder not in sys.path:
	sys.path.insert(0, cmd_folder)

# use this if you want to include modules from a subfolder
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0],"libs")))
if cmd_subfolder not in sys.path:
	sys.path.insert(0, cmd_subfolder)

#===============================================================================
# MAIN CODE
#===============================================================================
import time
import grovepi
from grove_rgb_lcd import *

# connect the LM2904 (sound / loudness) sensor to analog port A0
soundSensor = 0
hustMsg     = "Gandeng siah!!!"
isNoisy     = 0;

while True:
	try:
		# Read the sound level
		sensorValue = grovepi.analogRead(soundSensor)

		#print 'sensorValue = {0:d}'.format(sensorValue)

		if ( (sensorValue > 500) and (sensorValue != 65535) ):
			print 'sensorValue = {0:d}'.format(sensorValue)
			isNoisy  = 1
			hustList = list(hustMsg)
			setRGB(255, 0, 0)
			setText("" . join(hustList))
			time.sleep(3)
		else:
			if (isNoisy == 1):
				isNoisy = 0
				setRGB(0, 0, 0)
				hustList = list(hustMsg)
				for i in range(len(hustList)):
					hustList[i] = " "
					setText_norefresh("" . join(hustList))
					#time.sleep(.1)

		time.sleep(.5)

	except IOError:
		print ("Error")
