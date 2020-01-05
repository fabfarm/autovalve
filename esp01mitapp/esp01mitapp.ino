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
Serial.print("Connecting to SSID: ");
Serial.println(ssid);
 
WiFi.begin(ssid, password);
delay(5000);

// These four lines of code assign static IP address to ESP-01. Otherwise, comment out to get automatic IP.
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
  relay_status = LOW;
}
if (request.indexOf("/RELAYOFF") != -1)
{
  Serial.println("Relay is OFF");
  digitalWrite(relayPin, LOW);
  relay_status = HIGH;
}

// Return the response
// send a standard http response header
client.println("HTTP/1.1 200 OK");  // start the web response that is sent to the web browser
// The 200 OK is a HTTP response code and in this case is 200 which means the request is OK
client.println("Content-Type: text/html");  // tell the browser that the response content type is text/html
client.println("Connection: close");
// the connection will be closed after completion of the response
client.println(""); // need to have a space here for response
client.println("<!DOCTYPE HTML>");
//client.println("<html lang="en">"); 
client.println("<html>");
client.println("<head>");
//client.println("<meta charset='utf-8'>");
//client.println("<meta charset="utf-8">");
client.println("<title>Algarve Fab Farm</title>");
client.println("</head>");
client.println("<body>");
client.println("<h2>Algarve Fab Farm</h2>");
// Prints in the computer browser
client.print("<p>Status of the relay: ");

if(relay_status == LOW)
{
  client.println("ON</p>");
}
else
{
  client.println("OFF</p>");
}

client.println("</body>");
client.println("</html>");

// give the web browser time to receive the data
delay(1);

// close the connection:
//client.stop();
Serial.println("Client disconnected");
Serial.println("");
}
