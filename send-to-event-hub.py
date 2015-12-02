#!/usr/bin/python

#****************************************************************#
# $Id: Send to Azure Event Hub - Aug 2015 Exp $                  #
# $Id: send-to-event-hub.py Exp $                                #
# $Id: BlackOps Team Exp $                                       #
# $Id: The RainMaker (rainmaker at idenx dot web dot id) Exp $   #
#****************************************************************#

import azure
import socket
import sys
from datetime import datetime

##################################################################
# initialize variables to suit your needs
eventHubHost = "TRMIOT-ns.servicebus.windows.net"
eventHubPath = "EHDevices"
sasKeyName   = "RootManageSharedAccessKey"
sasKeyValue  = "v6PGCzL8fX1FVPa6hPjP/bL8qQduRd8cPp2Y+Vr7OFs="

##################################################################
# DO NOT MODIFY ANYTHING AFTER THIS LINE
from azure.servicebus import (
	_service_bus_error_handler
)

from azure.servicebus.servicebusservice import (
	ServiceBusService,
	ServiceBusSASAuthentication
)

from azure.http import (
	HTTPRequest,
	HTTPError
)

from azure.http.httpclient import _HTTPClient

def isfloat(value):
	try:
		float(value)
		return True
	except:
		return False

class EventHubClient(object):
	def sendMessage(self, body, partition):
		authentication = ServiceBusSASAuthentication(sasKeyName, sasKeyValue)

		httpclient     = _HTTPClient(service_instance = self)
		request        = HTTPRequest()
		request.method = "POST"
		request.host   = eventHubHost
		request.protocol_override = "https"
		request.path   = "/" + eventHubPath + "/publishers/" + partition + "/messages"
		request.body   = body
		request.headers.append(('Content-Type', 'application/atom+xml;type=entry;charset=utf-8'))

		authentication.sign_request(request, httpclient)

		request.headers.append(('Content-Length', str(len(request.body))))
		status = 0

		try:
			resp = httpclient.perform_request(request)
			status = resp.status
		except HTTPError as ex:
			status = ex.status

		return status

class EventDataParser(object):
	def getMessage(self, payload, host):
		body = '{ "DeviceID" : "' + host + '", '

		# input : SensorID:Arduino-LM35,SensorType:Temperature,SensorValue:29.30
		# output: { "DeviceID" : "RainMaker-RPi2B", "SensorID" : "Arduino-LM35", "SensorType" : "Temperature", "SensorValue" : 29.30, "TimeCreated" : "2015-08-23 05:59:38.278467" }
		msgs = payload.split(",")

		for msg in msgs:
			sensorKey, sensorVal = msg.split(":")
			if isfloat(sensorVal) is False:
				body += '"' + sensorKey + '" : "' + sensorVal + '", '
			else:
				body += '"' + sensorKey + '" : ' + sensorVal + ', '

		# add created time
		body += '"TimeCreated" : "' + str(datetime.utcnow()) + '"'
		# remove the trailing ", "
		#body = body[:-2]
		body += " }"

		return body

##################################################################
# main program
hubClient = EventHubClient()
msgParser = EventDataParser()
hostname  = socket.gethostname()

body      = msgParser.getMessage(sys.argv[1], hostname)
print "OUTPUT: " + body
hubStatus = hubClient.sendMessage(body, hostname)
# return the HTTP status to the caller
print "RETURN: ", hubStatus
