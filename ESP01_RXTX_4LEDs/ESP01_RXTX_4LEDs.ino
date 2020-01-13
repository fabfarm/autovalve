/* 
 * ESP-01 WiFi 4 LEDs controller via local network
 * version 1.1.3
 * Testing RX and TX as GPIO pins
 */

#include <ESP8266WiFi.h>

const char* ssid = "*****"; // enter Service Set Identifier
const char* password = "*****";  // enter WiFi network password

int led0Pin = 0; // GPIO0 of ESP-01
int led1Pin = 1; // GPIO1 of ESP-01 (TX)
int led2Pin = 2; // GPIO2 of ESP-01
int led3Pin = 3; // GPIO3 of ESP-01 (RX)

int led0_status = 0;  // status of LED 1 initialised to 0 or OFF
int led1_status = 0;  // status of LED 2 initialised to 0 or OFF
int led2_status = 0;  // status of LED 3 initialised to 0 or OFF
int led3_status = 0;  // status of LED 4 initialised to 0 or OFF
int check_request = 0; // invalid request if equal to 1

WiFiServer ESPserver(80); //Service Port

void setup() 
{

//********** CHANGE PIN FUNCTION  TO GPIO **********
pinMode(1, FUNCTION_3); // GPIO 1 (TX) swap the pin to a GPIO.
pinMode(3, FUNCTION_3); // GPIO 3 (RX) swap the pin to a GPIO.
//**************************************************
 
//Serial.begin(115200); // Default Baud Rate for ESP-01
pinMode(led0Pin, OUTPUT);  // set ESP-01's GPIO as output
//digitalWrite(led1Pin, LOW);  // set initial GPIO pin status to LOW
pinMode(led1Pin, OUTPUT);  // set ESP-01's GPIO as output
//digitalWrite(led1Pin, LOW);  // set initial GPIO pin status to LOW
pinMode(led2Pin, OUTPUT);  // set ESP-01's GPIO as output
//digitalWrite(led1Pin, LOW);  // set initial GPIO pin status to LOW
pinMode(led3Pin, OUTPUT);  // set ESP-01's GPIO as output
//digitalWrite(led1Pin, LOW);  // set initial GPIO pin status to LOW

/*
Serial.println();
Serial.println();
Serial.print("Connecting to WiFi: ");
Serial.println(ssid);
*/
WiFi.begin(ssid, password);
delay(5000);

// The following 5 lines of code assign static IP address to ESP-01. Otherwise, comment out to get automatic IP.
 IPAddress ip(192,168,8,233);
 IPAddress gateway(192,168,1,1);
 IPAddress subnet(255,255,255,0);
 WiFi.config(ip, gateway, subnet);
 delay(5000);
 
while (WiFi.status() != WL_CONNECTED) 
{
  delay(100);
  //Serial.print("*");
}
//Serial.println("");
//Serial.println("WiFi connected");
 
// Start the server
ESPserver.begin();
//Serial.println("Server started");
 
// Print the IP address
/*
Serial.print("The URL to control ESP-01: ");
Serial.print("http://");
Serial.println(WiFi.localIP());
Serial.println("");
*/
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
//Serial.println("Client connected");
while(!client.available())
{
  delay(1);
}

// Read the first line of the request
String request = client.readStringUntil('\r');
//Serial.println(request);
client.flush();

// Match the request
if (request.indexOf("/LED0OFF") != -1)
{
  //Serial.println("LED 0 is OFF");
  digitalWrite(led0Pin, LOW);
  led0_status = 0; // OFF
  check_request = 0;  // reset validity of request
}
else if (request.indexOf("/LED0ON") != -1)
{
  //Serial.println("LED 0 is ON");
  digitalWrite(led0Pin, HIGH);
  led0_status = 1; // ON
  check_request = 0;  // reset validity of request
}
else if (request.indexOf("/LED1OFF") != -1)
{
  //Serial.println("LED 1 is OFF");
  digitalWrite(led1Pin, LOW);
  led1_status = 0; // OFF
  check_request = 0;  // reset validity of request
}
else if (request.indexOf("/LED1ON") != -1)
{
  //Serial.println("LED 1 is ON");
  digitalWrite(led1Pin, HIGH);
  led1_status = 1; // ON
  check_request = 0;  // reset validity of request
}
else if (request.indexOf("/LED2OFF") != -1)
{
  //Serial.println("LED 2 is OFF");
  digitalWrite(led2Pin, LOW);
  led2_status = 0; // OFF
  check_request = 0;  // reset validity of request
}
else if (request.indexOf("/LED2ON") != -1)
{
  //Serial.println("LED 2 is ON");
  digitalWrite(led2Pin, HIGH);
  led2_status = 1; // ON
  check_request = 0;  // reset validity of request
}
else if (request.indexOf("/LED3OFF") != -1)
{
  //Serial.println("LED 3 is OFF");
  digitalWrite(led3Pin, LOW);
  led3_status = 0; // OFF
  check_request = 0;  // reset validity of request
}
else if (request.indexOf("/LED3ON") != -1)
{
  //Serial.println("LED 3 is ON");
  digitalWrite(led3Pin, HIGH);
  led3_status = 1; // ON
  check_request = 0;  // reset validity of request
}
else
{
  //Serial.println("Invalid request");
  check_request = 1;  // set request as invalid
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
client.println("<h1>Algarve Fab Farm</h1>");
client.println("<h3>ESP-01 WiFi LED controller</h3>");

// check if request/URL/command is invalid
if (check_request == 1)
{
  client.println("<p><h3><font color=\"red\">Error: invalid request. Enter correct URL or use links below.</font></h3></p>");
}

client.print("<p><b>LED 0 (GPIO 0):</b> ");
client.println("<a href='/LED0ON' target='_self'>ON</a>");
client.println("<a href='/LED0OFF' target='_self'>OFF</a></p>");
client.print("<p>Status of LED 0: ");
if (led0_status == 0)
{
  client.println("<font color=\"grey\"><b>OFF</b></font></p>");
}
else if (led0_status == 1)
{
  client.println("<font color=\"green\"><b>ON</b></font></p>");
}

client.print("<p><b>LED 1 (GPIO 1 / TX):</b> ");
client.println("<a href='/LED1ON' target='_self'>ON</a>");
client.println("<a href='/LED1OFF' target='_self'>OFF</a></p>");
client.print("<p>Status of LED 1: ");
if (led1_status == 0)
{
  client.println("<font color=\"grey\"><b>OFF</b></font></p>");
}
else if (led1_status == 1)
{
  client.println("<font color=\"green\"><b>ON</b></font></p>");
}

client.print("<p><b>LED 2 (GPIO 2):</b> ");
client.println("<a href='/LED2ON' target='_self'>ON</a>");
client.println("<a href='/LED2OFF' target='_self'>OFF</a></p>");
client.print("<p>Status of LED 2: ");
if (led2_status == 0)
{
  client.println("<font color=\"grey\"><b>OFF</b></font></p>");
}
else if (led2_status == 1)
{
  client.println("<font color=\"green\"><b>ON</b></font></p>");
}

client.print("<p><b>LED 3 (GPIO 3 / RX):</b> ");
client.println("<a href='/LED3ON' target='_self'>ON</a>");
client.println("<a href='/LED3OFF' target='_self'>OFF</a></p>");
client.print("<p>Status of LED 3: ");
if (led3_status == 0)
{
  client.println("<font color=\"grey\"><b>OFF</b></font></p>");
}
else if (led3_status == 1)
{
  client.println("<font color=\"green\"><b>ON</b></font></p>");
}

client.println("</body>");
client.println("</html>");

// give the web browser time to receive the data
delay(1);

// close the connection:
//Serial.println("Client disconnected");
//Serial.println();
}
