// Automated pump and valve relays for irrigation system
// Using Arduino UNO, DS1302 RTC and ACS712 current sensor
// version 1.1
//Changelog: Added low and high current limits

#include <virtuabotixRTC.h> // DS1302 RTC module library

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(5, 6, 7);  // Wiring of the RTC (CLK,DAT,RST)

//***************************************************************************************************
// User Configuration:
//---------------------------------------------------------------------------------------------------
// Set ON and OFF times for valve relays
//---------------------------------------------------------------------------------------------------
// Timers - fruit trees
const int valveRelay1_OnHour = 16;
const int valveRelay1_OnMin = 28;
const int valveRelay1_OffHour = 16;
const int valveRelay1_OffMin = 30;

// Timers - cypress
const int valveRelay2_OnHour = 21;
const int valveRelay2_OnMin = 47;
const int valveRelay2_OffHour = 21;
const int valveRelay2_OffMin = 49;

// Timers - vegetable garden
const int valveRelay3_OnHour = 17;
const int valveRelay3_OnMin = 0;
const int valveRelay3_OffHour = 17;
const int valveRelay3_OffMin = 20;

//---------------------------------------------------------------------------------------------------
// Set lower and upper current limits for pump
//---------------------------------------------------------------------------------------------------
float LowCurrentLimit = 0.4; // set the minimum current threshold in Amps
float HighCurrentLimit = 0.7; // set the maximum current threshold in Amps

//---------------------------------------------------------------------------------------------------
// Set wait times for pump and valve relays activation/deactivation
//---------------------------------------------------------------------------------------------------
unsigned long waitTimePumpOn = 5000; // wait time (ms) from relay activation to pump activation
unsigned long waitTimeValveOff = 1000; // wait time (ms) from pump deactivation to relay deactivation

//***************************************************************************************************

/////////////////// GPIO pins and current sensor sensitivity ////////////////////

// pump and relay pins
const int pumpRelay = 8;
const int valveRelay1 = 9;
const int valveRelay2 = 10;
const int valveRelay3 = 11;

// ACS712 current sensor
const int ACS712_sensor = A0; // set analog pin connected to the ACS712 current sensor
const int mVperAmp = 100; // Output sensitivity in mV per Amp
// ACS712 datasheet: scale factor is 185 for 5A module, 100 for 20A module and 66 for 30A module

/////////////////// Do Not Change Below This Line ////////////////////

float VRMSoffset = 0.0; //0.005; // set quiescent Vrms output voltage
// voltage offset at analog input with reference to ground when no signal applied to the sensor.

float AC_current; // AC current Irms value (Amps)
int count = 0; // initialise current limit count to zero
unsigned long MinTimePumpOperation = 8000; // minimum time (ms) for pump operation (due to immediate low or high current) is greater than 8s.

unsigned long RTCtimeInterval = 3000;  // prints RTC time every interval (ms)
unsigned long RTCtimeNow;

bool valve_1_state = 0; // valve 1 state initialised to OFF
bool valve_2_state = 0; // valve 2 state initialised to OFF
bool valve_3_state = 0; // valve 3 state initialised to OFF
bool pump_state = 0; // pump state initialised to OFF
bool LowCurrentLimit_state = 0; // initialise low current limit state (0 = normal, 1 = low)
bool HighCurrentLimit_state = 0; // initialise high current limit state (0 = normal, 1 = high)

// =====================================

#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <SPIFFS.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <Hash.h>
  #include <FS.h>
#endif
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "fabfarm-ele-container";
const char* password = "imakestuff";

const char* PARAM_INT1 = "inputHour1";
const char* PARAM_INT2 = "inputMinute1";

// HTML web page to handle 2 input fields (inputHour1, inputMinute1)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Algarve Fab Farm</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saved value to ESP8266 SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
  </script>
  </head><body> 
  
  <h1>Algarve Fab Farm</h1>
  <h3>NodeMCU ESP8266 relays controller</h3>
  <p><b>Set Timer for relay 1:</b></p>
  <form action="/get" target="hidden-form">
    inputHour1 (current value: %inputHour1%): <input type="number " name="inputHour1">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    inputMinute1 (current value: %inputMinute1%): <input type="number " name="inputMinute1">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form>
  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  Serial.println(fileContent);
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

// Replaces placeholder with stored values
String processor(const String& var){
  //Serial.println(var);
  if(var == "inputHour1"){
    return readFile(SPIFFS, "/inputHour1.txt");
  }
  else if(var == "inputMinute1"){
    return readFile(SPIFFS, "/inputMinute1.txt");
  }
  return String();
}

// =====================================

void setup() {
  pinMode(valveRelay1, OUTPUT);
  pinMode(valveRelay2, OUTPUT);
  pinMode(valveRelay3, OUTPUT);
  pinMode(pumpRelay, OUTPUT);

  // set initial state of all valve and pump relays to OFF
  digitalWrite(valveRelay1, LOW);
  digitalWrite(valveRelay2, LOW);
  digitalWrite(valveRelay3, LOW);
  digitalWrite(pumpRelay, LOW);
  
  Serial.begin(115200); // open serial port and set the baud rate
  Serial.println("Automated irrigation system with RTC and Current Sensor");
  Serial.println("********************************************************");
  Serial.println("User Configuration:");
  Serial.println("========================================================");
  Serial.println("RTC-based Valve Relay Timers (Hours:Minutes)");
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
  Serial.print(LowCurrentLimit);
  Serial.println(" Amps");
  Serial.print("High Current Limit: ");
  Serial.print(HighCurrentLimit);
  Serial.println(" Amps");
  Serial.println("========================================================");
  
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

  //Serial.begin(115200);
  // Initialize SPIFFS
  #ifdef ESP32
    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #else
    if(!SPIFFS.begin()){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    
    // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
    if (request->hasParam(PARAM_INT1)) {
      inputMessage = request->getParam(PARAM_INT1)->value();
      writeFile(SPIFFS, "/inputHour1.txt", inputMessage.c_str());
    }
    
    // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
    else if (request->hasParam(PARAM_INT2)) {
      inputMessage = request->getParam(PARAM_INT2)->value();
      writeFile(SPIFFS, "/inputMinute1.txt", inputMessage.c_str());
    }
    
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/text", inputMessage);
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() 
{
  myRTC.updateTime(); // update of variables for time or accessing the individual elements.      
  
  //------------------------------------------------------------------------------------
  // Valve Relay 1 timer control
  //------------------------------------------------------------------------------------
  
  // check valve status and timer to turn Valve Relay 1 ON
  if (valve_1_state == 0)
  {
    if (myRTC.hours == valveRelay1_OnHour && myRTC.minutes == valveRelay1_OnMin && myRTC.seconds <= ((waitTimePumpOn + waitTimeValveOff + MinTimePumpOperation) / 1000))
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
    if (myRTC.hours == valveRelay2_OnHour && myRTC.minutes == valveRelay2_OnMin && myRTC.seconds <= ((waitTimePumpOn + waitTimeValveOff + MinTimePumpOperation) / 1000))
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
    if (myRTC.hours == valveRelay3_OnHour && myRTC.minutes == valveRelay3_OnMin && myRTC.seconds <= ((waitTimePumpOn + waitTimeValveOff + MinTimePumpOperation) / 1000))
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

    // To access stored values in the text files inputIntHour1, inputIntMinute1, etc.
  int yourinputHour1 = readFile(SPIFFS, "/inputHour1.txt").toInt();
  Serial.print("inputHour1: ");
  Serial.print(yourinputHour1);

  int yourInputMinute1 = readFile(SPIFFS, "/inputMinute1.txt").toInt();
  Serial.print(" and inputMinute1: ");
  Serial.println(yourInputMinute1);
                                                                              
  }
}

//------------------------------------------------------------------------------------
// Function to measure peak-to-peak voltage and calculate Irms value
//------------------------------------------------------------------------------------ 

float getIRMS()  // continously sampling and logging max and min values
{
  float VPP; // peak-to-peak voltage
  float VRMS;  // RMS voltage
  float IRMS; // RMS current
  int readValue; // value from the sensor
  int maxValue = 0; // to store max value, initialised at lowest value.
  int minValue = 1024; // to store min value, initialised at highest value.

  uint32_t start_time = millis();
  while ((millis() - start_time) < 3000) // sample for 3000 ms or 3 secs
  {
    readValue = analogRead(ACS712_sensor);
    // check if a new maximum value
    if (readValue > maxValue)
    {
      // record the new maximum value
      maxValue = readValue;
    }
    // check if a new minimum value
    if (readValue < minValue)
    {
      // record the new minimum value
      minValue = readValue;
    }
  }

  // subtract min from max and convert range to volts
  VPP = ((maxValue - minValue) * 5.0) / 1024.0; // find peak-to-peak voltage
  VRMS = ((VPP / 2.0) * 0.707) - VRMSoffset; // divide by 2 to get peak voltage. 1 ÷ √(2) is 0.707
  IRMS = (VRMS * 1000.0) / mVperAmp; // first, multiply by 1000 to convert to mV

  Serial.print("Vpp/V: ");
  Serial.print(VPP, 3); // print to 3 decimal places
  Serial.print("\tVrms/V: ");
  Serial.print(VRMS, 3);  // print to 3 decimal places
  Serial.print("\tIrms/A: ");
  Serial.println(IRMS, 3);  // print to 3 decimal places
  
  return IRMS;
}
