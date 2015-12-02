// initializes output pin of the LM35 temperature sensor
int   LM35_PIN   = 0;
int   rawVoltage = 0;
float celsius    = 0;
float milliVolt  = 0;

// initializes JSON variables
char sensorID[]   = "Arduino-LM35";
char sensorType[] = "Temperature";
char sensorUnit[] = "C";
char celsiusMsg[] = "00.00";
char buffer[300];


void setup()
{
  // for higher resolution, uncomment the following line which will make the aRef to 1.1V rather than 5V
  // analogReference(INTERNAL);
  Serial.begin(9600);
}

void loop()
{
  rawVoltage = analogRead(LM35_PIN);
  // for higher resolution, uncomment the following line
  // celsius = rawVoltage / 9.31;

  // for common resolution, uncomment the following two lines
  milliVolt = (rawVoltage / 1024.0) * 5000;
  celsius   = milliVolt / 10;
  dtostrf(celsius, 5, 2, celsiusMsg);

  // sent as one Serial.print to reduce risk of serial errors
  memset(buffer, '\0', sizeof(buffer));
  strcat(buffer, "SensorID:");
  strcat(buffer, sensorID);
  strcat(buffer, ",SensorType:");
  strcat(buffer, sensorType);
  strcat(buffer,",SensorValue:");
  strcat(buffer, celsiusMsg);
  Serial.println(buffer);

  // this part will output result as JSON format
  /* memset(buffer, '\0', sizeof(buffer));
  strcat(buffer, "{");
  strcat(buffer, "\"SensorID\":\"");
  strcat(buffer, sensorID);
  strcat(buffer, "\",\"SensorType\":\"");
  strcat(buffer, sensorType);
  strcat(buffer,"\",\"SensorUnit\":\"");
  strcat(buffer, sensorUnit);
  strcat(buffer,"\",\"SensorValue\":");
  strcat(buffer, celsiusMsg);
  strcat(buffer, "}");
  Serial.println(buffer); */

  /* Serial.print("Temperature: ");
  Serial.print(celsius);
  Serial.print(" Celsius");
  Serial.println(); */
  delay(3000);
}

