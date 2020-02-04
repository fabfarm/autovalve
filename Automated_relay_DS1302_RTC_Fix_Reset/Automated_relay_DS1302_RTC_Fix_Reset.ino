/*
 * Automated relay control using DS1302 Real Time Clock module
 * version 2.1
 * Changelog:
 * Fixed automatic restart of pump immediately after power outage
 * Fixed serial output to avoid the timing falling behind by 40s
*/

#include <virtuabotixRTC.h> // DS1302 RTC module library

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(5, 6, 7);  // Wiring of the RTC (CLK,DAT,RST)

int setMinutes = 0, setSeconds = 0; // set minutes and seconds for pump activation
long pumpRelayTime = 40000; //set pump operation time in ms. int datatype can only hold max value 32767.
const int pumpRelay = 8;  // set pump relay pin

unsigned long interval = 2000;  // print RTC time every 2s
unsigned long time_now = 0;

void setup() {
  Serial.begin(9600); // open serial port and set the baud rate
  Serial.println("Automated pump relay control with RTC (Fixed Reset)");
  Serial.println("**************************************************");
  pinMode(pumpRelay, OUTPUT); // set digital pin pumpRelay as an output
  
  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(50, 59, 19, 5, 26, 1, 2020); // uncomment line, upload to reset RTC and then comment, upload.
  
  myRTC.updateTime(); // update of variables for time or accessing the individual elements.
  
  // Start printing elements as individuals                                                                 
  Serial.print("Running from preset RTC Date / Time: ");                                                                  
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
  myRTC.updateTime(); // update of variables for time or accessing the individual elements.                                                                                 

  // Start printing RTC time elements as individuals                                                                   
  if (millis() - time_now >= interval)  // non-blocking method. prints RTC time every 2 seconds.
  {
    time_now = millis();
    Serial.print("RTC Time: ");                                                                                                                                                    
    Serial.print(myRTC.hours);                                                                              
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);                                                                            
    Serial.print(":");                                                                                      
    Serial.println(myRTC.seconds);
  }
  if (myRTC.minutes == setMinutes && myRTC.seconds == setSeconds)
  {
    Serial.print("Pump activated at: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module                                                                              
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module                                                                              
    Serial.print(":");                                                                                           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module  
                                                                                   
    digitalWrite(pumpRelay, HIGH);  //turns on the pump relay
        
    Serial.println("*** Pump relay turned ON ***");
    Serial.print("Pump operation time: ");
    Serial.print(pumpRelayTime / 1000); // calculate time of operation in seconds      
    Serial.println(" seconds");
    delay(pumpRelayTime); // pump relay operation time
    digitalWrite(pumpRelay, LOW); // turn off the pump relay
    Serial.println("*** Pump relay turned OFF ***");                                                                                                                    
  }
}
