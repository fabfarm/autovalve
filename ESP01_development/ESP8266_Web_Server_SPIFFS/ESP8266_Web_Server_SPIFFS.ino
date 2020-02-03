#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>

// Replace with your network credentials
const char* ssid = "fabfarm-ele-container";
const char* password = "imakestuff";

const int ledPin = 2; // Set LED GPIO
String ledState; // Stores LED state
const int sensorPin = A0; // connected to ADC pin of ESP-01

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// analog sampling variables
#define NUMBER_OF_SAMPLES 5000  // maximum number of samples taken
long samples[NUMBER_OF_SAMPLES]; // declare maximum size of array
int numberOfSamples = 200; // number of samples taken in a single shoot
unsigned long samplingTime;  // time in us to collect all the numberOfSamples
int samplingPause = 4; // time in ms to pause between continuous sampling

unsigned long totalSamples = 0;
unsigned long millisLastSample;
unsigned long showStatisticsTimer = 0;
long sumOfSampleValues = 0; // sum of samples based on numberOfSamples
long averageAnalogValue; // average value of analog readings based on numberOfSamples

void analogSample(void)
{
  if (millis() - samplingPause < millisLastSample )
  {
    return;
  }
  samplingTime = micros();
  for (int i = 0; i < numberOfSamples; i++)
  {
    samples[i] = analogRead(A0);
    totalSamples++;
    sumOfSampleValues += samples[i];
  }
  samplingTime = micros() - samplingTime;

  averageAnalogValue = sumOfSampleValues / numberOfSamples; // average analog value based on numberOfSamples
  sumOfSampleValues = 0; // reset for next burst reading
  millisLastSample = millis();
}

/*
void showStatistics(void)
{
  // show statistics every 5s (5000ms)
  if(millis() - 5000 > showStatisticsTimer)
  {
    showStatisticsTimer = millis();
    // show total samples since program start
    Serial.print("Total samples: ");
    Serial.print(totalSamples);
    Serial.print(". ");
    // average value of samples taken in a single shot
    Serial.print("Average value of ");
    Serial.print(numberOfSamples);
    Serial.print(" samples: ");
    Serial.println(averageAnalogValue);
    // average number of samples per second including pause time
    //Serial.print("Average no. of samples per second: ");
    //Serial.println((numberOfSamples) / (samplingTime * 1000000 + samplingPause * 1000));
  }
}
*/



String getAnalogValue() {
  //float temperature = bme.readTemperature();
  Serial.println(averageAnalogValue);
  return String(averageAnalogValue);
}
/*
String getTemperature() {
  float temperature = bme.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float temperature = 1.8 * bme.readTemperature() + 32;
  Serial.println(temperature);
  return String(temperature);
}
  
String getHumidity() {
  float humidity = bme.readHumidity();
  Serial.println(humidity);
  return String(humidity);
}

String getPressure() {
  float pressure = bme.readPressure()/ 100.0F;
  Serial.println(pressure);
  return String(pressure);
}
*/
// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(ledPin)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.println(ledState);
    return ledState;
  }
  else if (var == "ANALOGSENSOR"){
    return getAnalogValue();
  }
  /*
  else if (var == "TEMPERATURE"){
    return getTemperature();
  }
  else if (var == "HUMIDITY"){
    return getHumidity();
  }
  else if (var == "PRESSURE"){
    return getPressure();
  }
  */
}
 
void setup(){
  Serial.begin(115200); // Default Baud Rate for ESP-01
  pinMode(ledPin, OUTPUT);  // Connect relay to ESP-01's GPIO 2
  digitalWrite(ledPin, LOW);  // set initial status to LOW
  pinMode(A0, INPUT); // analog input

/*
  // Initialize the sensor
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
*/

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP-01 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  
  server.on("/analogsensor", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getAnalogValue().c_str());
  });
  
  /*
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTemperature().c_str());
  });
  
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getHumidity().c_str());
  });
  
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getPressure().c_str());
  });
  */
  
  // Start server
  server.begin();
}
 
void loop(){
  
}
