//Automated Arduino-based pump relay control using DS1302 Real Time Clock Module with Battery Backup
//--------------------------------------------------------------------------------------------------

#include <virtuabotixRTC.h> //DS1302 RTC module library

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(7, 6, 5);  //Wiring of the RTC (CLK,DAT,RST)

int timeDiff, lastReadHour; //declaring variables for time difference and previous time recorded to calculate time elapsed
int pumpRelayTime = 30000; //set pump operation time in ms
int pumpRelay = 8;

void setup() {
  Serial.begin(9600); //open serial port and set the baud rate
  Serial.println("Automated pump relay control (RTC version)");
  
  pinMode(pumpRelay, OUTPUT); // initialise digital pin pumpRelay as an output
  pinMode(LED_BUILTIN, OUTPUT); // initialise digital pin LED_BUILTIN as an output
  
  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(10, 57, 14, 2, 17, 12, 2019);
  myRTC.setDS1302Time(00, 00, 20, 2, 17, 12, 2019); //the seconds and minutes are set to zero as the code executes every hour
  
  myRTC.updateTime(); //update of variables for time or accessing the individual elements.
  
  // Start printing elements as individuals                                                                 
  Serial.print("Current Date: ");                                                                  
  Serial.print(myRTC.dayofmonth);                                                                         
  Serial.print("/");                                                                                      
  Serial.print(myRTC.month);                                                                              
  Serial.print("/");                                                                                      
  Serial.println(myRTC.year);
  Serial.print("Current Hour (24-hour clock): ");                                                                                      
  Serial.println(myRTC.hours);                          
  //Serial.print(":");                                                                                      
  //Serial.print(myRTC.minutes);                                                                            
  //Serial.print(":");                                                                                      
  //Serial.println(myRTC.seconds); 
  
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

      //for debugging
      //Serial.print("Current RTC hour: "); //display the hour from RTC module
      //Serial.println(myRTC.hours);
      Serial.print("Last recorded hour: "); //display the hour from RTC module
      Serial.println(lastReadHour);
      Serial.print("Hour difference: "); //display the hour difference
      Serial.println(timeDiff);
      
      if (timeDiff >= 1) //conditional check if at least one hour difference
      {                                                                                
        digitalWrite(pumpRelay, HIGH);  //turns on the pump relay
        digitalWrite(LED_BUILTIN, HIGH);   //turns on the on-board LED (used for testing)
        Serial.println("*** Pump relay turned ON ***");
        delay(pumpRelayTime); //pump relay operation time
        digitalWrite(pumpRelay, LOW); //turns off the pump relay
        digitalWrite(LED_BUILTIN, LOW);  //turns off the on-board LED (used for testing)
        Serial.println("*** Pump relay turned OFF ***");
        Serial.print("Time of operation (seconds): ");
        Serial.println(pumpRelayTime / 1000);                                                                      
      }
      lastReadHour = myRTC.hours; //stores current hour value as previous hour value                                                   
  }
}
