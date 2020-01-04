#include <ESP8266WiFi.h>
 
const char* ssid = "fabfarm-ele-container"; // enter SSID
const char* password = "imakestuff";  // enter password
 
int relayPin = 2; // GPIO2 of ESP-01
int relay_status = LOW;  // status of relay initialised to LOW/OFF
WiFiServer ESPserver(80); //Service Port
 
void setup() 
{
Serial.begin(115200);
pinMode(relayPin, OUTPUT);
digitalWrite(relayPin, LOW);
 
Serial.println();
Serial.println();
Serial.print("Connecting to: ");
Serial.println(ssid);
 
WiFi.begin(ssid, password);
delay(5000);

// These four lines of code assign static IP Address to ESP-01 
// Otherwise, comment out for automatic IP
// IPAddress ip(192,168,8,233);
// IPAddress gateway(192,168,1,1);
// IPAddress subnet(255,255,255,0);
// WiFi.config(ip, gateway, subnet);

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
Serial.println("New Client");
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
  relay_status = LOW;
}
if (request.indexOf("/RELAYOFF") != -1)
{
  Serial.println("Relay is OFF");
  digitalWrite(relayPin, LOW);
  relay_status = HIGH;
}

// Return the response
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println(""); // need to have a space here for response
client.println("<!DOCTYPE HTML>");
client.println("<html>");

// Prints in the computer browser
client.println("Status of the relay: "); 

if(relay_status == LOW) 
{
  client.print("ON");  
}
else
{
  client.print("OFF");
}

delay(1);
//client.stop();
Serial.println("Client disconnected");
Serial.println("");
}
