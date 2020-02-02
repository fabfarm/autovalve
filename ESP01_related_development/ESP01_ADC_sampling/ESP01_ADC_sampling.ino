/* 
 * ESP-01 ADC sampling (set number of samples in one shot and pause time)
*/

const int sensorPin = A0; // connected to ADC pin of ESP-01

#define NUMBER_OF_SAMPLES 5000  // maximum number of samples taken
long samples[NUMBER_OF_SAMPLES]; // declare maximum size of array
int numberOfSamples = 200; // number of samples taken in a single shoot
unsigned long samplingTime;  // a variable value that stores the time in us to collect the numberOfSamples
int samplingPause = 3; // time in ms to pause between continuous sampling

unsigned long totalSamples = 0; // initialised to zero
unsigned long millisLastSample;
unsigned long showStatisticsTimer;
long sumOfSampleValues = 0; // sum of samples based on numberOfSamples
long averageSampleValue; // average value of analog readings based on numberOfSamples

void analogSample(void)
{
  if (millis() - samplingPause < millisLastSample )
  {
    return;
  }
  samplingTime = micros();
  for (int i = 0; i < numberOfSamples; i++)
  {
    samples[i] = analogRead(A0);
    totalSamples++;
    sumOfSampleValues += samples[i];
  }
  samplingTime = micros() - samplingTime;
  averageSampleValue = sumOfSampleValues / numberOfSamples; // average sample value
  sumOfSampleValues = 0; // reset for next burst sample reading
  millisLastSample = millis();
}

void showStatistics(void)
{
  // show statistics every 3s
  if(millis() - 3000 > showStatisticsTimer)
  {
    showStatisticsTimer = millis();
    // show total samples since program start
    //Serial.print("Total samples: ");
    //Serial.print(totalSamples);
    //Serial.print(". ");
    
    // average VALUE of numberOfSamples (200) taken in a single shot
    Serial.print("Avg value of ");
    Serial.print(numberOfSamples);
    Serial.print(" samples: ");
    Serial.print(averageSampleValue);
    Serial.print(". ");
    // average NUMBER of samples per second including pause time
    Serial.print("Avg no. of samples/s: ");
    Serial.print((numberOfSamples) / (samplingTime * 1000000 + samplingPause * 1000));
    Serial.print(". ");
    // average VALUE of samples per second including pause time
    Serial.print("Avg value of samples/s: ");
    Serial.println((numberOfSamples) / (samplingTime * 1000000 + samplingPause * 1000));
  }
}

void setup() 
{
  Serial.begin(115200); // Default Baud Rate for ESP-01
  pinMode(A0, INPUT); // analog input

  Serial.println();
  Serial.println("Analog values:");
}

void loop() 
{
  // take 200 continuous analog readings and then pause before taking next sample
  analogSample();

  // show statistics
  showStatistics();

  // check average sample threshold values
  if (averageSampleValue <= 200)
  {
    Serial.println("Low sensor value alert!");
  }
  else if (averageSampleValue >= 800)
  {
    Serial.println("High sensor value alert!");
  }
}
