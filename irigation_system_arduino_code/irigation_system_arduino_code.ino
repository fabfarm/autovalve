//This is the code for the Arduino based control board for the automated irrigation system
//--------------------------------------------------------------------------------------------------
//Defining the time variables used within the code.
//Timer variables
int mins = 0;
int secs = 0;
int h = 10;

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
int valve_2_on_mins  = 0;
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
int valveRelayPin1 = 2;
int valveRelayPin2 = 3;
int valveRelayPin3 = 4;
int valveRelayPin4 = 5;
int pumpRelayPin   = 8;

//--------------------------------------------------------------------------------------------------
//The function 'setup()' gets executed only once upon power-up/reset of Arduino
void setup() {
  Serial.begin(115200);           //  setup serial
  Serial.println("Power-on. Serial Monitor initiated!");          // debug value
  pinMode(valveRelayPin1, OUTPUT); // defining the relayPin as digital output
  pinMode(valveRelayPin2, OUTPUT);
  pinMode(valveRelayPin3, OUTPUT);
  pinMode(valveRelayPin4, OUTPUT);
  pinMode(pumpRelayPin, OUTPUT);
}

//--------------------------------------------------------------------------------------------------
//The 'setup()' function is done, the 'loop()' function gets executed repeatedly.
void loop() {

  //calling function for updating the time variables - secs, mins and hour
  timer();

  //printing time to serial monitor for observation
  Serial.println((String)"Time:" + h + " h, " + mins + " m, " + secs + " s");




  //------------------------------------------------------------------------------------
  //to turn valve 1 on
  if (h == valve_1_on_h) {
    if (mins == valve_1_on_mins) {
      if (secs == 0) {
        digitalWrite(valveRelayPin1, HIGH);
        Serial.println("\n***valve-1 turned on\n");
        valve_1_state = 1;
      }
      if (secs == 40) {
        digitalWrite(pumpRelayPin, HIGH);
        Serial.println("\n***pump turned on\n");
      }
    }
  }

  //------------------------------------------------------------------------------------
  //to turn valve 1 off
  if (h == valve_1_off_h) {
    if (mins == valve_1_off_mins) {
      if (secs == 0) {

        valve_1_state = 0;

        if ((valve_1_state == 0) && (valve_2_state == 0) && (valve_3_state == 0) && (valve_4_state == 0)) {
          digitalWrite(pumpRelayPin, LOW);
          Serial.println("\n***pump turned off\n");
        }

        digitalWrite(valveRelayPin1, LOW);
        Serial.println("\n***valve-1 turned off\n");
      }
    }
  }

  //------------------------------------------------------------------------------------
  //to turn valve 2 on
  if (h == valve_2_on_h) {
    if (mins == valve_2_on_mins) {
      if (secs == 0) {
        digitalWrite(valveRelayPin1, HIGH);
        Serial.println("\n***valve-2 turned on\n");
        valve_2_state = 1;
      }
      if (secs == 40) {
        digitalWrite(pumpRelayPin, HIGH);
        Serial.println("\n***pump turned on\n");
      }
    }
  }

  //------------------------------------------------------------------------------------
  //to turn valve 2 off
  if (h == valve_2_off_h) {
    if (mins == valve_2_off_mins) {
      if (secs == 0) {

        valve_2_state = 0;

        if ((valve_1_state == 0) && (valve_2_state == 0) && (valve_3_state == 0) && (valve_4_state == 0)) {
          digitalWrite(pumpRelayPin, LOW);
          Serial.println("\n***pump turned off\n");
        }

        digitalWrite(valveRelayPin1, LOW);
        Serial.println("\n***valve-2 turned off\n");
      }
    }
  }
  //------------------------------------------------------------------------------------
  //to turn valve 3 on
  if (h == valve_3_on_h) {
    if (mins == valve_3_on_mins) {
      if (secs == 0) {
        digitalWrite(valveRelayPin1, HIGH);
        Serial.println("\n***valve-1 turned on\n");
        valve_3_state = 1;
      }
      if (secs == 40) {
        digitalWrite(pumpRelayPin, HIGH);
        Serial.println("\n***pump turned on\n");
      }
    }
  }

  //------------------------------------------------------------------------------------
  //to turn valve 3 off
  if (h == valve_3_off_h) {
    if (mins == valve_3_off_mins) {
      if (secs == 0) {

        valve_3_state = 0;

        if ((valve_1_state == 0) && (valve_2_state == 0) && (valve_3_state == 0) && (valve_4_state == 0)) {
          digitalWrite(pumpRelayPin, LOW);
          Serial.println("\n***pump turned off\n");
        }

        digitalWrite(valveRelayPin1, LOW);
        Serial.println("\n***valve-3 turned off\n");
      }
    }
  }
  //------------------------------------------------------------------------------------
  //to turn valve 4 on
  if (h == valve_4_on_h) {
    if (mins == valve_4_on_mins) {
      if (secs == 0) {
        digitalWrite(valveRelayPin1, HIGH);
        Serial.println("\n***valve-4 turned on\n");
        valve_4_state = 1;
      }
      if (secs == 40) {
        digitalWrite(pumpRelayPin, HIGH);
        Serial.println("\n***pump turned on\n");
      }
    }
  }

  //------------------------------------------------------------------------------------
  //to turn valve 4 off
  if (h == valve_4_off_h) {
    if (mins == valve_4_off_mins) {
      if (secs == 0) {

        valve_4_state = 0;

        if ((valve_1_state == 0) && (valve_2_state == 0) && (valve_3_state == 0) && (valve_4_state == 0)) {
          digitalWrite(pumpRelayPin, LOW);
          Serial.println("\n***pump turned off\n");
        }

        digitalWrite(valveRelayPin1, LOW);
        Serial.println("\n***valve-4 turned off\n");
      }
    }
  }



}

//--------------------------------------------------------------------------------------------------
//This is a function for counting hours and minutes
int timer() {

  //waiting 1000 ms = 1s this is just a freaking comment!!!
  delay(1000);

  //after 59m increment hour counter and reset min counter
  if (mins == 59 && secs == 59) {
    h = h + 1;
    mins = 0;
    secs = -1;
  }

  //after 59s increment minute counter and reset second counter
  if (secs == 59) {
    mins = mins + 1;
    secs = 0;
  }
  //every other time, increment second counter
  else
    secs = secs + 1;
}
