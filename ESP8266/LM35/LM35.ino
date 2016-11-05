#include <ESP8266WiFi.h>
/* NodeMCU pin mapping to Arduino */
#define D0 16
#define D1 5  // I2C Bus SCL (clock)
#define D2 4  // I2C Bus SDA (data)
#define D3 0
#define D4 2  // same as "LED_BUILTIN", but inverted logic
#define D5 14 // SPI Bus SCK (clock)
#define D6 12 // SPI Bus MISO 
#define D7 13 // SPI Bus MOSI
#define D8 15 // SPI Bus SS (CS)
#define D9 3  // RX0 (Serial console)
#define D10 1 // TX0 (Serial console)
/* initializes output pin of the LM35 temperature sensor */
int   LM35_PIN   = A0;
int   rawVoltage = 0;
float celcius    = 0;
int   extLed     = D7;
/* initializes JSON variables */
char sensorID[]   = "NodeMCU-LM35";
char sensorType[] = "Temperature";
char sensorUnit[] = "C";
char celciusMsg[] = "00.0";
char buffer[300];
/* initialize wifi properties */
const char* ssid     = "RainMaker-2G";
const char* password = "key2-homewlan";

void setup() {
  Serial.begin(115200);
  pinMode(extLed, OUTPUT);
  delay(100);
}

void loop() {
  rawVoltage = analogRead(LM35_PIN);
  digitalWrite(extLed, HIGH);
  /*
   * 3.1  = supply voltage
   * 1000 = volt to millivolt
   * 1024 = number of bits (10 bits)
   * 10   = every degree rise in temperature the output of LM35 will rise by 10mV
   */
  celcius = ((rawVoltage * 2.85 * 1000) / 1024) / 10;
  dtostrf(celcius, 4, 1, celciusMsg);

  // sent as one Serial.print to reduce risk of serial errors
  /*
  memset(buffer, '\0', sizeof(buffer));
  strcat(buffer, "SensorID:");
  strcat(buffer, sensorID);
  strcat(buffer, ",SensorType:");
  strcat(buffer, sensorType);
  strcat(buffer,",SensorValue:");
  strcat(buffer, celciusMsg);
  Serial.println(buffer);
  */

  // this part will output result as JSON format
  memset(buffer, '\0', sizeof(buffer));
  strcat(buffer, "{");
  strcat(buffer, "\"SensorID\":\"");
  strcat(buffer, sensorID);
  strcat(buffer, "\",\"SensorType\":\"");
  strcat(buffer, sensorType);
  strcat(buffer,"\",\"SensorUnit\":\"");
  strcat(buffer, sensorUnit);
  strcat(buffer,"\",\"SensorValue\":");
  strcat(buffer, celciusMsg);
  strcat(buffer, "}");
  Serial.println(buffer);
  digitalWrite(extLed, LOW);

  delay(3000);
}

