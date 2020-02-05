// Automated pump and valve relays for irrigation system
// Using Arduino UNO, DS1302 RTC and ACS712 current sensor
// version 1.0

#include <virtuabotixRTC.h> // DS1302 RTC module library

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(5, 6, 7);  // Wiring of the RTC (CLK,DAT,RST)

//---------------------------------------------------------------------------------------------------
// Set ON and OFF timers
//---------------------------------------------------------------------------------------------------
// Timers - fruit trees
const int valveRelay1_OnHour = 17;
const int valveRelay1_OnMin = 3;
const int valveRelay1_OffHour = 17;
const int valveRelay1_OffMin = 5;

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
float CurrentLimit = 1.1; // set the maximum current threshold in Amps

float AC_current; // AC current Irms value
int count = 0; // initialise current spike count to zero

// pump and relay pins
const int pumpRelay = 8;
const int valveRelay1 = 9;
const int valveRelay2 = 10;
const int valveRelay3 = 11;

// ACS712 current sensor
const int ACS712_sensor = A0; // set the analog pin connected to the ACS712 current sensor
const int mVperAmp = 100; // Output sensitivity in mV per Amp
// ACS712 scale factor: 185 for 5A module, 100 for 20A module and 66 for 30A module

float VRMSoffset = 0.0; //0.005; // set quiescent Vrms output voltage
// voltage offset at analog input with reference to ground when no signal applied to the sensor.

unsigned long RTCtimeInterval = 3000;  // prints RTC time every time interval
unsigned long RTCtimeNow;

unsigned long pumpTimeNow;
unsigned long valveTimeNow;

unsigned long waitTimePump = 40000; // wait 40s to activate pump
unsigned long waitTimeValve = 10000; // wait 10s to deactivate relay

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
  Serial.print("Valve Relay 2 Timer ON: ");
  Serial.print(valveRelay2_OnHour);
  Serial.print(":");
  Serial.println(valveRelay2_OnMin);
  Serial.print("Valve Relay 2 Timer OFF: ");
  Serial.print(valveRelay2_OffHour);
  Serial.print(":");
  Serial.println(valveRelay2_OffMin);
  Serial.print("Valve Relay 3 Timer ON: ");
  Serial.print(valveRelay3_OnHour);
  Serial.print(":");
  Serial.println(valveRelay3_OnMin);
  Serial.print("Valve Relay 3 Timer OFF: ");
  Serial.print(valveRelay3_OffHour);
  Serial.print(":");
  Serial.println(valveRelay3_OffMin);
  Serial.println("********************************************************");
  
  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  //myRTC.setDS1302Time(10, 24, 12, 1, 5, 2, 2020); // uncomment line, upload to reset RTC and then comment, upload.
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
    Serial.println("Waiting 40s before activating Pump Relay.");
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
    // wait 10s then turn valve relay 1 OFF
    Serial.println("Waiting 10s before deactivating Valve Relay 1.");
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
  if (valve_2_state == 0)
  {
    if (myRTC.hours == valveRelay2_OnHour && myRTC.minutes == valveRelay2_OnMin)
    {
    digitalWrite(valveRelay2, HIGH);
    valve_2_state = 1;
    Serial.println("*** Valve Relay 2 turned ON ***");
    Serial.print("Activation RTC time: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module                
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module            
    Serial.print(":");                                                                                           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module  
    // wait 40s then turn pump relay ON
    Serial.println("Waiting 40s before activating Pump Relay.");
    delay(40000);
    digitalWrite(pumpRelay, HIGH);
    pump_state = 1;
    Serial.println("*** Pump Relay turned ON ***");
    }
  }
  
  // turn Valve Relay 2 OFF
  if (valve_2_state == 1)
  {
    if (myRTC.hours == valveRelay2_OffHour && myRTC.minutes == valveRelay2_OffMin)
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
    // wait 10s then turn valve relay 2 OFF
    Serial.println("Waiting 10s before deactivating Valve Relay 2.");
    delay(10000);
    digitalWrite(valveRelay2, LOW);
    valve_2_state = 0;
    Serial.println("*** Valve Relay 2 turned OFF ***");
    }
  }

  //------------------------------------------------------------------------------------
  // Valve Relay 3 control
  //------------------------------------------------------------------------------------
  
  // turn Valve Relay 3 ON
  if (valve_3_state == 0)
  {
    if (myRTC.hours == valveRelay3_OnHour && myRTC.minutes == valveRelay3_OnMin)
    {
    digitalWrite(valveRelay3, HIGH);
    valve_3_state = 1;
    Serial.println("*** Valve Relay 3 turned ON ***");
    Serial.print("Activation RTC time: ");
    Serial.print(myRTC.hours);  // display the current hour from RTC module                
    Serial.print(":");                                                                                      
    Serial.print(myRTC.minutes);  // display the current minutes from RTC module            
    Serial.print(":");                                                                                           
    Serial.println(myRTC.seconds);  // display the seconds from RTC module  
    // wait 40s then turn pump relay ON
    Serial.println("Waiting 40s before activating Pump Relay.");
    delay(40000);
    digitalWrite(pumpRelay, HIGH);
    pump_state = 1;
    Serial.println("*** Pump Relay turned ON ***");
    }
  }
  
  // turn Valve Relay 3 OFF
  if (valve_3_state == 1)
  {
    if (myRTC.hours == valveRelay3_OffHour && myRTC.minutes == valveRelay3_OffMin)
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
    // wait 10s then turn valve relay 3 OFF
    Serial.println("Waiting 10s before deactivating Valve Relay 3.");
    delay(10000);
    digitalWrite(valveRelay3, LOW);
    valve_3_state = 0;
    Serial.println("*** Valve Relay 3 turned OFF ***");
    }
  }

  //------------------------------------------------------------------------------------
  // Current threshold monitor
  //------------------------------------------------------------------------------------
  
  if (pump_state == 1) // if pump is on, start monitoring current level
  {
    Serial.println("*** Monitoring current threshold level ***");
    AC_current = getIRMS(); // current sensor value is returned about every 3s
    
  if (AC_current >= CurrentLimit)
  {
    count++; // increment current spike count by 1
    Serial.print("*** Current spike detected! ");
    Serial.print(count);
    Serial.println(" out of 3 ***");
    
    //if current spike detected 3 counts in a row
    if (count == 3) // if current threshold exceeded for about 10s, turn off pump relay
    {
      digitalWrite(pumpRelay, LOW); // turn pump off
      Serial.println("*** Current limit exceeded! Pump Relay turned OFF ***");
      pump_state = 0; // stop monitoring current level
      count = 0; // reset current spike count
    
      // turning off valve relay
      if (valve_1_state == 1)
      {
        // wait 10s then turn valve relay 1 OFF
        Serial.println("Waiting 10s before deactivating Valve Relay 1.");
        delay(10000);
        digitalWrite(valveRelay1, LOW);
        valve_1_state = 0;
        Serial.println("*** Valve Relay 1 turned OFF ***");
      }
      if (valve_2_state == 1)
      {
        // wait 10s then turn valve relay 2 OFF
        Serial.println("Waiting 10s before deactivating Valve Relay 2.");
        delay(10000);
        digitalWrite(valveRelay2, LOW);
        valve_2_state = 0;
        Serial.println("*** Valve Relay 2 turned OFF ***");
      }
      if (valve_3_state == 1)
      {
        // wait 10s then turn valve relay 3 OFF
        Serial.println("Waiting 10s before deactivating Valve Relay 3.");
        delay(10000);
        digitalWrite(valveRelay3, LOW);
        valve_3_state = 0;
        Serial.println("*** Valve Relay 3 turned OFF ***");
      }
    }
  }
  else
  {
    if (count != 0)
    {
      Serial.println("*** Current spike count reset ***");
    }
    count = 0; // reset current spike count
  }
  }
  
  
  //------------------------------------------------------------------------------------
  // Print RTC time at regular interval 
  //------------------------------------------------------------------------------------                                   
  
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

//------------------------------------------------------------------------------------
// Function to measure peak-to-peak voltage and calculate Irms value
//------------------------------------------------------------------------------------ 

float getIRMS()  // continously sampling and logging max and min values
{
  float VPP; // peak-to-peak voltage
  float VRMS;  // RMS voltage
  float IRMS; // RMS current
  int readValue; // value from the sensor
  int maxValue = 0; // to store max value, initialised at lowest value.
  int minValue = 1024; // to store min value, initialised at highest value.

  uint32_t start_time = millis();
  while ((millis() - start_time) < 3000) // sample for 3000 ms or 3 secs
  {
    readValue = analogRead(ACS712_sensor);
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
  VPP = ((maxValue - minValue) * 5.0) / 1024.0; // find peak-to-peak voltage
  VRMS = ((VPP / 2.0) * 0.707) - VRMSoffset; // divide by 2 to get peak voltage. 1 ÷ √(2) is 0.707
  IRMS = (VRMS * 1000.0) / mVperAmp; // first, multiply by 1000 to convert to mV

  Serial.print("Vpp/V: ");
  Serial.print(VPP, 3); // print to 3 decimal places
  Serial.print("\tVrms/V: ");
  Serial.print(VRMS, 3);  // print to 3 decimal places
  Serial.print("\tIrms/A: ");
  Serial.println(IRMS, 3);  // print to 3 decimal places
  
  return IRMS;
}
