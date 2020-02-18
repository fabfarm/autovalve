// Automated pump and valve relays for irrigation system
// Using NodeMCU v1.0 ESP8266, DS1302 RTC and ACS712 current sensor
// version 1.1
//Changelog: Added low and high current limits

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <Hash.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>

#include <virtuabotixRTC.h> // DS1302 RTC module library

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(5, 6, 7);  // Wiring of the RTC (CLK,DAT,RST)

//****************************************
// User Configuration:
//----------------------------------------
// Set ON and OFF times for valve relays
//----------------------------------------
const int valveRelay1_OnHour = 16;
const int valveRelay1_OnMin = 28;

//relay pins
const int valveRelay1 = 9;

unsigned long RTCtimeInterval = 3000;  // prints RTC time every interval (ms)
unsigned long RTCtimeNow;

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
  
  <h1>NodeMCU device control panel</h1>
  <h3>NodeMCU ESP8266 relays controller</h3>
  <p><b>Set Timer relay 1:</b></p>
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

  // set initial state of valve relay to OFF
  digitalWrite(valveRelay1, LOW);
  
  //Serial.begin(115200); // open serial port and set the baud rate
  //Serial.begin(9600);
//  Serial.println("User Configuration:");
//  Serial.println("========================================================");
//  Serial.println("RTC-based Valve Relay Timer (Hours:Minutes)");
//  Serial.print("Valve Relay 1 ON: ");
//  Serial.print(valveRelay1_OnHour);
//  Serial.print(":");
//  Serial.println(valveRelay1_OnMin);
  
  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(10, 7, 20, 1, 5, 2, 2020); // uncomment line, upload to reset RTC and then comment, upload.
//  myRTC.updateTime(); //update of variables for time or accessing the individual elements.
//  
//  // Start printing elements as individuals                                                                 
//  Serial.print("Preset RTC Date and Time: ");                                                                  
//  Serial.print(myRTC.dayofmonth);                                                                         
//  Serial.print("/");                                                                                      
//  Serial.print(myRTC.month);                                                                              
//  Serial.print("/");                                                                                      
//  Serial.print(myRTC.year);
//  Serial.print(" ");                                                                                      
//  Serial.print(myRTC.hours);                          
//  Serial.print(":");                                                                                      
//  Serial.print(myRTC.minutes);                                                                            
//  Serial.print(":");                                                                                      
//  Serial.println(myRTC.seconds);

  Serial.begin(115200);
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
