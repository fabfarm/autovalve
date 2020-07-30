//------------------------------------------------------------------------------------
// Function to measure ADC and calculate Irms value
//------------------------------------------------------------------------------------ 

float getIRMS()
{
  float VPP; // peak-to-peak voltage
  float VRMS;  // RMS voltage
  float IRMS; // RMS current
  int readValue; // analog value from the sensor
  int maxValue = 0; // store max value, initialised at lowest value.
  int minValue = 1024; // store min value, initialised at highest value.

  // controlled sampling rate to avoid Wi-Fi disconnection
  for (int j = 0; j < 100; j++) // repeat 100 times for more accurate max and min readings
  {
    for (int i = 0; i < NUMBER_OF_SAMPLES; i++) // start continuous analog sampling
    {
      readValue = analogRead(ACS712_sensor);
      // check if there is a new maximum and minimum value
      if (readValue > maxValue)
      {
        maxValue = readValue; // record the new maximum sensor value
      }
      else if (readValue < minValue)
      {
        minValue = readValue; // record the new minimum sensor value
      }
    }
    delay(3); // pause for 3 ms
  }
  // subtract min from max and convert range to volts
  VPP = ((maxValue - minValue) * 5.0) / 1024.0; // find peak-to-peak voltage
  VRMS = ((VPP / 2.0) * 0.707) - VRMSoffset; // divide by 2 to get peak voltage. 1 ÷ √(2) is 0.707
  IRMS = (VRMS * 1000.0) / mVperAmp; // first, multiply by 1000 to convert to mV
  
//  Serial.print("Vpp/V: ");
//  Serial.print(VPP, 3); // print to 3 decimal places
//  Serial.print("\tVrms/V: ");
//  Serial.print(VRMS, 3);
  Serial.print("\tIrms/A: ");
  Serial.println(IRMS, 3);
  
  return IRMS;
}
