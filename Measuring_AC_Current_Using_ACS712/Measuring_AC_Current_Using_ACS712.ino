/*
* Measuring AC Current Using ACS712
*/
const int ACsensorPin = A0;
const int mVperAmp = 100; // Output sensitivity in mV per Amp
// Use 185 for 5A module, 100 for 20A module and 66 for 30A module

double VPP = 0; // peak-to-peak voltage
double VRMS = 0;  // RMS voltage
double AmpsRMS = 0; // RMS current

float ACSoffset = 0.025; // set quiescent Vrms output voltage
//voltage at an output terminal with reference to a common terminal, normally ground, 
//when no signal is applied to the input.

void setup(){
  Serial.begin(115200);
}

void loop(){
  VPP = getVPP(); // find peak-to-peak voltage
  VRMS = ((VPP / 2.0) * 0.707) - ACSoffset; // divide by 2 to get peak voltage. 1 ÷ √(2) is 0.707
  AmpsRMS = (VRMS * 1000) / mVperAmp; // first, multiply by 1000 to convert to mV
  
  Serial.print("Vpp: ");
  Serial.print(VPP, 3);// print to 3 decimal places
  Serial.print("\tVrms: ");
  Serial.print(VRMS, 3);// print to 3 decimal places
  Serial.print("\tIrms: ");
  Serial.println(AmpsRMS, 3);// print to 3 decimal places
}

float getVPP()  // continously sampling and logging max and min values
{
  float result;
  int readValue; // value from the sensor
  int maxValue = 0; // to store max value, initialised at lowest value.
  int minValue = 1024; // to store min value, initialised at highest value.

  // non-blocking method
  uint32_t start_time = millis();
  while((millis()- start_time) < 1000) // sample for 1000 ms or 1 sec
  {
    readValue = analogRead(ACsensorPin);
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
