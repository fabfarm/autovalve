// Executes a print out example that accesses individual data elements.                    
                                                                                                         
#include <virtuabotixRTC.h> //RTC DS1302 library                                                                            
                                                                                                         
// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(7, 6, 5);  //Wiring of the RTC (CLK,DAT,RST)

void setup()  {      
  Serial.begin(9600); // opens serial port and set standard baud rate.

// Set the current date, and time in the following format:
// seconds, minutes, hours, day of the week, day of the month, month, year
  myRTC.setDS1302Time(00, 54, 11, 2, 17, 12, 2019); //this is set only once.
}
                                                                                                                                                               
void loop()  {                                                                                            
               
  myRTC.updateTime();  // allows update of variables for time or accessing individual elements.                                                                                    
                                                                                                          
// print date and time elements individually                                                                   
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
                                                                                                                                                                     
  delay(2000);  // delay so the program does not print non-stop  
}                                                                                                         
