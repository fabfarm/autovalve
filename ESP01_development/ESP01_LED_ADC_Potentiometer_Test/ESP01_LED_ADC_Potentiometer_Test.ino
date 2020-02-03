/* 
 * ESP-01 LED WiFi controller (with ADC sensor)
 * version 1.2 beta
 */

#include <ESP8266WiFi.h>

const char* ssid = "fabfarm-ele-container"; // enter Service Set Identifier
const char* password = "imakestuff";  // enter WiFi network password

const int ledPin = 2; // connected to GPIO-2 of ESP-01
const int sensorPin = A0; // connected to ADC pin of ESP-01

int led_status = 0;  // status of LED initialised to 0 or OFF

WiFiServer ESPserver(80); //Service Port

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

void setup() 
{
  
Serial.begin(115200); //Default Baud Rate for ESP-01
pinMode(ledPin, OUTPUT);  // Connect relay to ESP-01's GPIO 2
digitalWrite(ledPin, LOW);  // set initial status to LOW
pinMode(A0, INPUT); // analog input

Serial.println();
Serial.println();
Serial.print("Connecting to WiFi: ");
Serial.println(ssid);
 
WiFi.begin(ssid, password);
delay(5000);

 //The following 4 lines of code assign static IP address to ESP-01. Otherwise, comment out to get automatic IP.
 IPAddress ip(192,168,1,158);
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

// THIS IS THE BLOCKING CODE
while(!client.available())
{
  delay(1);
  //Serial.print("*");
}
//Serial.println("");
//Serial.println("Client sent data");

// Read the first line of the request
String request = client.readStringUntil('\r');
Serial.println(request);
client.flush();

// Match the request
if (request.indexOf("/LEDOFF") != -1)
{
  Serial.println("LED is OFF");
  digitalWrite(ledPin, LOW);
  led_status = 0; // OFF
}
else if (request.indexOf("/LEDON") != -1)
{
  Serial.println("LED is ON");
  digitalWrite(ledPin, HIGH);
  led_status = 1; // ON
}
else
{
  Serial.println("Invalid request");
  led_status = 2; // Invalid request
}

// take analog readings within a certain time and then pause before taking next readings
analogSample();



// show statistics
showStatistics();

// check average sensor threshold values
if (averageAnalogValue <= 200)
{
  Serial.println("Low sensor value alert!");
  digitalWrite(ledPin, LOW); // turn LED off
}
else if (averageAnalogValue >= 800)
{
  Serial.println("High sensor value alert!");
  digitalWrite(ledPin, LOW);  // turn LED off
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
client.println("<title>ESP-01 LED Wi-Fi controller (with analog sensor)</title>");
client.println("</head>");
client.println("<body>");

// Prints text in web browser
client.println("<h2>ESP-01 LED Wi-Fi controller (with analog sensor)</h2>");
client.print("<p>Control LED: ");
client.println("<a href='/LEDON' target='_self'>ON</a>");
client.println("<a href='/LEDOFF' target='_self'>OFF</a></p>");

client.print("<p>Status of LED: ");
if (led_status == 0)
{
  client.println("<font color=\"grey\"><b>OFF</b></font></p>");
}
else if (led_status == 1)
{
  client.println("<font color=\"green\"><b>ON</b></font></p>");
}
else if (led_status == 2)
{
  client.println("Invalid request</p>");
}

client.println("</body>");
client.println("</html>");

// give the web browser time to receive the data
delay(1);

// close the connection
Serial.println("Client disconnected");
Serial.println();
}
