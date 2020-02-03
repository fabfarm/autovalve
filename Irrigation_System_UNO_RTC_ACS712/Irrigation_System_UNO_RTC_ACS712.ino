// Automated pump and valve relays for irrigation system
// Using Arduino UNO, DS1302 RTC and ACS712 current sensor
// version 1.0 alpha

#include <virtuabotixRTC.h> // DS1302 RTC module library

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(5, 6, 7);  // Wiring of the RTC (CLK,DAT,RST)

//---------------------------------------------------------------------------------------------------
// Set ON and OFF timers
//---------------------------------------------------------------------------------------------------
// Timers - fruit trees
const int valveRelay1_OnHour = 15;
const int valveRelay1_OnMin = 44;
const int valveRelay1_OffHour = 15;
const int valveRelay1_OffMin = 46;

// Timers - cypress
const int valveRelay2_OnHour = 10;
const int valveRelay2_OnMin = 0;
const int valveRelay2_OffHour = 11;
const int valveRelay2_OffMin = 0;

// Timers - vegetable garden
const int valveRelay3_OnHour = 17;
const int valveRelay3_OnMin = 0;
const int valveRelay3_OffHour = 17;
const int valveRelay3_OffMin = 20;

//---------------------------------------------------------------------------------------------------
// Set current limit for pump
//---------------------------------------------------------------------------------------------------
float currentLimit = 1.0; // set the maximum current limit (Amps)

// pump and relay pins
const int pumpRelay = 8;
const int valveRelay1 = 9;
const int valveRelay2 = 10;
const int valveRelay3 = 11;

// ACS712 current sensor
const int ACS_pin = A0; // set analog input pin
const int mVperAmp = 100; // Output sensitivity in mV per Amp
// Use scale factor: 185 for 5A module, 100 for 20A module and 66 for 30A module

float VPP = 0.0; // peak-to-peak voltage
float VRMS = 0.0;  // RMS voltage
float IRMS = 0.0; // RMS current

float VRMSoffset = 0.0; //0.025; // set quiescent Vrms output voltage
//voltage at the output terminal with reference to ground when no signal is applied to input.

unsigned long RTCtimeInterval = 3000;  // prints RTC time every time interval
unsigned long RTCtimeNow;

unsigned long currentTimeNow;
unsigned long pumpTimeNow;
unsigned long valveTimeNow;

unsigned long waitTimePump = 40000; // wait 40s to activate pump
unsigned long waitTimeValve = 10000; // wait 10s to deactivate relay
unsigned long waitTimeCurrent = 5000; // wait a few seconds to avoid current spikes

bool valve_1_state = 0; // valve 1 state initialised to OFF
bool valve_2_state = 0; // valve 2 state initialised to OFF
bool valve_3_state = 0; // valve 3 state initialised to OFF
bool pump_state = 0; // pump state initialised to OFF

void setup() {
  pinMode(valveRelay1, OUTPUT);
  pinMode(valveRelay2, OUTPUT);
  pinMode(valveRelay3, OUTPUT);
  pinMode(pumpRelay, OUTPUT);

  // set initial state of all valve and pump relays to OFF
  digitalWrite(valveRelay1, LOW);
  digitalWrite(valveRelay2, LOW);
  digitalWrite(valveRelay3, LOW);
  digitalWrite(pumpRelay, LOW);
  
  Serial.begin(9600); // open serial port and set the baud rate
  Serial.println("Automated irrigation system with RTC and Current Sensor");
  Serial.println("********************************************************");
  Serial.println("Preset RTC-based Timers:");
  Serial.print("Valve Relay 1 Timer ON: ");
  Serial.print(valveRelay1_OnHour);
  Serial.print(":");
  Serial.println(valveRelay1_OnMin);
  Serial.print("Valve Relay 1 Timer OFF: ");
  Serial.print(valveRelay1_OffHour);
  Serial.print(":");
  Serial.println(valveRelay1_OffMin);
  
  Serial.println("********************************************************");
  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(50, 43, 12, 1, 3, 2, 2020); // uncomment line, upload to reset RTC and then comment, upload.
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

void loop() 
{
  myRTC.updateTime(); // update of variables for time or accessing the individual elements.      

  
  //------------------------------------------------------------------------------------
  // Valve Relay 1 control
  //------------------------------------------------------------------------------------
  
  // turn Valve Relay 1 ON
  if (valve_1_state == 0)
  {
    if (myRTC.hours == valveRelay1_OnHour && myRTC.minutes == valveRelay1_OnMin)
    {
    digitalWrite(valveRelay1, HIGH);
    valve_1_state = 1;
    Serial.println("*** Valve Relay 1 turned ON ***");
    Serial.print("Activation RTC time: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module                
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module            
    Serial.print(":");                                                                                           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module  
    // wait 40s then turn pump relay ON
    Serial.println("Wait 40s before activating Pump Relay.");
    delay(40000);
    digitalWrite(pumpRelay, HIGH);
    pump_state = 1;
    Serial.println("*** Pump Relay turned ON ***");
    }
  }
  
  // turn Valve Relay 1 OFF
  if (valve_1_state == 1)
  {
    if (myRTC.hours == valveRelay1_OffHour && myRTC.minutes == valveRelay1_OffMin)
    {
    digitalWrite(pumpRelay, LOW);
    pump_state = 0;
    Serial.println("*** Pump Relay turned OFF ***");
    Serial.print("Deactivation RTC time: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module                
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module            
    Serial.print(":");                                                                                           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module
    // wait 10s then turn valve relay OFF
    Serial.println("Wait 10s before deactivating Valve Relay 1.");
    delay(10000);
    digitalWrite(valveRelay1, LOW);
    valve_1_state = 0;
    Serial.println("*** Valve Relay 1 turned OFF ***");
    }
  }
  
  //------------------------------------------------------------------------------------
  // Valve Relay 2 control
  //------------------------------------------------------------------------------------
  // turn Valve Relay 2 ON
  if (myRTC.hours == valveRelay2_OnHour && myRTC.minutes == valveRelay2_OnMin)
  {
    digitalWrite(valveRelay2, HIGH);  // turns on the pump relay
    Serial.println("*** Valve Relay 2 turned ON ***");
    Serial.print("Activation time: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module        
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module               
    Serial.print(":");                                                                                           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module  
    // wait 40s then turn pump relay ON
    Serial.println("Wait 40s before activating Pump Relay.");
    pumpTimeNow = millis();
    // to check if non-blocking
    while (millis() < pumpTimeNow + waitTimePump)
    {
      //wait 40s
    }
    digitalWrite(pumpRelay, HIGH);
    //pump_state = 1;
    Serial.println("*** Pump Relay turned ON ***");
  }
  // turn Valve Relay 2 OFF
  else if (myRTC.hours == valveRelay2_OffHour && myRTC.minutes == valveRelay2_OffMin)
  {
    digitalWrite(pumpRelay, LOW);
    pump_state = 0;
    Serial.println("*** Pump Relay turned OFF ***");
    // wait 10s then turn valve relay OFF
    Serial.println("Wait 10s before deactivating Valve Relay 2.");
    valveTimeNow = millis();
    // to check if non-blocking
    while (millis() < valveTimeNow + waitTimeValve)
    {
      //wait 10s
    }
    digitalWrite(valveRelay2, LOW);
    Serial.println("*** Valve Relay 2 turned OFF ***");
  }

  //------------------------------------------------------------------------------------
  // Valve Relay 3 control
  //------------------------------------------------------------------------------------
  // turn Valve Relay 3 ON
  if (myRTC.hours == valveRelay3_OnHour && myRTC.minutes == valveRelay3_OnMin)
  {
    digitalWrite(valveRelay3, HIGH);  // turns on the pump relay
    Serial.println("*** Valve Relay 3 turned ON ***");
    Serial.print("Activation time: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module        
    Serial.print(":");                                                           
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module       
    Serial.print(":");               
    Serial.println(myRTC.seconds);  // display the seconds from RTC module  
    // wait 40s then turn pump relay ON
    Serial.println("Wait 40s before activating Pump Relay.");
//    pumpTimeNow = millis();
//    // to check if non-blocking
//    while (millis() < pumpTimeNow + waitTimePump)
//    {
//      //wait 40s
//    }
    delay(10000);
    digitalWrite(pumpRelay, HIGH);
    pump_state = 1;
    Serial.println("*** Pump Relay turned ON ***");
  }
  // turn Valve Relay 3 OFF
  else if (myRTC.hours == valveRelay3_OffHour && myRTC.minutes == valveRelay3_OffMin)
  {
    digitalWrite(pumpRelay, LOW);
    pump_state = 0;
    Serial.println("*** Pump Relay turned OFF ***");
    // wait 10s then turn valve relay OFF
    Serial.println("Wait 10s before deactivating Valve Relay 3.");
    //valveTimeNow = millis();
    // to check if non-blocking
    //while (millis() < valveTimeNow + waitTimeValve)
    //{
      //wait 10s
    //}
    delay(10000);
    digitalWrite(valveRelay3, LOW);
    Serial.println("*** Valve Relay 3 turned OFF ***");
  }

  
  // calculate the IRMS current
  VPP = getVPP(); // find peak-to-peak voltage
  VRMS = ((VPP / 2.0) * 0.707) - VRMSoffset; // divide by 2 to get peak voltage. 1 ÷ √(2) is 0.707
  IRMS = (VRMS * 1000.0) / mVperAmp; // first, multiply by 1000 to convert to mV

  // start monitoring current level
  if (pump_state == 1)
  {
    Serial.println("*** Monitoring current threshold level ***");
    Serial.print("Vpp/V: ");
    Serial.print(VPP, 3); // print to 3 decimal places
    Serial.print("\tVrms/V: ");
    Serial.print(VRMS, 3);  // print to 3 decimal places
    Serial.print("\tIrms/A: ");
    Serial.println(IRMS, 3);  // print to 3 decimal places
    
    if (IRMS >= currentLimit)
    {
      //wait a few seconds to neglect current spikes
      currentTimeNow = millis();
      while (millis() < currentTimeNow + waitTimeCurrent)
      {
        //wait 1s
        Serial.println("Current spike detected! Waiting 1s.");
      }
      //check current level again
      if (IRMS >= currentLimit) // check if current limit is still exceeded after waiting, then turn off pump
      {
      digitalWrite(pumpRelay, LOW);
      pump_state = 0; // stop monitoring current level
      Serial.println("*** Current limit exceeded! Pump Relay turned OFF ***");
      }
    }
  }

  // Start printing RTC time elements as individuals                                                                  
  if (millis() - RTCtimeNow >= RTCtimeInterval)  // non-blocking. prints RTC time every time interval.
  {
    RTCtimeNow = millis();
    Serial.print("RTC Time: ");                     
    Serial.print(myRTC.hours);                                                                              
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);                                                                            
    Serial.print(":");                                                                                      
    Serial.println(myRTC.seconds);                                                                          
  }

}

// function to measure peak-to-peak voltage
float getVPP()  // continously sampling and logging max and min values
{
  float result;
  int readValue; // value from the sensor
  int maxValue = 0; // to store max value, initialised at lowest value.
  int minValue = 1024; // to store min value, initialised at highest value.


  uint32_t start_time = millis();
  while ((millis() - start_time) < 3000) // sample for 3000 ms
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
