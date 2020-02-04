/*
  Measuring AC Current Using ACS712
  Changelog: 
  Resolved detection of temporary current spikes, for example, when switching on.
  If current spike detected 2 times, indicate current threshold exceeded.
*/
const int ACS712_sensor = A0; // set the analog pin connected to the ACS712 current sensor
const int mVperAmp = 100; // Output sensitivity in mV per Amp
// ACS712 scale factor: 185 for 5A module, 100 for 20A module and 66 for 30A module

float AC_current; // store AC current Irms value
float CurrentLimit = 2.0; // set the maximum current threshold in Amps
int count = 0; // initialise count to zero

float VRMSoffset = 0.0; //0.025; // set quiescent Vrms output voltage
// voltage offset at analog terminal with reference to ground when no signal applied to the input.

void setup() {
  Serial.begin(9600);
}

void loop() {
  
  AC_current = getIRMS(); // current sensor value is returned about every 3s

  if (AC_current >= CurrentLimit)
  {
    Serial.println("Current spike detected and it exceeded threshold value.");
    count++;
    if (count == 2)
    {
      //digitalWrite(pumpRelay, LOW); // turn pump off
      Serial.println("Current threshold value exceeded!");
      count = 0; // reset count
    }
  }
  
  Serial.print("Irms/A: ");
  Serial.println(AC_current, 3);  // print to 3 decimal places
}

// function to measure peak-to-peak voltage and calculate Irms value
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
