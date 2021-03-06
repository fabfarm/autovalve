/* 
 * 
 * ***Currentely on Bore Hole pump***
 * 
 * 
 * Timer-based automatic pump relay control
 * Using Arduino UNO R3 and DS1302 RTC module
 * version 2.1
 * Changelog:
 * - Fixed automatic restart of pump immediately after power outage
 * - Fixed serial output to avoid the timing falling behind by 40s
*/

#include <virtuabotixRTC.h> // DS1302 RTC module library

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(5, 6, 7);  // Wiring of the RTC (CLK,DAT,RST)

const int pumpRelay = 8;  // set pump relay pin
int setMinutes = 0; // set timer minutes for pump activation
int setMinutes1 = 0; // set timer minutes for pump activation
int setMinutes2 = 0; // set timer minutes for pump activation
long pumpOperationTime = 20000; // set pump operation time in ms. Note: int datatype can only hold max value 32767.

unsigned long interval = 2000;  // print RTC time every 2s
unsigned long time_now = 0;

void setup() {
  Serial.begin(9600); // open serial port and set the baud rate
  Serial.println("Automated pump relay control with RTC");
  Serial.println("**************************************");
  pinMode(pumpRelay, OUTPUT); // set digital pin pumpRelay as an output
  
  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  myRTC.setDS1302Time(50, 59, 22, 5, 1, 5, 2020); // uncomment line, upload to reset RTC and then comment, upload.
  
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
  if (millis() - time_now >= interval)  // non-blocking. prints RTC time every 2 seconds.
  {
    time_now = millis();
    Serial.print("RTC Time: ");                                                                  
    Serial.print(myRTC.hours);                                                                              
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);                                                                            
    Serial.print(":");                                                                                      
    Serial.println(myRTC.seconds);
  }
  if (myRTC.minutes == setMinutes && myRTC.seconds <= (pumpOperationTime / 1000))
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
    Serial.print(pumpOperationTime / 1000); // show time of pump operation in seconds      
    Serial.println(" seconds");
    delay(pumpOperationTime); // pump relay operation time
    digitalWrite(pumpRelay, LOW); // turn off the pump relay
    Serial.println("*** Pump relay turned OFF ***");                                            
  }
}
