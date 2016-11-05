/*
 The internal blue LED is connected to GPIO2 / D4
 The internal red LED is connected to GPIO16 / D0
 If use internal led, we cannot user Serial.print() at the same time
*/

int intLed = 16;
int extLed = 13;

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(extLed, OUTPUT);
}

void loop() {
  // note that LOW is the voltage level
  // digitalWrite(intLed, LOW);
  Serial.println("HIDUP");
  digitalWrite(extLed, HIGH);
  delay(1000);
  //digitalWrite(intLed, HIGH);
  Serial.println("MATI");
  digitalWrite(extLed, LOW);
  delay(2000);
}

