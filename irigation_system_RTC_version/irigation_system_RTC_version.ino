//Automated Arduino-based pump relay control using DS1302 Real Time Clock Module with Battery Backup
//--------------------------------------------------------------------------------------------------

#include <virtuabotixRTC.h> //DS1302 RTC library

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(7, 6, 5);  //Wiring of the RTC (CLK,DAT,RST)

int timeDiff, lastReadHour; //declaring variables for time difference and previous time recorded to calculate time elapsed
int pumpRelayTime = 30000; //pump operation time in ms
int pumpRelay   = 8;

void setup() {
  Serial.begin(9600); //open serial port and set the baud rate
  Serial.println("Power ON. Serial Monitor initiated. RTC version.");
  pinMode(pumpRelay, OUTPUT);
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(10, 57, 14, 2, 17, 12, 2019);
  myRTC.setDS1302Time(00, 00, 20, 2, 17, 12, 2019);
  
  myRTC.updateTime(); //update of variables for time or accessing the individual elements.
  
  // Start printing elements as individuals                                                                 
  Serial.print("Current Date / Time: ");                                                                  
  Serial.print(myRTC.dayofmonth);                                                                         
  Serial.print("/");                                                                                      
  Serial.print(myRTC.month);                                                                              
  Serial.print("/");                                                                                      
  Serial.print(myRTC.year);                                                                               
  Serial.print("  ");                                                                                     
  Serial.print(myRTC.hours);                                                                              
  Serial.print(":");                                                                                      
  Serial.print(myRTC.minutes);                                                                            
  Serial.print(":");                                                                                      
  Serial.println(myRTC.seconds); 
  
  lastReadHour = myRTC.hours - 1; //decrement initial value by 1 to run the pump immediately when the program starts
}

void loop() {
      
  myRTC.updateTime(); // update of variables for time or accessing the individual elements.                                                                                 

/*
// Start printing elements as individuals                                                                   
  Serial.print("Current Date / Time: ");                                                                  
  Serial.print(myRTC.dayofmonth);                                                                         
  Serial.print("/");                                                                                      
  Serial.print(myRTC.month);                                                                              
  Serial.print("/");                                                                                      
  Serial.print(myRTC.year);                                                                               
  Serial.print("  ");                                                                                     
  Serial.print(myRTC.hours);                                                                              
  Serial.print(":");                                                                                      
  Serial.print(myRTC.minutes);                                                                            
  Serial.print(":");                                                                                      
  Serial.println(myRTC.seconds);                                                                          
                                                                                                                                                              
  delay(2000);  // delay of 2 seconds so the program doesn't print non-stop 
*/                                                                                      
  if (myRTC.hours != lastReadHour)  //Checks if the values of two operands are not equal
  {
      timeDiff = myRTC.hours - lastReadHour;  //additional check, but hour difference should already exist                                                              
      timeDiff = abs(timeDiff); //calculates absolute value of hour difference
      
      Serial.print("RTC hour: "); //display the hour from RTC module for debugging
      Serial.println(myRTC.hours);
      Serial.print("Hour difference: "); //display the hour difference for debugging
      Serial.println(timeDiff);
      
    if (timeDiff >= 1) //conditional check if at least one hour difference
    {                                                                                
        digitalWrite(pumpRelay, HIGH);  //turns on the pump relay
        digitalWrite(LED_BUILTIN, HIGH);   //turns on the on-board LED pin on Arduino UNO board, used for testing
        Serial.println("*** Pump turned ON for 40 secs ***");
        delay(pumpRelayTime); //delay of 40 seconds by default
        digitalWrite(pumpRelay, LOW);
        digitalWrite(LED_BUILTIN, LOW);  //turns off the on-board LED pin on Arduino UNO board, used for testing
        Serial.println("*** Pump turned OFF ***");                                                                      
    }                                                                                                
    lastReadHour = myRTC.hours; //stores current hour value as previous hour value                                                                           
  }
}
