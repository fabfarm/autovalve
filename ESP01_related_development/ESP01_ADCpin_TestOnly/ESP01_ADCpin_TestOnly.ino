const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0

int analogValue = 0;  // value read from the analog pin

void setup() {
  // initialize serial communication at 115200
  Serial.begin(115200);
}

void loop() {
  // read the analog in value
  analogValue = analogRead(analogInPin);
  
  // print the readings in the Serial Monitor
  Serial.print("ESP-01 analog sensor value = ");
  Serial.println(analogValue);

  delay(1000);
}
