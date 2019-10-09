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

//Programmable times for control events, expressed in minutes from the time0 
int time1  = 1;
int time2  = 2;
int time3  = 3;
int time4  = 4;
int time5  = 5;
int time6  = 6;
int time7  = 7;


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

  //When the second counter is 0, check the for the following cases
  if (secs == 0){
    if(mins == time1){
          digitalWrite(pumpRelayPin, HIGH);
          Serial.println("\n pump turned on\n");
    }
    
    if(mins == time2){
          digitalWrite(valveRelayPin1, HIGH); 
          Serial.println("\n***valve-1 turned on\n");
    }
    
    if(mins == time3){
          digitalWrite(valveRelayPin1, LOW); 
          digitalWrite(valveRelayPin2, HIGH); 
          Serial.println("\n***valve-1 turned off, valve-2 turned on\n");          
    }
    
    if(mins == time4){
          digitalWrite(valveRelayPin2, LOW); 
          digitalWrite(valveRelayPin3, HIGH); 
          Serial.println("\n***valve-2 turned off, valve-3 turned on\n");    
    }
          
    if(mins == time5){
          digitalWrite(valveRelayPin3, LOW); 
          digitalWrite(valveRelayPin4, HIGH); 
          Serial.println("\n***valve-3 turned off, valve-4 turned on\n");          
    }
     
    if(mins == time6){
          digitalWrite(valveRelayPin4, LOW); 
          Serial.println("\n***valve-4 turned off\n");
    }
     
    if(mins == time4){
          digitalWrite(pumpRelayPin, LOW); 
          Serial.println("\n***pump turned off\n");                                                  
    }
  }
}

//--------------------------------------------------------------------------------------------------
//This is a function for counting minutes
int timer(){

  //waiting 1000 ms = 1s
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
