/*
 * Automated relay control using DS1302 Real Time Clock module
 * version 2.0
 * Fixed automatic start of pump immediately after power outage
*/

#include <virtuabotixRTC.h> //DS1302 RTC module library

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(5, 6, 7);  // Wiring of the RTC (CLK,DAT,RST)

int timeDiff, lastReadHour, setMinutes = 0, setSeconds = 0; //declaring variables for time difference and previous time recorded to calculate time elapsed
long pumpRelayTime = 40000; //set pump operation time in ms. int datatype can only hold max value 32767.
int pumpRelay = 8;

unsigned long interval = 2000;  // used to print current time every 2000 ms or 2 secs
unsigned long time_now = 0;

void setup() {
  Serial.begin(9600); //open serial port and set the baud rate
  Serial.println("Automated pump relay control with RTC (version 2)");
  Serial.println("*************************************************");
  pinMode(pumpRelay, OUTPUT); // set digital pin pumpRelay as an output
  pinMode(LED_BUILTIN, OUTPUT); // set digital pin LED_BUILTIN as an output
  
  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(10, 57, 14, 2, 17, 12, 2019);
  //myRTC.setDS1302Time(40, 59, 18, 5, 24, 1, 2020); //the seconds and minutes are set to zero as the code executes every hour
  
  myRTC.updateTime(); //update of variables for time or accessing the individual elements.
  
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
  
  //lastReadHour = myRTC.hours - 1; //decrement initial value by 1 to run the pump immediately when the program starts
}

void loop() {
  myRTC.updateTime(); // update of variables for time or accessing the individual elements.                                                                                 

// Start printing RTC time elements as individuals                                                                   
if (millis() >= time_now + interval)  //non-blocking
{
  time_now += interval;
  //myRTC.updateTime();
  Serial.print("RTC Time: ");                                                                                                                                                    
  Serial.print(myRTC.hours);                                                                              
  Serial.print(":");                                                                                      
  Serial.print(myRTC.minutes);                                                                            
  Serial.print(":");                                                                                      
  Serial.println(myRTC.seconds);                                                                          
}                                                       
                                                                            
  /*
  if (myRTC.hours == setHour)
  {
      timeDiff = myRTC.hours - lastReadHour;  //additional check, but hour difference should already exist                                                              
      timeDiff = abs(timeDiff); //calculates absolute value of hour difference
      
      //for debugging
      //Serial.print("Current RTC hour: "); //display the hour from RTC module
      //Serial.println(myRTC.hours);
      
      Serial.print("Current hour: "); //display the hour from RTC module
      Serial.println(myRTC.hours);
      
      if (timeDiff >= 1) //conditional check if at least one hour difference
      {                                                                                
        digitalWrite(pumpRelay, HIGH);  //turns on the pump relay
        digitalWrite(LED_BUILTIN, HIGH);   //turns on the on-board LED (used for testing)
        Serial.println("*** Pump relay turned ON ***");
        Serial.print("Pump operation time: ");
        Serial.print(pumpRelayTime / 1000);
        Serial.println(" seconds");
        delay(pumpRelayTime); //pump relay operation time
        digitalWrite(pumpRelay, LOW); //turns off the pump relay
        digitalWrite(LED_BUILTIN, LOW);  //turns off the on-board LED (used for testing)
        Serial.println("*** Pump relay turned OFF ***");
        //Serial.print("Time of operation (seconds): ");
        //Serial.println(pumpRelayTime / 1000);                                                                      
      }
      lastReadHour = myRTC.hours; //stores current hour value as previous hour value                                                 
  }
  */
  
  if (myRTC.minutes == setMinutes && myRTC.seconds == setSeconds)
  {
    
      Serial.print("Pump activated at: ");
      Serial.print(myRTC.hours);  // display the current hour from RTC module                                                                              
      Serial.print(":");                                                                                      
      Serial.print(myRTC.minutes);  // display the current minutes from RTC module                                                                              
      Serial.print(":");                                                                                      
      Serial.println(myRTC.seconds);  // display the seconds from RTC module  
                                                                                   
      digitalWrite(pumpRelay, HIGH);  //turns on the pump relay
      digitalWrite(LED_BUILTIN, HIGH);   //turns on the on-board LED (used for testing)
        
      Serial.println("*** Pump relay turned ON ***");
      Serial.print("Pump operation time: ");
      Serial.print(pumpRelayTime / 1000); // calculate time of operation in seconds
      Serial.println(" seconds");
      delay(pumpRelayTime); // pump relay operation time
      digitalWrite(pumpRelay, LOW); // turn off the pump relay
      digitalWrite(LED_BUILTIN, LOW);  // turn off the on-board LED (used for testing only)
      Serial.println("*** Pump relay turned OFF ***");                                                                                                                    
  }
}
