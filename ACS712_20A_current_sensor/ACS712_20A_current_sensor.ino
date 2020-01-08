/*
 * Allegro ACS712 Current Sensor with range of up to 20A
 */

const int VIN = A0; // define the Arduino pin A0 as voltage input (V in)
const float VCC   = 5.0;// supply voltage is from 4.5 to 5.5V. Normally 5V.
const int model = 1;   // enter the model number

/*
"ACS712ELCTR-05B-T" use model 0
"ACS712ELCTR-20A-T" use model 1
"ACS712ELCTR-30A-T" use model 2  
sensitivity array is holding the sensitivy of the ACS712 current sensors. 
Do not change. All values are from page 5 of datasheet.        
*/

float cutOffLimit = 1.01; // set the current which below that value, doesn't matter. Or set 0.5

float sensitivity[] = {
          0.185,// for ACS712ELCTR-05B-T
          0.100,// for ACS712ELCTR-20A-T
          0.066// for ACS712ELCTR-30A-T
         }; 

const float QOV =   0.5 * VCC;// set quiescent output voltage of 0.5V
//The dc voltage at an output terminal with reference to a common terminal, normally ground, 
//when no signal is applied to the input.

float voltage;// global variable

void setup() {
    Serial.begin(9600);// initialize serial monitor
    Serial.println("ACS712 20 A current Sensor");
}

void loop() {
  
  float voltage_raw =   (5.0 / 1023.0)* analogRead(VIN); // Read the voltage from sensor
  voltage =  voltage_raw - QOV + 0.012 ; // 0.000 is a value to make voltage zero when there is no current
  float current = voltage / sensitivity[model];
 
  if(abs(current) > cutOffLimit ){
    Serial.print("V: ");
    Serial.print(voltage, 3);// print voltage with 3 decimal places
    Serial.print("V, I: ");
    Serial.print(current, 2); // print the current with 2 decimal places
    Serial.println("A");
  }
  else{
    Serial.println("No Current");
  }
  delay(500);
}
