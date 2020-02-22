// Irrigation control system
// Using NodeMCU v1.0 12-E, DS1302 RTC and ACS712 current sensor
// version 1.4
// user-configurable timers and current thresholds to control 3 valves with automatic current sensing when pump is active.

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <Hash.h> // Arduino Cryptography Library
#include <ESPAsyncWebServer.h> // Async Web Server for ESP8266
#include <FS.h> // SPIFFS library
#include <virtuabotixRTC.h> // DS1302 RTC module library

const char* ssid     = "NodeMCU-Irrigation-AP";
const char* password = "iplantstuff";

AsyncWebServer server(80);

const char* PARAM_INT1 = "valveRelay1_OnHour";
const char* PARAM_INT2 = "valveRelay1_OnMin";
const char* PARAM_INT3 = "valveRelay1_OffHour";
const char* PARAM_INT4 = "valveRelay1_OffMin";

const char* PARAM_INT5 = "valveRelay2_OnHour";
const char* PARAM_INT6 = "valveRelay2_OnMin";
const char* PARAM_INT7 = "valveRelay2_OffHour";
const char* PARAM_INT8 = "valveRelay2_OffMin";

const char* PARAM_INT9 = "valveRelay3_OnHour";
const char* PARAM_INT10 = "valveRelay3_OnMin";
const char* PARAM_INT11 = "valveRelay3_OffHour";
const char* PARAM_INT12 = "valveRelay3_OffMin";

const char* PARAM_FLOAT1 = "LowCurrentLimit";
const char* PARAM_FLOAT2 = "HighCurrentLimit";

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(5, 4, 2);  // (D1,D2,D4) for NodeMCU. Wiring of the DS1302 RTC (CLK,DAT,RST)

// Declaring variables for valve relays and initialising to zero
// Timers - fruit trees
int valveRelay1_OnHour = 0;
int valveRelay1_OnMin = 0;
int valveRelay1_OffHour = 0;
int valveRelay1_OffMin = 0;

// Timers - cypress
int valveRelay2_OnHour = 0;
int valveRelay2_OnMin = 0;
int valveRelay2_OffHour = 0;
int valveRelay2_OffMin = 0;

// Timers - vegetable garden
int valveRelay3_OnHour = 0;
int valveRelay3_OnMin = 0;
int valveRelay3_OffHour = 0;
int valveRelay3_OffMin = 0;

// Declaring variables for lower and upper current limits for pump and initialising to zero
float LowCurrentLimit = 0; // set the minimum current threshold in Amps
float HighCurrentLimit = 0; // set the maximum current threshold in Amps

// Set wait times for pump and valve relays activation/deactivation
unsigned long waitTimePumpOn = 5000; // wait time (ms) from relay activation to pump activation
unsigned long waitTimeValveOff = 1000; // wait time (ms) from pump deactivation to relay deactivation

// NodeMCU ESP8266 pump and relay GPIO pins 
const int pumpRelay = 14; // D5
const int valveRelay1 = 12; // D6
const int valveRelay2 = 13; // D7
const int valveRelay3 = 15; // D8

// ACS712 current sensor
#define NUMBER_OF_SAMPLES 200 // number of samples taken in a single shot
const int ACS712_sensor = A0; // set analog pin connected to the ACS712 current sensor
const int mVperAmp = 100; // Output sensitivity in mV per Amp
// ACS712 datasheet: scale factor is 185 for 5A module, 100 for 20A module and 66 for 30A module

float VRMSoffset = 0.0; //0.005; // set quiescent Vrms output voltage
// voltage offset at analog input with reference to ground when no signal applied to the sensor.

float AC_current; // measured AC current Irms value (Amps)
int count = 0; // initialise current limit count to zero
unsigned long MinTimePumpOperation = 0; // minimum time (ms) for pump operation (due to immediate low or high current)

unsigned long RTCtimeInterval = 3000;  // prints RTC time every interval (ms)
unsigned long RTCtimeNow;

unsigned long configTimeInterval = 10000; // set interval in ms
unsigned long configTimeNow = 0; // stores current value from millis()

bool valve_1_state = 0; // valve 1 state initialised to OFF
bool valve_2_state = 0; // valve 2 state initialised to OFF
bool valve_3_state = 0; // valve 3 state initialised to OFF
bool pump_state = 0; // pump state initialised to OFF
bool LowCurrentLimit_state = 0; // initialise low current limit state (0 = normal, 1 = low)
bool HighCurrentLimit_state = 0; // initialise high current limit state (0 = normal, 1 = high)

// HTML web page to handle input fields
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>NodeMCU v1.0 12-E Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saved value to NodeMCU SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
  </script></head><body>
  <h1>Algarve Fab Farm</h1>
  <h3>Irrigation System Configuration</h3>
  
  <p><b>Set Relay 1 Timer (Hours & Minutes):</b></p>
  <form action="/get" target="hidden-form">
    ON hour (saved value: %valveRelay1_OnHour%): <input type="number" name="valveRelay1_OnHour" maxlength="2" size="2" min="0" max="23">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    ON mins (saved value: %valveRelay1_OnMin%): <input type="number" name="valveRelay1_OnMin" maxlength="2" size="2" min="0" max="59">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    OFF hour (saved value: %valveRelay1_OffHour%): <input type="number" name="valveRelay1_OffHour" maxlength="2" size="2" min="0" max="23">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    OFF mins (saved value: %valveRelay1_OffMin%): <input type="number" name="valveRelay1_OffMin" maxlength="2" size="2" min="0" max="59">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>
  
  <p><b>Set Relay 2 Timer (Hours & Minutes):</b></p>
  <form action="/get" target="hidden-form">
    ON hour (saved value: %valveRelay2_OnHour%): <input type="number" name="valveRelay2_OnHour" maxlength="2" size="2" min="0" max="23">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    ON mins (saved value: %valveRelay2_OnMin%): <input type="number" name="valveRelay2_OnMin" maxlength="2" size="2" min="0" max="59">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    OFF hour (saved value: %valveRelay2_OffHour%): <input type="number" name="valveRelay2_OffHour" maxlength="2" size="2" min="0" max="23">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    OFF mins (saved value: %valveRelay2_OffMin%): <input type="number" name="valveRelay2_OffMin" maxlength="2" size="2" min="0" max="59">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>

  <p><b>Set Relay 3 Timer (Hours & Minutes):</b></p>
  <form action="/get" target="hidden-form">
    ON hour (saved value: %valveRelay3_OnHour%): <input type="number" name="valveRelay3_OnHour" maxlength="2" size="2" min="0" max="23">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    ON mins (saved value: %valveRelay3_OnMin%): <input type="number" name="valveRelay3_OnMin" maxlength="2" size="2" min="0" max="59">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    OFF hour (saved value: %valveRelay3_OffHour%): <input type="number" name="valveRelay3_OffHour" maxlength="2" size="2" min="0" max="23">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    OFF mins (saved value: %valveRelay3_OffMin%): <input type="number" name="valveRelay3_OffMin" maxlength="2" size="2" min="0" max="59">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>
  
  <p><b>Set Current Thresholds (Amps):</b></p>
  <form action="/get" target="hidden-form">
    Low (saved value: %LowCurrentLimit%): <input type="number" name="LowCurrentLimit" maxlength="5" size="5" min="0" max="99" step="0.01">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    High (saved value: %HighCurrentLimit%): <input type="number" name="HighCurrentLimit" maxlength="5" size="5" min="0" max="99" step="0.01">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>
  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char * path){
  //Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  //Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  //Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

// Replaces placeholder with saved values
String processor(const String& var){
  //Serial.println(var);
  if(var == "valveRelay1_OnHour"){
    return readFile(SPIFFS, "/valveRelay1_OnHour.txt");
  }
  else if(var == "valveRelay1_OnMin"){
    return readFile(SPIFFS, "/valveRelay1_OnMin.txt");
  }
  else if(var == "valveRelay1_OffHour"){
    return readFile(SPIFFS, "/valveRelay1_OffHour.txt");
  }
  else if(var == "valveRelay1_OffMin"){
    return readFile(SPIFFS, "/valveRelay1_OffMin.txt");
  }
  
  else if(var == "valveRelay2_OnHour"){
    return readFile(SPIFFS, "/valveRelay2_OnHour.txt");
  }
  else if(var == "valveRelay2_OnMin"){
    return readFile(SPIFFS, "/valveRelay2_OnMin.txt");
  }
  else if(var == "valveRelay2_OffHour"){
    return readFile(SPIFFS, "/valveRelay2_OffHour.txt");
  }
  else if(var == "valveRelay2_OffMin"){
    return readFile(SPIFFS, "/valveRelay2_OffMin.txt");
  }
  
  else if(var == "valveRelay3_OnHour"){
    return readFile(SPIFFS, "/valveRelay3_OnHour.txt");
  }
  else if(var == "valveRelay3_OnMin"){
    return readFile(SPIFFS, "/valveRelay3_OnMin.txt");
  }
  else if(var == "valveRelay3_OffHour"){
    return readFile(SPIFFS, "/valveRelay3_OffHour.txt");
  }
  else if(var == "valveRelay3_OffMin"){
    return readFile(SPIFFS, "/valveRelay3_OffMin.txt");
  }
  
  else if(var == "LowCurrentLimit"){
    return readFile(SPIFFS, "/LowCurrentLimit.txt");
  }
  else if(var == "HighCurrentLimit"){
    return readFile(SPIFFS, "/HighCurrentLimit.txt");
  }
  // return the RTC time
  else if(var == "RTChour"){
    return readFile(SPIFFS, "/HighCurrentLimit.txt");
  }
  return String();
}

void setup() {
  Serial.begin(9600);
  // Initialize SPIFFS
    if(!SPIFFS.begin()){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  
  Serial.println();
  Serial.println("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
  
  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET valveRelay1_OnHour value on <ESP_IP>/get?valveRelay1_OnHour=<inputMessage>
    if (request->hasParam(PARAM_INT1)) {
      inputMessage = request->getParam(PARAM_INT1)->value();
      writeFile(SPIFFS, "/valveRelay1_OnHour.txt", inputMessage.c_str());
    }
    // GET valveRelay1_OnMin value on <ESP_IP>/get?valveRelay1_OnMin=<inputMessage>
    else if (request->hasParam(PARAM_INT2)) {
      inputMessage = request->getParam(PARAM_INT2)->value();
      writeFile(SPIFFS, "/valveRelay1_OnMin.txt", inputMessage.c_str());
    }
    // GET valveRelay1_OffHour value on <ESP_IP>/get?valveRelay1_OffHour=<inputMessage>
    else if (request->hasParam(PARAM_INT3)) {
      inputMessage = request->getParam(PARAM_INT3)->value();
      writeFile(SPIFFS, "/valveRelay1_OffHour.txt", inputMessage.c_str());
    }
    // GET valveRelay1_OffMin value on <ESP_IP>/get?valveRelay1_OffMin=<inputMessage>
    else if (request->hasParam(PARAM_INT4)) {
      inputMessage = request->getParam(PARAM_INT4)->value();
      writeFile(SPIFFS, "/valveRelay1_OffMin.txt", inputMessage.c_str());
    }

    // GET valveRelay2_OnHour value on <ESP_IP>/get?valveRelay2_OnHour=<inputMessage>
    else if (request->hasParam(PARAM_INT5)) {
      inputMessage = request->getParam(PARAM_INT5)->value();
      writeFile(SPIFFS, "/valveRelay2_OnHour.txt", inputMessage.c_str());
    }
    // GET valveRelay2_OnMin value on <ESP_IP>/get?valveRelay2_OnMin=<inputMessage>
    else if (request->hasParam(PARAM_INT6)) {
      inputMessage = request->getParam(PARAM_INT6)->value();
      writeFile(SPIFFS, "/valveRelay2_OnMin.txt", inputMessage.c_str());
    }
    // GET valveRelay2_OffHour value on <ESP_IP>/get?valveRelay2_OffHour=<inputMessage>
    else if (request->hasParam(PARAM_INT7)) {
      inputMessage = request->getParam(PARAM_INT7)->value();
      writeFile(SPIFFS, "/valveRelay2_OffHour.txt", inputMessage.c_str());
    }
    // GET valveRelay2_OffMin value on <ESP_IP>/get?valveRelay2_OffMin=<inputMessage>
    else if (request->hasParam(PARAM_INT8)) {
      inputMessage = request->getParam(PARAM_INT8)->value();
      writeFile(SPIFFS, "/valveRelay2_OffMin.txt", inputMessage.c_str());
    }

    // GET valveRelay3_OnHour value on <ESP_IP>/get?valveRelay3_OnHour=<inputMessage>
    else if (request->hasParam(PARAM_INT9)) {
      inputMessage = request->getParam(PARAM_INT9)->value();
      writeFile(SPIFFS, "/valveRelay3_OnHour.txt", inputMessage.c_str());
    }
    // GET valveRelay3_OnMin value on <ESP_IP>/get?valveRelay3_OnMin=<inputMessage>
    else if (request->hasParam(PARAM_INT10)) {
      inputMessage = request->getParam(PARAM_INT10)->value();
      writeFile(SPIFFS, "/valveRelay3_OnMin.txt", inputMessage.c_str());
    }
    // GET valveRelay3_OffHour value on <ESP_IP>/get?valveRelay3_OffHour=<inputMessage>
    else if (request->hasParam(PARAM_INT11)) {
      inputMessage = request->getParam(PARAM_INT11)->value();
      writeFile(SPIFFS, "/valveRelay3_OffHour.txt", inputMessage.c_str());
    }
    // GET valveRelay3_OffMin value on <ESP_IP>/get?valveRelay3_OffMin=<inputMessage>
    else if (request->hasParam(PARAM_INT12)) {
      inputMessage = request->getParam(PARAM_INT12)->value();
      writeFile(SPIFFS, "/valveRelay3_OffMin.txt", inputMessage.c_str());
    }
    
    // GET LowCurrentLimit value on <ESP_IP>/get?LowCurrentLimit=<inputMessage>
    else if (request->hasParam(PARAM_FLOAT1)) {
      inputMessage = request->getParam(PARAM_FLOAT1)->value();
      writeFile(SPIFFS, "/LowCurrentLimit.txt", inputMessage.c_str());
    }
    // GET HighCurrentLimit value on <ESP_IP>/get?HighCurrentLimit=<inputMessage>
    else if (request->hasParam(PARAM_FLOAT2)) {
      inputMessage = request->getParam(PARAM_FLOAT2)->value();
      writeFile(SPIFFS, "/HighCurrentLimit.txt", inputMessage.c_str());
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/text", inputMessage);
  });
  server.onNotFound(notFound);
  server.begin();
  
  pinMode(valveRelay1, OUTPUT);
  pinMode(valveRelay2, OUTPUT);
  pinMode(valveRelay3, OUTPUT);
  pinMode(pumpRelay, OUTPUT);

  // set initial state of all valve and pump relays to OFF
  digitalWrite(valveRelay1, LOW);
  digitalWrite(valveRelay2, LOW);
  digitalWrite(valveRelay3, LOW);
  digitalWrite(pumpRelay, LOW);
  
  Serial.println();
  Serial.println("NodeMCU Irrigation Control System");
  Serial.println("**********************************");
  
  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(10, 7, 20, 1, 5, 2, 2020); // uncomment line, upload to reset RTC and then comment, upload.
  myRTC.updateTime(); //update of variables for time or accessing the individual elements.
  
  // Start printing elements as individuals                                                                 
  Serial.print("Preset RTC Date and Time: ");                                                                  
  Serial.print(myRTC.dayofmonth);                                                                         
  Serial.print("/");                                                                                      
  Serial.print(myRTC.month);                                                                              
  Serial.print("/");                                                                                      
  Serial.print(myRTC.year);
  Serial.print(" ");                                                                                      
  Serial.print(myRTC.hours);                          
  Serial.print(":");                                                                                      
  Serial.print(myRTC.minutes);                                                                            
  Serial.print(":");                                                                                      
  Serial.println(myRTC.seconds);
}

void loop() {

  if (millis() - configTimeNow >= configTimeInterval)  // non-blocking
  {
  configTimeNow = millis();
  // Read saved values on NodeMCU SPIFFS
  valveRelay1_OnHour = readFile(SPIFFS, "/valveRelay1_OnHour.txt").toInt();
  valveRelay1_OnMin = readFile(SPIFFS, "/valveRelay1_OnMin.txt").toInt();
  valveRelay1_OffHour = readFile(SPIFFS, "/valveRelay1_OffHour.txt").toInt();
  valveRelay1_OffMin = readFile(SPIFFS, "/valveRelay1_OffMin.txt").toInt();

  valveRelay2_OnHour = readFile(SPIFFS, "/valveRelay2_OnHour.txt").toInt();
  valveRelay2_OnMin = readFile(SPIFFS, "/valveRelay2_OnMin.txt").toInt();
  valveRelay2_OffHour = readFile(SPIFFS, "/valveRelay2_OffHour.txt").toInt();
  valveRelay2_OffMin = readFile(SPIFFS, "/valveRelay2_OffMin.txt").toInt();

  valveRelay3_OnHour = readFile(SPIFFS, "/valveRelay3_OnHour.txt").toInt();
  valveRelay3_OnMin = readFile(SPIFFS, "/valveRelay3_OnMin.txt").toInt();
  valveRelay3_OffHour = readFile(SPIFFS, "/valveRelay3_OffHour.txt").toInt();
  valveRelay3_OffMin = readFile(SPIFFS, "/valveRelay3_OffMin.txt").toInt();
  
  LowCurrentLimit = readFile(SPIFFS, "/LowCurrentLimit.txt").toFloat();
  HighCurrentLimit = readFile(SPIFFS, "/HighCurrentLimit.txt").toFloat();
  
  Serial.println("********************************************************");
  Serial.println("Current User Configuration:");
  Serial.println("--------------------------------------------------------");
  Serial.println("RTC-based Timers (Hours:Minutes)");
  Serial.print("Valve Relay 1 ON: ");
  Serial.print(valveRelay1_OnHour);
  Serial.print(":");
  Serial.println(valveRelay1_OnMin);
  Serial.print("Valve Relay 1 OFF: ");
  Serial.print(valveRelay1_OffHour);
  Serial.print(":");
  Serial.println(valveRelay1_OffMin);
  Serial.print("Valve Relay 2 ON: ");
  Serial.print(valveRelay2_OnHour);
  Serial.print(":");
  Serial.println(valveRelay2_OnMin);
  Serial.print("Valve Relay 2 OFF: ");
  Serial.print(valveRelay2_OffHour);
  Serial.print(":");
  Serial.println(valveRelay2_OffMin);
  Serial.print("Valve Relay 3 ON: ");
  Serial.print(valveRelay3_OnHour);
  Serial.print(":");
  Serial.println(valveRelay3_OnMin);
  Serial.print("Valve Relay 3 OFF: ");
  Serial.print(valveRelay3_OffHour);
  Serial.print(":");
  Serial.println(valveRelay3_OffMin);
  Serial.println("--------------------------------------------------------");
  Serial.print("Low Current Limit: ");
  Serial.print(LowCurrentLimit, 2);
  Serial.println(" Amps");
  Serial.print("High Current Limit: ");
  Serial.print(HighCurrentLimit, 2);
  Serial.println(" Amps");
  Serial.println("********************************************************");
  }
  
  myRTC.updateTime(); // update of variables for time or accessing the individual elements.      
  
  //------------------------------------------------------------------------------------
  // Valve Relay 1 timer control
  //------------------------------------------------------------------------------------
  
  // check valve status and timer to turn Valve Relay 1 ON
  if (valve_1_state == 0)
  {
    if (myRTC.hours == valveRelay1_OnHour && myRTC.minutes == valveRelay1_OnMin && myRTC.seconds < ((waitTimePumpOn + waitTimeValveOff  + MinTimePumpOperation) / 1000)) // secs less than 6
    {
    Serial.print("Valve Relay 1 Activation sequence RTC time: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module                
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module            
    Serial.print(":");                                                                                           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module
    digitalWrite(valveRelay1, HIGH);
    valve_1_state = 1;
    Serial.println("*** Valve Relay 1 turned ON ***");
    // wait then turn pump relay ON
    Serial.print("Waiting ");
    Serial.print(waitTimePumpOn / 1000);
    Serial.println("s before activating Pump Relay.");
    delay(waitTimePumpOn);
    digitalWrite(pumpRelay, HIGH);
    pump_state = 1;
    Serial.println("*** Pump Relay turned ON ***");
    }
  }
  
  // check valve status and timer to turn Valve Relay 1 OFF
  if (valve_1_state == 1)
  {
    if (myRTC.hours == valveRelay1_OffHour && myRTC.minutes == valveRelay1_OffMin)
    {
    Serial.print("Valve Relay 1 Deactivation sequence RTC time: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module                
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module            
    Serial.print(":");                                                                                           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module
    digitalWrite(pumpRelay, LOW);
    pump_state = 0;
    Serial.println("*** Pump Relay turned OFF ***");
    // wait then turn valve relay 1 OFF
    Serial.print("Waiting ");
    Serial.print(waitTimeValveOff / 1000);
    Serial.println("s before deactivating Valve Relay 1.");
    delay(waitTimeValveOff);
    digitalWrite(valveRelay1, LOW);
    valve_1_state = 0;
    Serial.println("*** Valve Relay 1 turned OFF ***");
    }
  }
  
  //------------------------------------------------------------------------------------
  // Valve Relay 2 timer control
  //------------------------------------------------------------------------------------
  
  // check valve status and timer to turn Valve Relay 2 ON
  if (valve_2_state == 0)
  {
    if (myRTC.hours == valveRelay2_OnHour && myRTC.minutes == valveRelay2_OnMin && myRTC.seconds < ((waitTimePumpOn + waitTimeValveOff + MinTimePumpOperation) / 1000))
    {
    Serial.print("Valve Relay 2 Activation sequence RTC time: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module                
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module            
    Serial.print(":");                                                                                           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module  
    digitalWrite(valveRelay2, HIGH);
    valve_2_state = 1;
    Serial.println("*** Valve Relay 2 turned ON ***");
    // wait then turn pump relay ON
    Serial.print("Waiting ");
    Serial.print(waitTimePumpOn / 1000);
    Serial.println("s before activating Pump Relay.");
    delay(waitTimePumpOn);
    digitalWrite(pumpRelay, HIGH);
    pump_state = 1;
    Serial.println("*** Pump Relay turned ON ***");
    }
  }
  
  // check valve status and timer to turn Valve Relay 2 OFF
  if (valve_2_state == 1)
  {
    if (myRTC.hours == valveRelay2_OffHour && myRTC.minutes == valveRelay2_OffMin)
    {
    Serial.print("Valve Relay 2 Deactivation sequence RTC time: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module                
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module            
    Serial.print(":");                                                                                           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module
    digitalWrite(pumpRelay, LOW);
    pump_state = 0;
    Serial.println("*** Pump Relay turned OFF ***");
    // wait then turn valve relay 2 OFF
    Serial.print("Waiting ");
    Serial.print(waitTimeValveOff / 1000);
    Serial.println("s before deactivating Valve Relay 2.");
    delay(waitTimeValveOff);
    digitalWrite(valveRelay2, LOW);
    valve_2_state = 0;
    Serial.println("*** Valve Relay 2 turned OFF ***");
    }
  }

  //------------------------------------------------------------------------------------
  // Valve Relay 3 timer control
  //------------------------------------------------------------------------------------
  
  // check valve status and timer to turn Valve Relay 3 ON
  if (valve_3_state == 0)
  {
    if (myRTC.hours == valveRelay3_OnHour && myRTC.minutes == valveRelay3_OnMin && myRTC.seconds < ((waitTimePumpOn + waitTimeValveOff + MinTimePumpOperation) / 1000))
    {
    Serial.print("Valve Relay 3 Activation sequence RTC time: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module                
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module            
    Serial.print(":");                                                                                           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module 
    digitalWrite(valveRelay3, HIGH);
    valve_3_state = 1;
    Serial.println("*** Valve Relay 3 turned ON ***"); 
    // wait then turn pump relay ON
    Serial.print("Waiting ");
    Serial.print(waitTimePumpOn / 1000);
    Serial.println("s before activating Pump Relay.");
    delay(waitTimePumpOn);
    digitalWrite(pumpRelay, HIGH);
    pump_state = 1;
    Serial.println("*** Pump Relay turned ON ***");
    }
  }
  
  // check valve status and timer to turn Valve Relay 3 OFF
  if (valve_3_state == 1)
  {
    if (myRTC.hours == valveRelay3_OffHour && myRTC.minutes == valveRelay3_OffMin)
    {
    Serial.print("Valve Relay 3 Deactivation sequence RTC time: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module                
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module            
    Serial.print(":");                                                                                           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module
    digitalWrite(pumpRelay, LOW);
    pump_state = 0;
    Serial.println("*** Pump Relay turned OFF ***");
    // wait then turn valve relay 3 OFF
    Serial.print("Waiting ");
    Serial.print(waitTimeValveOff / 1000);
    Serial.println("s before deactivating Valve Relay 3.");
    delay(waitTimeValveOff);
    digitalWrite(valveRelay3, LOW);
    valve_3_state = 0;
    Serial.println("*** Valve Relay 3 turned OFF ***");
    }
  }
  
  //------------------------------------------------------------------------------------
  // Current threshold monitor
  //------------------------------------------------------------------------------------
  
  if (pump_state == 1) // if pump is on, start monitoring current level
  {
    //Serial.println("*** Monitoring current threshold level ***");
    AC_current = getIRMS(); // read current sensor value, returned about every 3s
  
  // check low current threshold
  if (AC_current <= LowCurrentLimit)
  {
    LowCurrentLimit_state = 1;
    count++; // increment current limit count by 1
    Serial.print("*** Low current detected! ");
    Serial.print(count);
    Serial.println(" out of 3 ***");
    
    //if low current limit detected 3 counts in a row
    if (count == 3) // if current threshold low for about 10s, turn off pump relay
    {
      digitalWrite(pumpRelay, LOW); // turn pump off
      Serial.println("*** Low current limit! Pump Relay turned OFF ***");
      pump_state = 0; // stop monitoring current level
      count = 0; // reset current limit count
      
      // turning off valve relay
      if (valve_1_state == 1)
      {
        // wait then turn valve relay 1 OFF
        Serial.print("Waiting ");
        Serial.print(waitTimeValveOff / 1000);
        Serial.println("s before deactivating Valve Relay 1.");
        delay(waitTimeValveOff);
        digitalWrite(valveRelay1, LOW);
        valve_1_state = 0;
        Serial.println("*** Valve Relay 1 turned OFF ***");
      }
      if (valve_2_state == 1)
      {
        // wait then turn valve relay 2 OFF
        Serial.print("Waiting ");
        Serial.print(waitTimeValveOff / 1000);
        Serial.println("s before deactivating Valve Relay 2.");
        delay(waitTimeValveOff);
        digitalWrite(valveRelay2, LOW);
        valve_2_state = 0;
        Serial.println("*** Valve Relay 2 turned OFF ***");
      }
      if (valve_3_state == 1)
      {
        // wait then turn valve relay 3 OFF
        Serial.print("Waiting ");
        Serial.print(waitTimeValveOff / 1000);
        Serial.println("s before deactivating Valve Relay 3.");
        delay(waitTimeValveOff);
        digitalWrite(valveRelay3, LOW);
        valve_3_state = 0;
        Serial.println("*** Valve Relay 3 turned OFF ***");
      }
    }
  }

  // check high current threshold
  if (AC_current >= HighCurrentLimit)
  {
    HighCurrentLimit_state = 1;
    count++; // increment current limit count by 1
    Serial.print("*** High current detected! ");
    Serial.print(count);
    Serial.println(" out of 3 ***");
    
    //if current limit detected 3 counts in a row
    if (count == 3) // if current threshold exceeded for about 10s, turn off pump relay
    {
      digitalWrite(pumpRelay, LOW); // turn pump off
      Serial.println("*** High current limit! Pump Relay turned OFF ***");
      pump_state = 0; // stop monitoring current level
      count = 0; // reset current limit count
    
      // turning off valve relay
      if (valve_1_state == 1)
      {
        // wait then turn valve relay 1 OFF
        Serial.print("Waiting ");
        Serial.print(waitTimeValveOff / 1000);
        Serial.println("s before deactivating Valve Relay 1.");
        delay(waitTimeValveOff);
        digitalWrite(valveRelay1, LOW);
        valve_1_state = 0;
        Serial.println("*** Valve Relay 1 turned OFF ***");
      }
      if (valve_2_state == 1)
      {
        // wait then turn valve relay 2 OFF
        Serial.print("Waiting ");
        Serial.print(waitTimeValveOff / 1000);
        Serial.println("s before deactivating Valve Relay 2.");
        delay(waitTimeValveOff);
        digitalWrite(valveRelay2, LOW);
        valve_2_state = 0;
        Serial.println("*** Valve Relay 2 turned OFF ***");
      }
      if (valve_3_state == 1)
      {
        // wait then turn valve relay 3 OFF
        Serial.print("Waiting ");
        Serial.print(waitTimeValveOff / 1000);
        Serial.println("s before deactivating Valve Relay 3.");
        delay(waitTimeValveOff);
        digitalWrite(valveRelay3, LOW);
        valve_3_state = 0;
        Serial.println("*** Valve Relay 3 turned OFF ***");
      }
    }
  }

  // if AC current within acceptable limits, reset current limit count
  if (AC_current < HighCurrentLimit && AC_current > LowCurrentLimit)
  {
    if (count != 0 && LowCurrentLimit_state == 1)
    {
      Serial.println("*** Low current limit count reset ***");
      LowCurrentLimit_state = 0;
    }
    if (count != 0 && HighCurrentLimit_state == 1)
    {
      Serial.println("*** High current limit count reset ***");
      HighCurrentLimit_state = 0;
    }
    count = 0; // reset current limit count
 }
}
  
  
  //------------------------------------------------------------------------------------
  // Print RTC time at regular interval 
  //------------------------------------------------------------------------------------                                   
  
  if (millis() - RTCtimeNow >= RTCtimeInterval)  // non-blocking. prints RTC time every time interval.
  {
    RTCtimeNow = millis();
    Serial.print("RTC Time: ");                     
    Serial.print(myRTC.hours);                                                                              
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);                                                                            
    Serial.print(":");                                                                                      
    Serial.println(myRTC.seconds);                                                                          
  }
}

//------------------------------------------------------------------------------------
// Function to measure peak-to-peak voltage and calculate Irms value
//------------------------------------------------------------------------------------ 

float getIRMS()  // continously sampling max and min values
{
  float VPP; // peak-to-peak voltage
  float VRMS;  // RMS voltage
  float IRMS; // RMS current
  int readValue; // value from the sensor
  int maxValue = 0; // store max value, initialised at lowest value.
  int minValue = 1024; // store min value, initialised at highest value.
  int count = 0; // reset count
  
  // repeat 100 times for more accurate max and min readings
  for (int j = 0; j < 100; j++)
  {
    // start continuous sampling
    for (int i = 0; i < NUMBER_OF_SAMPLES; i++)
    {
      readValue = analogRead(ACS712_sensor);
      // check if there is a new maximum and minimum value
      if (readValue > maxValue)
      {
        maxValue = readValue; // record the new maximum sensor value
      }
      else if (readValue < minValue)
      {
        minValue = readValue; // record the new minimum sensor value
      }
    }
    delay(3); // pause for 3 ms
    count++;
  }
  // subtract min from max and convert range to volts
  VPP = ((maxValue - minValue) * 5.0) / 1024.0; // find peak-to-peak voltage
  VRMS = ((VPP / 2.0) * 0.707) - VRMSoffset; // divide by 2 to get peak voltage. 1 ÷ √(2) is 0.707
  IRMS = (VRMS * 1000.0) / mVperAmp; // first, multiply by 1000 to convert to mV

  Serial.print("Vpp/V: ");
  Serial.print(VPP, 3); // print to 3 decimal places
  Serial.print("\tVrms/V: ");
  Serial.print(VRMS, 3);
  Serial.print("\tIrms/A: ");
  Serial.println(IRMS, 3);
  
  return IRMS;
}
