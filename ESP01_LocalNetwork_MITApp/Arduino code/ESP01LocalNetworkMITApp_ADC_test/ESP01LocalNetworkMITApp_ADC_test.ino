/* 
 * Manual WiFi relay controller via local network and MIT Android app using ESP-01
 * version 1.1
 */

#include <ESP8266WiFi.h>

const char* ssid = "fabfarm-ele-container"; // enter Service Set Identifier
const char* password = "imakestuff";  // enter WiFi network password

const int relayPin = 2; // GPIO-2 of ESP-01
const int ACS_pin = A0;
const int mVperAmp = 100; // Output sensitivity in mV per Amp
// Use scale factor: 185 for 5A module, 100 for 20A module and 66 for 30A module

float VPP = 0.0; // peak-to-peak voltage
float VRMS = 0.0;  // RMS voltage
float IRMS = 0.0; // RMS current

float VRMSoffset = 0.0; //0.025; // set quiescent Vrms output voltage
//voltage at an output terminal with reference to a common terminal, normally ground,
//when no signal is applied to the input.

int relay_status = 0;  // status of relay initialised to 0 or OFF.

unsigned long interval = 3000;  // sample for 3000 ms or 3 secs
unsigned long previousMillis = 0;
  
int maxValue = 0; // store max value, initialised at lowest value.
int minValue = 1024; // store min value, initialised at highest value.

WiFiServer ESPserver(80); //Service Port

void setup() 
{
  //********** CHANGE PIN FUNCTION  TO GPIO **********
//pinMode(1, FUNCTION_0); // GPIO 1 (TX) swap the pin to a GPIO.
//pinMode(3, FUNCTION_0); // GPIO 3 (RX) swap the pin to a GPIO.
//**************************************************

Serial.begin(115200); //Default Baud Rate for ESP-01
pinMode(relayPin, OUTPUT);  // Connect relay to ESP-01's GPIO 2
digitalWrite(relayPin, LOW);  // set initial status to LOW
pinMode(A0, INPUT); // analog input
 
Serial.println();
Serial.println();
Serial.print("Connecting to WiFi: ");
Serial.println(ssid);
 
WiFi.begin(ssid, password);
delay(5000);

 //The following 4 lines of code assign static IP address to ESP-01. Otherwise, comment out to get automatic IP.
 IPAddress ip(192,168,8,158);
 IPAddress gateway(192,168,1,1);
 IPAddress subnet(255,255,255,0);
 WiFi.config(ip, gateway, subnet);
 delay(5000);
 
while (WiFi.status() != WL_CONNECTED) 
{
  delay(100);
  Serial.print("*");
}
Serial.println("");
Serial.println("WiFi connected");
 
// Start the server
ESPserver.begin();
Serial.println("Server started");
 
// Print the IP address
Serial.print("The URL to control ESP-01: ");
Serial.print("http://");
Serial.println(WiFi.localIP());
Serial.println("");
}

void loop() 
{
// Check if a client has connected
WiFiClient client = ESPserver.available();
if (!client) 
{
  return;
}

// Wait until the client sends some data
Serial.println("Client connected");
while(!client.available())
{
  delay(1);
}

// Read the first line of the request
String request = client.readStringUntil('\r');
Serial.println(request);
client.flush();

// Match the request
if (request.indexOf("/RELAYON") != -1)
{
  Serial.println("Relay is ON");
  digitalWrite(relayPin, HIGH);
  relay_status = 1; // ON
}
else if (request.indexOf("/RELAYOFF") != -1)
{
  Serial.println("Relay is OFF");
  digitalWrite(relayPin, LOW);
  relay_status = 0; // OFF
}
else
{
  Serial.println("Invalid request");
  relay_status = 2; // Invalid request
  //client.stop();
  //Serial.println("Client disconnected");
  //Serial.println();
  //return;
}

// find the minimum and maximum sensor values within interval and convert range to volts
unsigned long currentMillis = millis(); // get current time

// check if interval time limit has been reached
if ((unsigned long)(currentMillis - previousMillis) < interval)
{
  //  ACS712 current sensor sampling
  sensorValueRange(); // find peak-to-peak sensor values
}
else
{
  // calculations to find peak-to-peak voltage after time interval
  VPP = ((maxValue - minValue) * 5.0) / 1024.0; // subtract min from max and convert range to volts
  VRMS = ((VPP / 2.0) * 0.707) - VRMSoffset; // divide by 2 to get peak voltage. 1 ÷ √(2) is 0.707
  IRMS = (VRMS * 1000.0) / mVperAmp; // first, multiply by 1000 to convert to mV
  Serial.print("Irms/A: ");
  Serial.println(IRMS, 3);  // print to 3 decimal places
  
  previousMillis = millis();
}

// Send a standard http response header
client.println("HTTP/1.1 200 OK");  // start the web response that is sent to the web browser
// The 200 OK is a HTTP response code and in this case is 200 which means the request is OK
client.println("Content-Type: text/html");  // tell the browser that the response content type is text/html
client.println("Connection: close");
// the connection will be closed after completion of the response
client.println(); // need to have a space here after http response header
client.println("<!DOCTYPE HTML>");  // HTML web site template
client.println("<html lang=\"en\">");
client.println("<head>");
client.println("<meta charset=\"utf-8\">");
client.println("<title>Algarve Fab Farm</title>");
client.println("</head>");
client.println("<body>");

// Prints text in web browser
client.println("<h2>Algarve Fab Farm</h2>");
client.print("<p>Control relay: ");
client.println("<a href='/RELAYON' target='_self'>ON</a>");
client.println("<a href='/RELAYOFF' target='_self'>OFF</a></p>");

client.print("<p>Status of the relay: ");
if (relay_status == 1)
{
  client.println("<font color=\"red\"><b>ON</b></font></p>");
}
else if (relay_status == 0)
{
  client.println("<font color=\"blue\"><b>OFF</b></font></p>");
}
else if (relay_status == 2)
{
  client.println("Invalid request</p>");
}

client.println(IRMS,3); // testing to see if it can print current values to web page

client.println("<div style=\"height:200px;width:240px;overflow:auto;background-color:yellowgreen;color:white;scrollbar-base-color:gold;font-family:sans-serif;padding:10px;\">This HTML scroll box has had color added. You can add color to the background of your scroll box. You can also add color to the scroll bars.</div>");

client.println("</body>");
client.println("</html>");

// give the web browser time to receive the data
delay(1);

// close the connection
Serial.println("Client disconnected");
Serial.println();
}

// function to sample the maximum and minimum sensor values within interval
float sensorValueRange()
{
  int readValue; // local variable to store current sensor value
  
    readValue = analogRead(ACS_pin);  // read the current sensor value
    
    // check if there is a new maximum value
    if (readValue > maxValue)
    {
      // record the maximum sensor value
      maxValue = readValue;
    }
    
    // check if there is a new minimum value
    if (readValue < minValue)
    {
      // record the minimum sensor value
      minValue = readValue;
    }
}
