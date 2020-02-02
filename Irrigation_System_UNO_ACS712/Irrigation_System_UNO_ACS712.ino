// Arduino based controller for the automated irrigation system
// Using ACS712 current sensor
// version 1.1
//--------------------------------------------------------------------------------------------------
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
//Programmable times for control events, expressed in hours and minutes from the time0
//Input time for:
//Valve 1 --- fruit trees on
int valve_1_on_h  = 14;
int valve_1_on_mins  = 0;
int valve_1_off_h = 14;
int valve_1_off_mins = 30;
//Valve 2 --- cypress on
int valve_2_on_h  = 10;
int valve_2_on_mins  = 1;
int valve_2_off_h = 11;
int valve_2_off_mins = 0;
//Valve 3 --- vegetable garden
int valve_3_on_h  = 17;
int valve_3_on_mins  = 0;
int valve_3_off_h = 17;
int valve_3_off_mins = 20;
//Valve 4 --- extra
int valve_4_on_h  = 99999;
int valve_4_on_mins  = 99999;
int valve_4_off_h = 99999;
int valve_4_off_mins = 99999;
//--------------------------------------------------------------------------------------------------
//The section below assigns digital Arduino pins to named variables
// for better readability of code
int valveRelay1 = 2;
int valveRelay2 = 3;
int valveRelay3 = 4;
int valveRelay4 = 5;
int pumpRelay   = 8;

//--------------------------------------------------------------------------------------------------
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

#include <virtuabotixRTC.h> // DS1302 RTC module library

// Creation of the Real Time Clock Object
virtuabotixRTC myRTC(5, 6, 7);  // Wiring of the RTC (CLK,DAT,RST)

int setMinutes = 0, setSeconds = 0; // set minutes and seconds for pump activation
long pumpRelayTime = 40000; //set pump operation time in ms. int datatype can only hold max value 32767.
const int pumpRelay = 8;  // set pump relay pin

unsigned long interval = 2000;  // prints RTC time every 2000 ms or 2 secs
unsigned long time_now = 0;


//--------------------------------------------------------------------------------------------------
//The function 'setup()' gets executed only once upon power-up/reset of Arduino
void setup() {
  Serial.begin(9600);           //  setup serial
  Serial.println("Power-on. Serial Monitor initiated!");          // debug value
  pinMode(valveRelay1, OUTPUT); // defining the relayPin as digital output
  pinMode(valveRelay2, OUTPUT);
  pinMode(valveRelay3, OUTPUT);
  pinMode(valveRelay4, OUTPUT);
  pinMode(pumpRelay, OUTPUT);
}

//--------------------------------------------------------------------------------------------------
//The 'setup()' function is done, the 'loop()' function gets executed repeatedly.
void loop() {

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

  //calling function for updating the time variables - secs, mins and hour
  timer();


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

//--------------------------------------------------------------------------------------------------
//This is a function for counting hours and minutes
int timer() {

  //waiting 1000 ms = 1s this is just a freaking comment!!!
  delay(1000);

  // increment hours
  if (h == 23 && mins == 59 && secs == 59) {
    h = 0;
  }
  else if (mins == 59 && secs == 59) {
    h = h + 1;
  }

  // increment minutes
  if (mins == 59 && secs == 59) {
    mins = 0;
  }
  else if (secs == 59) {
    mins = mins + 1;
  }

  // seconds
  if (secs == 59) {
    secs = 0;
  }
  else
    secs = secs + 1;
}

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
