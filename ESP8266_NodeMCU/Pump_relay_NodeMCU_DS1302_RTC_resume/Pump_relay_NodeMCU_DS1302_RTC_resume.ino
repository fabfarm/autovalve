/*
 * 
 * ***Resume function needs to be merged*** 
 * 
 * Calculate Hour and Minute difference based on RTC timers
 * Resume pump operation if time interval is valid
 * Using NodeMCU and DS1302 RTC module
 * version 1.0
*/

#include <virtuabotixRTC.h> // DS1302 RTC module library

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(5, 4, 2);  // (D1,D2,D4) for NodeMCU. Wiring of the DS1302 RTC (CLK,DAT,RST)

const int pumpRelay = 14; // D5  // set pump relay pin
unsigned long interval = 3000;  // print RTC time every 3s
unsigned long time_now = 0;

// Set hour and minutes for ON and OFF
int pumpRelay_OnHour = 15;
int pumpRelay_OnMin = 9;
int pumpRelay_OffHour = 15;
int pumpRelay_OffMin = 10;

bool pumpRelay_status = 0; // set initial state of pump relay to OFF

int RTCtimeInMins;
int pumpRelay_ONinMins;
int pumpRelay_OFFinMins;

void setup() {
  Serial.begin(9600); // open serial port and set the baud rate
  Serial.println();
  Serial.println("Automated pump relay control with RTC");
  Serial.println("**************************************");
  pinMode(pumpRelay, OUTPUT); // set digital pin pumpRelay as an output
  
  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  myRTC.setDS1302Time(50, 8, 15, 5, 26, 1, 2020); // uncomment line, upload to reset RTC and then comment, upload.
  
  myRTC.updateTime(); // update of variables for time or accessing the individual elements.
  
  // Start printing elements as individuals                          
  Serial.print("Preset RTC Time and Date: ");   
  Serial.print(myRTC.hours);                          
  Serial.print(":");                                                                                      
  Serial.print(myRTC.minutes);                                                                            
  Serial.print(":");                                                                                      
  Serial.print(myRTC.seconds);
  Serial.print(" ");
  Serial.print(myRTC.dayofmonth);                                                                         
  Serial.print("/");                                                                                      
  Serial.print(myRTC.month);                                                                              
  Serial.print("/"); 
  Serial.println(myRTC.year);
  
  Serial.println("********************************************************");
  Serial.println("User Configuration:");
  Serial.println("--------------------------------------------------------");
  Serial.println("RTC-based Timers (Hours:Minutes)");
  Serial.print("Pump ON: ");
  Serial.print(pumpRelay_OnHour);
  Serial.print(":");
  Serial.println(pumpRelay_OnMin);
  Serial.print("Pump OFF: ");
  Serial.print(pumpRelay_OffHour);
  Serial.print(":");
  Serial.println(pumpRelay_OffMin);
  Serial.println("********************************************************");
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
    Serial.print(myRTC.seconds);
    
    RTCtimeInMins = (myRTC.hours * 60) + myRTC.minutes; // convert the RTC time (hours and minutes) in minutes
    
    // calculate the pump relay in minutes
    pumpRelay_ONinMins = (pumpRelay_OnHour * 60) + pumpRelay_OnMin;
    pumpRelay_OFFinMins = (pumpRelay_OffHour * 60) + pumpRelay_OffMin;
    
    pumpRelay_status = (RTCtimeInMins >= pumpRelay_ONinMins) != (RTCtimeInMins >= pumpRelay_OFFinMins); // Boolean expression
    // Note: "!=" in the above is operating on Boolean values so it is effectively a logical XOR operator. 
    
    Serial.print("\t");
    Serial.print("RTC in Mins: ");
    Serial.print(RTCtimeInMins);
    Serial.print(", ");
    Serial.print("pumpRelay_ONinMins: ");
    Serial.print(pumpRelay_ONinMins);
    Serial.print(", ");
    Serial.print("pumpRelay_OFFinMins: ");
    Serial.print(pumpRelay_OFFinMins);
    Serial.print(", ");
    Serial.print("pumpRelay_status: ");
    Serial.println(pumpRelay_status);
  }


    if (pumpRelay_status == 1)
    {
//    Serial.print("Pump activated at: ");
//    Serial.print(myRTC.hours);  // display the current hour from RTC module
//    Serial.print(":");
//    Serial.print(myRTC.minutes);  // display the current minutes from RTC module 
//    Serial.print(":");
//    Serial.println(myRTC.seconds);  // display the seconds from RTC module  
    digitalWrite(pumpRelay, HIGH);  //turns on the pump relay
//    Serial.println("*** Pump relay is ON ***");
    }
    
    if (pumpRelay_status == 0)
    {
//    Serial.print("Pump deactivated at: ");
//    Serial.print(myRTC.hours);  // display the current hour from RTC module   
//    Serial.print(":");                                                                                      
//    Serial.print(myRTC.minutes);  // display the current minutes from RTC module                         
//    Serial.print(":");           
//    Serial.println(myRTC.seconds);  // display the seconds from RTC module  
      digitalWrite(pumpRelay, LOW); // turn off the pump relay
//    Serial.println("*** Pump relay is OFF ***");
    }
    
    
    // check if pump operation time interval is valid and if pump is OFF, then re-activate pump
    if (pumpRelay_status == 1 && pumpRelay == LOW)
    {
    Serial.print("Pump re-activated at: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module   
    Serial.print(":");      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module 
    Serial.print(":");           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module  
    digitalWrite(pumpRelay, HIGH);  //turns on the pump relay
    Serial.println("*** Pump relay is ON again ***");
    }
  
//    if (pumpRelay_status == 0)
//    {
//    Serial.print("Pump activated at: ");
//    Serial.print(myRTC.hours);  // display the current hour from RTC module   
//    Serial.print(":");                                                                                      
//    Serial.print(myRTC.minutes);  // display the current minutes from RTC module                         
//    Serial.print(":");           
//    Serial.println(myRTC.seconds);  // display the seconds from RTC module  
//    digitalWrite(pumpRelay, HIGH);  //turns on the pump relay
//    Serial.println("*** Pump relay is ON ***");
//    pumpRelay_status = 1;
//    }
//    if (pumpRelay_status == 1)
//    {
//    Serial.print("Pump deactivated at: ");
//    Serial.print(myRTC.hours);  // display the current hour from RTC module   
//    Serial.print(":");                                                                                      
//    Serial.print(myRTC.minutes);  // display the current minutes from RTC module                         
//    Serial.print(":");           
//    Serial.println(myRTC.seconds);  // display the seconds from RTC module  
//    digitalWrite(pumpRelay, LOW); // turn off the pump relay
//    Serial.println("*** Pump relay is OFF ***");
//    pumpRelay_status = 0;
//  }
}
