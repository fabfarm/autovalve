/* 
 * Manual WiFi relay controller via local network and MIT Android app using ESP-01
 * version 1.1
 */

#include <ESP8266WiFi.h>

const char* ssid = "fabfarm-ele-container"; // enter Service Set Identifier
const char* password = "imakestuff";  // enter WiFi network password

int relayPin = 2; // GPIO2 of ESP-01
int relay_status = 0;  // status of relay initialised to 0 or OFF.
WiFiServer ESPserver(80); //Service Port

void setup() 
{
Serial.begin(115200); //Default Baud Rate for ESP-01
pinMode(relayPin, OUTPUT);  // Connect relay to ESP-01's GPIO 2
digitalWrite(relayPin, LOW);  // set initial status to LOW
 
Serial.println();
Serial.println();
Serial.print("Connecting to WiFi: ");
Serial.println(ssid);
 
WiFi.begin(ssid, password);
delay(5000);

// The following 5 lines of code assign static IP address to ESP-01. Otherwise, comment out to get automatic IP.
// IPAddress ip(192,168,8,233);
// IPAddress gateway(192,168,1,1);
// IPAddress subnet(255,255,255,0);
// WiFi.config(ip, gateway, subnet);
//delay(5000);
 
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
  relay_status = 0; // OFF
}
else if (request.indexOf("/RELAYOFF") != -1)
{
  Serial.println("Relay is OFF");
  digitalWrite(relayPin, LOW);
  relay_status = 1; // ON
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
if (relay_status == 0)
{
  client.println("<font color=\"red\"><b>ON</b></font></p>");
}
else if (relay_status == 1)
{
  client.println("<font color=\"blue\"><b>OFF</b></font></p>");
}
else if (relay_status == 2)
{
  client.println("Invalid request</p>");
}
client.println("</body>");
client.println("</html>");

// give the web browser time to receive the data
delay(1);

// close the connection:
//client.stop();
Serial.println("Client disconnected");
Serial.println();
}
