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
cmd_subfolder = os.path.realpath(os.path.abspath(os.path.join(os.path.split(inspect.getfile( inspect.currentframe() ))[0], "libs")))
if cmd_subfolder not in sys.path:
	sys.path.insert(0, cmd_subfolder)

#===============================================================================
# MAIN CODE
#===============================================================================
import decimal, urllib, urllib2, time
from datetime import datetime
from grovepi import *

# connect the DHT11 (temperature and humidity) sensor to digital port D7
dhtPort      = 7
REST_API_URL = "https://api.powerbi.com/beta/f5c57609-9307-457e-8e45-052f52120ad8/datasets/85119573-9bb9-42e4-8759-ff29876ae7f9/rows?key=%2FUZhZSvDW73NaRW7%2Bbjc%2BeH6JJfTXzNQaAg0kk1pNmgmhMV0UqP717GRWeVr%2BvNweDfeH40DCFI2zjqj%2BMfATA%3D%3D"

while True:
	try:
		# read and print out humidity and temperature from sensor
		[ temp, humidity ] = dht(dhtPort, 0)
		print 'Temp = {0:0.1f}*C - Humidity = {1:0.1f}%'.format(temp, humidity)

		# ensure that timestamp string is formatted properly
		now = datetime.strftime(datetime.now(), "%Y-%m-%dT%H:%M:%S%Z")

		# data that we're sending to Power BI REST API
		data = '[{{ "timestamp": "{0}", "temperature": "{1:0.1f}", "humidity": "{2:0.1f}" }}]'.format(now, temp, humidity)

		# make HTTP POST request to Power BI REST API
		req      = urllib2.Request(REST_API_URL, data)
		response = urllib2.urlopen(req)
		print("POST request to Power BI with data: {0}".format(data))
		print("Response: HTTP {0} {1}\n".format(response.getcode(), response.read()))	
		time.sleep(2)

	except (IOError,TypeError) as e:
		print("Error" + str(e))
    

