/* 
 * ESP-01 ADC sampling (set number of samples in one shot and pause time)
*/

const int sensorPin = A0; // connected to ADC pin of ESP-01

#define NUMBER_OF_SAMPLES 5000  // maximum number of samples taken
long samples[NUMBER_OF_SAMPLES]; // declare maximum size of array
int numberOfSamples = 200; // number of samples taken in a single shoot
unsigned long samplingTime;  // a variable value that stores the time in us to collect the numberOfSamples
int samplingPause = 3; // time in ms to pause between continuous sampling

unsigned long millisLastSample = 0;
unsigned long showResultsTimer;

int maxValue = 0; // to store max value, initialised at lowest value.
int minValue = 1024; // to store min value, initialised at highest value.

const int mVperAmp = 100; // Output sensitivity in mV per Amp
// Use scale factor: 185 for 5A module, 100 for 20A module and 66 for 30A module

float VPP = 0.0; // peak-to-peak voltage
float VRMS = 0.0;  // RMS voltage
float IRMS = 0.0; // RMS current

float VRMSoffset = 0.0; //0.025; // set quiescent Vrms output voltage
//voltage at an output terminal with reference to a common terminal, normally ground,
//when no signal is applied to the input.

void analogSample(void)
{
  // check sampling pause time
  //if true, then escape from the analogSample function
  //else, proceed to sampling
  if (millis() - samplingPause < millisLastSample)
  {
    return; // could this be the culprit??
  }
  // start sampling
  //samplingTime = micros();
  for (int i = 0; i < numberOfSamples; i++)
  {
    samples[i] = analogRead(sensorPin);
    
    // check if there is a new maximum and minimum value
    if (samples[i] > maxValue)
    {
      // record the new maximum sensor value
      maxValue = samples[i];
    }
    if (samples[i] < minValue)
    {
      // record the new minimum sensor value
      minValue = samples[i];
    }
  }
  //samplingTime = micros() - samplingTime; // not needed but good to know.
  // end of sampling
  
  millisLastSample = millis(); // works together to set the sampling pause time
}
/*
void showResults(void)
{
  // show results every 5s
  if(millis() - 5000 > showResultsTimer)
  {
    showResultsTimer = millis();
    // show maximum and minimum analog values
    Serial.print("Max value: ");
    Serial.print(maxValue);
    Serial.print(". ");
    Serial.print("Min value: ");
    Serial.print(maxValue);
    Serial.print(". ");
    Serial.print("VPP: ");
    Serial.print(VPP);
    Serial.print(" V. ");
    Serial.print("VRMS: ");
    Serial.print(VRMS, 3);  // print to 3 decimal places
    Serial.print(" V. ");
    Serial.print("Irms/A: ");
    Serial.print(IRMS, 3);  // print to 3 decimal places
    Serial.println(" A.");
    // check threshold values
    if (IRMS <= 200)
    {
      Serial.println("Low current value alert!");
    }
    else if (IRMS >= 800)
    {
      Serial.println("High current value alert!");
    }
  }
}
*/
void showResults(void)
{
    // show maximum and minimum analog values
    Serial.print("Max value: ");
    Serial.print(maxValue);
    Serial.print(". ");
    Serial.print("Min value: ");
    Serial.print(maxValue);
    Serial.print(". ");
    Serial.print("VPP: ");
    Serial.print(VPP);
    Serial.print(" V. ");
    Serial.print("VRMS: ");
    Serial.print(VRMS, 3);  // print to 3 decimal places
    Serial.print(" V. ");
    Serial.print("Irms/A: ");
    Serial.print(IRMS, 3);  // print to 3 decimal places
    Serial.println(" A.");
    // check threshold values
    if (IRMS <= 1)
    {
      Serial.println("Low current value alert!");
    }
    else if (IRMS >= 5)
    {
      Serial.println("High current value alert!");
    }
}

void setup() 
{
  Serial.begin(115200); // Default Baud Rate for ESP-01
  pinMode(sensorPin, INPUT); // analog input
}

void loop() 
{
  // 1000 loops, each taking 200 continuous analog readings, then pausing for 3ms before taking another sample.
  // time taken is a bit more than 3ms x 1000 = 3000ms.
  for (int j = 0; j < 1000; j++)
  {
    analogSample();
  }
  // subtract min from max and convert range to volts
  VPP = ((maxValue - minValue) * 5.0) / 1024.0;
  VRMS = ((VPP / 2.0) * 0.707) - VRMSoffset; // divide by 2 to get peak voltage. 1 ÷ √(2) is 0.707
  IRMS = (VRMS * 1000.0) / mVperAmp; // first, multiply by 1000 to convert to mV
  //show immediate calculated value of IRMS
  showResults();
  //reset max and min values for next 1000 loops, each taking 200 samples.
  maxValue = 0; // to store max value, initialised at lowest value.
  minValue = 1024; // to store min value, initialised at highest value.
}
