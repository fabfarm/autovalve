// Arduino based controller for the automated irrigation system
// Using RTC and ACS712 current sensor
// version 1
//--------------------------------------------------------------------------------------------------
#include <virtuabotixRTC.h> // DS1302 RTC module library

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(5, 6, 7);  // Wiring of the RTC (CLK,DAT,RST)

//Defining the time variables used within the code.
//Timer variables
int h = 10;
int mins = 0;
int secs = 0;

//----------------
String newTime = "";    // string to hold serial input
//----------------

boolean valve_1_state = 0;
boolean valve_2_state = 0;
boolean valve_3_state = 0;
boolean valve_4_state = 0;
//boolean pump_state = 0;

//---------------------------------------------------------------------------------------------------
//Programmable times, expressed in hours and minutes

//Valve 1 --- fruit trees on
const int valveRelay1_OnHour = 14;
const int valveRelay1_OnMin = 0;
const int valveRelay1_OffHour = 14;
const int valveRelay1_OffMin = 30;

//Valve 2 --- cypress on
const int valveRelay2_OnHour = 10;
const int valveRelay2_OnMin = 0;
const int valveRelay2_OffHour = 11;
const int valveRelay2_OffMin = 0;

//Valve 3 --- vegetable garden
const int valveRelay3_OnHour = 17;
const int valveRelay3_OnMin = 0;
const int valveRelay3_OffHour = 17;
const int valveRelay3_OffMin = 20;

// pump and relay pins
const int pumpRelay = 8;  // set pump relay pin
const int valveRelay1 = 9;
const int valveRelay2 = 10;
const int valveRelay3 = 11;
const int valveRelay4 = 12;

// ACS712 current sensor
const int ACS_pin = A0;
const int mVperAmp = 100; // Output sensitivity in mV per Amp
// Use scale factor: 185 for 5A module, 100 for 20A module and 66 for 30A module

float VPP = 0.0; // peak-to-peak voltage
float VRMS = 0.0;  // RMS voltage
float IRMS = 0.0; // RMS current

float VRMSoffset = 0.0; //0.025; // set quiescent Vrms output voltage
//voltage at an output terminal with reference to a common terminal, normally ground,
//when no signal is applied to the input.

// not needed
int setMinutes = 0, setSeconds = 0; // set minutes and seconds for pump activation
long pumpRelayTime = 40000; //set pump operation time in ms. int datatype can only hold max value 32767.

unsigned long interval = 2000;  // prints RTC time every 2000 ms or 2 secs
unsigned long time_now = 0;

unsigned long waitTime = 40000; // wait 40s between activation of relay and then pump.
unsigned long timeNow = 0;

void setup() {
  
  pinMode(valveRelay1, OUTPUT);
  pinMode(valveRelay2, OUTPUT);
  pinMode(valveRelay3, OUTPUT);
  pinMode(pumpRelay, OUTPUT);

  digitalWrite(valveRelay1, LOW);
  digitalWrite(valveRelay2, LOW);
  digitalWrite(valveRelay3, LOW);
  digitalWrite(pumpRelay, LOW);
  
  Serial.begin(9600); // open serial port and set the baud rate
  Serial.println("Automated pump relay control with RTC and ACS712 Current Sensor");
  Serial.println("***************************************************************");

  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(50, 59, 19, 5, 26, 1, 2020); // uncomment line, upload to reset RTC and then comment, upload.
  
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
}

void loop() {
  myRTC.updateTime(); // update of variables for time or accessing the individual elements.                                                                                 

  // Start printing RTC time elements as individuals
  /*                                                                   
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
  */
/*
  //printing time to serial monitor for observation
  Serial.println((String)"Time:" + h + " h, " + mins + " m, " + secs + " s");
  // In order to read a serial input and write a new time the function described at
  // https://www.arduino.cc/en/Tutorial.StringToIntExample was used

  // Read serial input:
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      newTime += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      Serial.print("Value entered:");
      Serial.println(newTime.toInt());
      Serial.print("New time is: ");
      Serial.println(newTime);
      Serial.print("hours");
      h = newTime.toInt();
      // clear the string for new input:
      newTime = "";
    }
  }
*/

  if (myRTC.hours == valveRelay1_OnHour && myRTC.minutes == valveRelay1_OnMin)
  {
    digitalWrite(pumpRelay, HIGH);  // turns on the pump relay
    Serial.println("*** Pump Relay turned ON ***");
    
    Serial.print("Activation time: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module                                                                              
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module                                                                              
    Serial.print(":");                                                                                           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module  
    
    //Serial.print("Pump operation time: ");
    //Serial.print(pumpRelayTime / 1000); // calculate time of operation      
    //Serial.println(" minutes");

    // wait 40s then turn pump relay ON
    timeNow = millis();
    Serial.println("Wait 40s before activating Pump Relay 1.");
    //IS THIS NON-BLOCKING??
    while (millis() < timeNow + waitTime)
    {
      //wait approx. 40s
    }
    
  
  }
  else if (myRTC.hours == valveRelay1_OffHour && myRTC.minutes == valveRelay1_OffMin)
  {
      digitalWrite(pumpRelay,LOW);
      Serial.println("*** Pump Relay turned OFF ***");
  }

  //------------------------------------------------------------------------------------
  //to turn valve 1 on
  if (h == valve_1_on_h && mins == valve_1_on_mins) {
    if (secs == 0) {
      digitalWrite(valveRelay1, HIGH);
      Serial.println("\n***valve-1 turned on\n");
      valve_1_state = 1;
    }
    if (secs == 40) {
      digitalWrite(pumpRelay, HIGH);
      Serial.println("\n***pump turned on\n");
    }
  }

  //------------------------------------------------------------------------------------
  //to turn valve 1 off
  if (h == valve_1_off_h && mins == valve_1_off_mins && secs == 0) {
    valve_1_state = 0;

    if ((valve_1_state == 0) && (valve_2_state == 0) && (valve_3_state == 0) && (valve_4_state == 0)) {
      digitalWrite(pumpRelay, LOW);
      Serial.println("\n***pump turned off\n");
    }

    digitalWrite(valveRelay1, LOW);
    Serial.println("\n***valve-1 turned off\n");
  }

  //------------------------------------------------------------------------------------
  //to turn valve 2 on
  if (h == valve_2_on_h && mins == valve_2_on_mins) {
    if (secs == 0) {
      digitalWrite(valveRelay2, HIGH);
      Serial.println("\n***valve-2 turned on\n");
      valve_2_state = 1;
    }
    if (secs == 40) {
      digitalWrite(pumpRelay, HIGH);
      Serial.println("\n***pump turned on\n");
    }
  }

  //------------------------------------------------------------------------------------
  //to turn valve 2 off
  if (h == valve_2_off_h && mins == valve_2_off_mins && secs == 0) {
    valve_2_state = 0;

    if ((valve_1_state == 0) && (valve_2_state == 0) && (valve_3_state == 0) && (valve_4_state == 0)) {
      digitalWrite(pumpRelay, LOW);
      Serial.println("\n***pump turned off\n");
    }

    digitalWrite(valveRelay2, LOW);
    Serial.println("\n***valve-2 turned off\n");
  }

  //------------------------------------------------------------------------------------
  //to turn valve 3 on
  if (h == valve_3_on_h && mins == valve_3_on_mins) {
    if (secs == 0) {
      digitalWrite(valveRelay3, HIGH);
      Serial.println("\n***valve-1 turned on\n");
      valve_3_state = 1;
    }
    if (secs == 40) {
      digitalWrite(pumpRelay, HIGH);
      Serial.println("\n***pump turned on\n");
    }
  }

  //------------------------------------------------------------------------------------
  //to turn valve 3 off
  if (h == valve_3_off_h && mins == valve_3_off_mins && secs == 0) {
    valve_3_state = 0;

    if ((valve_1_state == 0) && (valve_2_state == 0) && (valve_3_state == 0) && (valve_4_state == 0)) {
      digitalWrite(pumpRelay, LOW);
      Serial.println("\n***pump turned off\n");
    }

    digitalWrite(valveRelay3, LOW);
    Serial.println("\n***valve-3 turned off\n");
  }

  //------------------------------------------------------------------------------------
  //to turn valve 4 on
  if (h == valve_4_on_h && mins == valve_4_on_mins) {
    if (secs == 0) {
      digitalWrite(valveRelay4, HIGH);
      Serial.println("\n***valve-4 turned on\n");
      valve_4_state = 1;
    }
    if (secs == 40) {
      digitalWrite(pumpRelay, HIGH);
      Serial.println("\n***pump turned on\n");
    }
  }

  //------------------------------------------------------------------------------------
  //to turn valve 4 off
  if (h == valve_4_off_h && mins == valve_4_off_mins && secs == 0) {
    valve_4_state = 0;

    if ((valve_1_state == 0) && (valve_2_state == 0) && (valve_3_state == 0) && (valve_4_state == 0)) {
      digitalWrite(pumpRelay, LOW);
      Serial.println("\n***pump turned off\n");
    }

    digitalWrite(valveRelay4, LOW);
    Serial.println("\n***valve-4 turned off\n");
  }
 

  VPP = getVPP(); // find peak-to-peak voltage
  VRMS = ((VPP / 2.0) * 0.707) - VRMSoffset; // divide by 2 to get peak voltage. 1 ÷ √(2) is 0.707
  IRMS = (VRMS * 1000.0) / mVperAmp; // first, multiply by 1000 to convert to mV

  Serial.print("Vpp/V: ");
  Serial.print(VPP, 3); // print to 3 decimal places
  Serial.print("\tVrms/V: ");
  Serial.print(VRMS, 3);  // print to 3 decimal places
  Serial.print("\tIrms/A: ");
  Serial.println(IRMS, 3);  // print to 3 decimal places

}

// Function to turn on valve
//void turnValveOn(valve) {
//  if (h == valve_4_on_h && mins == valve_4_on_mins) {
//    if (secs == 0) {
//      digitalWrite(valveRelay4, HIGH);
//      Serial.println("\n***valve-4 turned on\n");
//      valve_4_state = 1;
//    }
//    if (secs == 40) {
//      digitalWrite(pumpRelay, HIGH);
//      Serial.println("\n***pump turned on\n");
//    }
//  }
//}

// function to measure peak-to-peak voltage
float getVPP()  // continously sampling and logging max and min values
{
  float result;
  int readValue; // value from the sensor
  int maxValue = 0; // to store max value, initialised at lowest value.
  int minValue = 1024; // to store min value, initialised at highest value.


  uint32_t start_time = millis();
  while ((millis() - start_time) < 3000) // sample for 3000 ms or 3 secs
  {
    readValue = analogRead(ACS_pin);
    // check if a new maxValue
    if (readValue > maxValue)
    {
      // record the maximum sensor value
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      // record the minimum sensor value
      minValue = readValue;
    }
  }

  // subtract min from max and convert range to volts
  result = ((maxValue - minValue) * 5.0) / 1024.0;
  return result;
}
