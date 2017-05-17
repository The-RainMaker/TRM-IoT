#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
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
int LM35_PIN   = A0;
int rawVoltage = 0;
float celcius  = 0;
/* initialize wifi properties */
const char ssid[] = "NOKIA Lumia 820_0527";
const char pass[] = "papamarcus";
/* initializes time-related variables, ntp server using ntp.nict.jp */
#define TIMEZONE 7
IPAddress timeServer(133, 243, 238, 243);
String currTS          = "";
String dataSend        = "";
time_t prevDisplay     = 0;
unsigned int localPort = 8888;
WiFiUDP Udp;
/* REST API */
const char* host = "api.powerbi.com";
String url = "/beta/f5c57609-9307-457e-8e45-052f52120ad8/datasets/fb4d1908-4060-4af6-9eec-e61e68c991c2/rows?key=rYfptyd1Fr4AYkKN8cORexlN8nZg8KjNUvxQZMH7S6KPfemMCb12yminmKxdOH5BwdBddj1Ht6pPNMx8Gu3mDQ%3D%3D";
char buffer[256];

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.print(" ");

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("Waiting for sync");
  setSyncProvider(GetNTPTime);
  setSyncInterval(60 * 60);
}

void loop()
{
  /* read sensor data */
  rawVoltage = analogRead(LM35_PIN);
  /*
   * 2.85 = supply voltage
   * 1000 = volt to millivolt
   * 1024 = number of bits (10 bits)
   * 10   = every degree rise in temperature the output of LM35 will rise by 10mV
   */
  celcius = ((rawVoltage * 2.85 * 1000) / 1024) / 10;
  /* get timestamp */
  prevDisplay = now();
  currTS = TStoString();

  /* data that we're sending to Power BI REST API */
  //dataSend = "[{{ \"timestamp\": \"" + currTS + "\", \"temperature\": \"" + String(celcius) + "\" }}]";
  dataSend = serializeData();
  Serial.println(dataSend);

  /* use WiFiClientSecure class to create TLS connection */
  WiFiClientSecure client;
  Serial.print("Connecting to: ");
  Serial.println(host);
  if (!client.connect(host, 443)) {
    Serial.println("CONNECTION FAILED");
    return;
  }

  Serial.print("Requesting URL: ");
  Serial.println(url);
  client.print(buildHttpRequest(serializeData())); 
  Serial.println("Request sent");

  String response = "";
  String chunk    = "";
  int limit       = 1;
  
  do {
    if (client.connected()) { 
      yield();
      chunk = client.readStringUntil('\n');
      response += chunk;
    }
  } while (chunk.length() > 0 && ++limit < 100);  
  
  Serial.print("Response chunks ");
  Serial.print(limit);
  Serial.print(", Response code: ");
  
  if (response.length() > 12) { Serial.println(response.substring(9, 12)); }
  else { Serial.println("unknown"); }
  
  Serial.println();
  delay(2000);
}

String TStoString(){
  String myTS = String(year()) + '-' + String(month()) + '-' + String(day()) + 'T';
  myTS = myTS + String(hour()) + ':' + String(minute()) + ':' + String(second());
  //myTS = myTS + String(hour()) + ':' + String(minute()) + ':' + String(second()) + 'Z';
  return myTS;
}

String serializeData(){
  StaticJsonBuffer<JSON_OBJECT_SIZE(16)> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["timestamp"] = currTS;
  root["temperature"] = celcius;

  root.printTo(buffer, sizeof(buffer));
  return (String)buffer;
}

String buildHttpRequest(String dataSend){  
  return "POST " + url + " HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" +
    "Content-Type: application/atom+xml;type=entry;charset=utf-8\r\n" +
    "Content-Length: " + dataSend.length() + "\r\n\r\n" + dataSend;
}

