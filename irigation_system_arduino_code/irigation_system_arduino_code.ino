//This is the code for the Arduino based control board for the automated irrigation system
// The code triggers output pins on the following time steps:
// time0: arduino gets power
// time1: pump is switched on
// time2: valve1 is open
// time3: valve1 is closed, valve2 is open
// time4: valve2 is closed, valve3 is open
// time5: valve3 is closed, valve4 is open
// time6: valve4 is closed
// time7: pump is switched off
// time8: arduino powered down

// time0 and time8 are therefore inherent, while all the other ones can be programmed below;

//--------------------------------------------------------------------------------------------------
//Defining the time variables used within the code.  
//Timer variables
int mins = 0;
int secs = 0;

boolean valve_1_state = 0;
boolean valve_2_state = 0;
boolean valve_3_state = 0;
boolean valve_4_state = 0;
//boolean pump_state = 0;

//Programmable times for control events, expressed in minutes from the time0 
//Input time for fruit trees on valve 1
int valve_1_on  = 300;
int valve_1_off = 360;
//Input time for cypress on valve 2
int valve_2_on  = 1;
int valve_2_off = 120;
//Input time for vegetable garden on valve 3
int valve_3_on  = 540;
int valve_3_off = 600;
int valve_4_on  = 9999;
int valve_4_off = 9999;

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
  Serial.begin(9600);           //  setup serial
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
  
  //calling function for updating the time variables - secs and mins
  timer();
  
  //printing time to serial monitor for observation
  Serial.println((String)"Time:" + mins + " m, " + secs +" s");
//------------------------------------------------------------------------------------
  if(mins == valve_1_on){
    if (secs == 0){
        digitalWrite(valveRelayPin1, HIGH); 
        Serial.println("\n***valve-1 turned on\n"); 
        valve_1_state = 1;
    }
    if (secs == 40){
        digitalWrite(pumpRelayPin, HIGH);
        Serial.println("\n***pump turned on\n"); 
    }
  }

//------------------------------------------------------------------------------------
  if(mins == valve_1_off){
    if (secs == 0){  
      
      valve_1_state = 0;
      
      if ((valve_1_state == 0) && (valve_2_state == 0) && (valve_3_state == 0) && (valve_4_state == 0)) {
        digitalWrite(pumpRelayPin, LOW);
        Serial.println("\n***pump turned off\n");
      }
       
      digitalWrite(valveRelayPin1, LOW); 
      Serial.println("\n***valve-1 turned off\n"); 
    }
  } 

//------------------------------------------------------------------------------------
  if(mins == valve_2_on){
    if (secs == 0){
        digitalWrite(valveRelayPin2, HIGH); 
        Serial.println("\n***valve-2 turned on\n"); 
        valve_2_state = 1;
    }
    if (secs == 40){
        digitalWrite(pumpRelayPin, HIGH);
        Serial.println("\n***pump turned on\n"); 
    }
  }

//------------------------------------------------------------------------------------
  if(mins == valve_2_off){
    if (secs == 0){  
      
      valve_2_state = 0;
      
      if ((valve_1_state == 0) && (valve_2_state == 0) && (valve_3_state == 0) && (valve_4_state == 0)) {
        digitalWrite(pumpRelayPin, LOW);
        Serial.println("\n***pump turned off\n");
      }
       
      digitalWrite(valveRelayPin2, LOW); 
      Serial.println("\n***valve-2 turned off\n"); 
    }
  }

//------------------------------------------------------------------------------------
  if(mins == valve_3_on){
    if (secs == 0){
        digitalWrite(valveRelayPin3, HIGH); 
        Serial.println("\n***valve-3 turned on\n"); 
        valve_3_state = 1;
    }
    if (secs == 40){
        digitalWrite(pumpRelayPin, HIGH);
        Serial.println("\n***pump turned on\n"); 
    }
  }

//------------------------------------------------------------------------------------
  if(mins == valve_3_off){
    if (secs == 0){  
      
      valve_3_state = 0;
      
      if ((valve_1_state == 0) && (valve_2_state == 0) && (valve_3_state == 0) && (valve_4_state == 0)) {
        digitalWrite(pumpRelayPin, LOW);
        Serial.println("\n***pump turned off\n");
      }
       
      digitalWrite(valveRelayPin3, LOW); 
      Serial.println("\n***valve-3 turned off\n"); 
    }
  } 

//------------------------------------------------------------------------------------
  if(mins == valve_4_on){
    if (secs == 0){
        digitalWrite(valveRelayPin4, HIGH); 
        Serial.println("\n***valve-4 turned on\n"); 
        valve_4_state = 1;
    }
    if (secs == 40){
        digitalWrite(pumpRelayPin, HIGH);
        Serial.println("\n***pump turned on\n"); 
    }
  }

//------------------------------------------------------------------------------------
  if(mins == valve_4_off){
    if (secs == 0){  
      
      valve_4_state = 0;
      
      if ((valve_1_state == 0) && (valve_2_state == 0) && (valve_3_state == 0) && (valve_4_state == 0)) {
        digitalWrite(pumpRelayPin, LOW);
        Serial.println("\n***pump turned off\n");
      }
       
      digitalWrite(valveRelayPin4, LOW); 
      Serial.println("\n***valve-4 turned off\n"); 
    }
  }


}

//--------------------------------------------------------------------------------------------------
//This is a function for counting minutes
int timer(){

  //waiting 10 ms = 1s
  delay(1000);

  //after 59s increment minute counter and reset second counter      
  if(secs == 59){
    mins = mins + 1;
    secs = 0;  
  }
  //every other time, increment second counter   
  else
    secs = secs + 1;      
}
