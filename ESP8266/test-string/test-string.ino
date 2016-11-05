#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "RAINMAKER-2G";
const char* pass = "key2-homewlan";

//const char* host = "https://api.powerbi.com/beta/f5c57609-9307-457e-8e45-052f52120ad8/datasets/fb4d1908-4060-4af6-9eec-e61e68c991c2/rows?key=rYfptyd1Fr4AYkKN8cORexlN8nZg8KjNUvxQZMH7S6KPfemMCb12yminmKxdOH5BwdBddj1Ht6pPNMx8Gu3mDQ%3D%3D";
const char* host = "api.powerbi.com";
String url  = "/beta/f5c57609-9307-457e-8e45-052f52120ad8/datasets/fb4d1908-4060-4af6-9eec-e61e68c991c2/rows?key=rYfptyd1Fr4AYkKN8cORexlN8nZg8KjNUvxQZMH7S6KPfemMCb12yminmKxdOH5BwdBddj1Ht6pPNMx8Gu3mDQ%3D%3D";
const char* fingerprint = "17 14 0E E6 8D A5 F8 3B 5B 94 A2 16 8B 3E 76 0B 69 84 1E 81";
const int httpsPort = 443;

String dataSend;
String myTS       = "2016-10-27T10:37:35Z";
float temperature = 25.0;
float humidity    = 80;

int sendCount = 0;
char buffer[256];

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  dataSend = "[{{ \"timestamp\": \"" + myTS + "\", \"temperature\": \"" + String(temperature) + "\", \"humidity\": \"" + String(humidity) + "\" }}]";
  Serial.println(dataSend);

  // use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  Serial.print("requesting URL: ");
  Serial.println(url);
  client.print(buildHttpRequest(serializeData())); 

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("NodeMCU successfull!");
  } else {
    Serial.println("NodeMCU has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");

  delay(2000);
}

String serializeData(){
  StaticJsonBuffer<JSON_OBJECT_SIZE(16)> jsonBuffer;  //  allow for a few extra json fields that actually being used at the moment
  JsonObject& root = jsonBuffer.createObject();

  root["timestamp"] = myTS;
  root["temperature"] = temperature;
  root["Id"] = ++sendCount;

  root.printTo(buffer, sizeof(buffer));
  return (String)buffer;
}

String buildHttpRequest(String dataSend){  
  return "POST " + url + " HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" +
    "Content-Type: application/atom+xml;type=entry;charset=utf-8\r\n" +
    "Content-Length: " + dataSend.length() + "\r\n\r\n" + dataSend;
}

