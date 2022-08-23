#include <soc/rtc.h>
//#include "pitches.h"

// Definição de notas musicais
#define C_0 20
#define D_0 25
#define E_0 30
#define F_0 35
#define G_0 40
#define A_0 45
#define B_0 50
#define C_1 60

#define BASE_COMPASS 300000

const int freqList[] = {G_0, E_0, G_0, E_0, G_0, E_0, D_0, C_0};
const int noteTimes[] = {1, 2, 2, 2, 2, 2, 2, 2};

const float sinePoints[] = {0.000000,0.188255,0.611260,0.950484,0.950484,0.611260,0.188255,0.000000};


// the number of the LED pin
const int pwmPin1 = 16;    // 16 corresponds to GPIO16
const int pwmPin2 = 17;    // 17 corresponds to GPIO17
const int TONE_OUTPUT_PIN = 18;  // 18 corresponds to GPIO18

int potValue = 0;
int count = 0;

int perfil = 0;

int innerWaveForm = 1;
int outterWaveForm = 1;
int innerFrequency = 1;
int duty_pct = 30;
int duty = ((float)duty_pct / 100) * 255;
int uptime = 1000000; //micro
int wait = 1000000; //micro

// setting PWM properties
int freq = 2000;

const int BASE_DUTY_INIT_VALUE = 50; // 30% do dutyMax (255)

int innerWavePoints[256];

const int TONE_PWM_CHANNEL = 1;
const int ledChannel1 = 0;
const int ledChannel2 = 2;
const int resolution = 8;

void setup() {

  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("MinScale, MaxScale, Inner, Outter");
//  Serial.println("MinScale, MaxScale, Train");

  memset(innerWavePoints, 0, sizeof(innerWavePoints));

//  SawToothWaveVectorGenerator(duty);
  TrapezoidalWaveVectorGenerator(duty);

  // configure LED PWM functionalitites
  ledcSetup(ledChannel1, freq, resolution);
  ledcSetup(ledChannel2, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(pwmPin1, ledChannel1);
  ledcAttachPin(pwmPin2, ledChannel2);
  ledcAttachPin(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);
}

void SawToothWaveVectorGenerator(int dutyInicial) {
  
  float dutyScaleStep = (float)dutyInicial/BASE_DUTY_INIT_VALUE; // Escala entre os valores de duty e dutyBase (ajuste de slope)
  int i = 0;
  
  for(float dutyLocal = (float)dutyInicial; dutyLocal >= 0; dutyLocal -= dutyScaleStep, i++) {
    innerWavePoints[i] = ceil(dutyLocal);
  }
}

void TrapezoidalWaveVectorGenerator(int dutyInicial) {

  int finalDutyPct = 20;
  int finalDuty = ((float)finalDutyPct / 100) * dutyInicial;
  
  float dutyScaleStep = (float)dutyInicial/BASE_DUTY_INIT_VALUE; // Escala entre os valores de duty e dutyBase (ajuste de slope)
  int i = 0;
  
  for(float dutyLocal = (float)dutyInicial; dutyLocal >= finalDuty; dutyLocal -= dutyScaleStep, i++) {
    innerWavePoints[i] = ceil(dutyLocal);
  }
}

void PulseTrainGenerator(int innerWave, int outterWave, int localDuty, int frequency, int upTime, int wait, int channel) {
  int InnerPeriod = ((1.0/frequency)*(1000000));
//  Serial.println(InnerPeriod);
//  SquareWave(channel, localDuty, upTime, InnerPeriod);
//  TrapezoidalWave(channel, upTime, frequency, InnerPeriod);
//  SawToothWave(channel, upTime, InnerPeriod);
  SineWave(channel, upTime, InnerPeriod);
//    switch(innerWave) {
//      case 0:
//        SquareWave(channel, localDuty, upTime, InnerPeriod);
//        break;
//      case 1:
//        SawToothWave(channel, localDuty, upTime, InnerPeriod);
//        break;
//      case 2:
//        TriangleWave(channel, localDuty, upTime, InnerPeriod);
//        break;
//      case 3:
//        SineWave(channel, localDuty, upTime, InnerPeriod);
//        break;
//      default:
//        break;
//    }
    RestWave(channel, wait, InnerPeriod);
  }

void RestWave(int channel, int _time, int period) {
  int stepBase = round((float)period/8); // Tempo de cada step do duty base
//  int stepBase = round((float)period/BASE_DUTY_INIT_VALUE); // Tempo de cada step do duty base
 
  int64_t old = esp_timer_get_time();

  while(esp_timer_get_time() - old <= (int64_t)_time) {
    ledcWrite(channel, 0);
    Serial.println("0, 255, 0, 0");
//    Serial.println("0, 255, 0");

    int64_t oldStep = esp_timer_get_time();

    // Espera o stepTime para decair o duty
    while(esp_timer_get_time() - oldStep <= (int64_t)stepBase);
  }
}

void SquareWave(int channel, int localDuty, int upTime, int frequency) {
//  Serial.println("squareWave");

  int64_t old = esp_timer_get_time();

  while(esp_timer_get_time() - old <= (int64_t)upTime) {
    
    int64_t oldInner = esp_timer_get_time();
    
    while(esp_timer_get_time() - oldInner <= (int64_t)frequency/2) {
      Serial.printf("0, 255, %d\n", localDuty);
      ledcWrite(channel, localDuty);
    }
    while(esp_timer_get_time() - oldInner <= (int64_t)frequency) {
      Serial.printf("0, 255, 0\n");
      // checks if uptime is over
      ledcWrite(channel, 0);
    }
    
  }
}

void TrapezoidalWave(int channel, int upTime, int frequency, int period) {
  //  Serial.println("TrapezoidalWave");

  int waveCount = 0;

  int stepBase = round((float)period/BASE_DUTY_INIT_VALUE); // Tempo de cada step do duty base

  int64_t old = esp_timer_get_time();
  float innerWaveElapsed = 0.0;
  float innerZeroElapsed = 0.0;

  ledcWriteTone(TONE_PWM_CHANNEL, 17*frequency); // Buzzer ON !

  while(esp_timer_get_time() - old <= (int64_t)upTime) {
    
    int64_t oldInner = esp_timer_get_time();
    
    for(int i = 0; (esp_timer_get_time() - oldInner <= (int64_t)period/2) && (innerWavePoints[i] > 0); i++) {      
      ledcWrite(channel, innerWavePoints[i]);
      Serial.printf("0, 255, %d, %d\n", innerWavePoints[i], duty); 

      int64_t oldStep = esp_timer_get_time();

      // Espera o stepTime para decair o duty
      while(esp_timer_get_time() - oldStep <= (int64_t)stepBase);
    }
    innerWaveElapsed = ((float)esp_timer_get_time() - oldInner)/1000000;
    while(esp_timer_get_time() - oldInner <= (int64_t)period) {
      ledcWrite(channel, 0);
      Serial.printf("0, 255, 0, %d\n", duty);
      int64_t oldStep = esp_timer_get_time();

      // Espera o stepTime para decair o duty
      while(esp_timer_get_time() - oldStep <= (int64_t)stepBase);
    }

    innerZeroElapsed = ((float)esp_timer_get_time() - oldInner)/1000000;

    waveCount++;
    
  }

  ledcWriteTone(TONE_PWM_CHANNEL, 0); // Buzzer OFF !

//  Serial.println(waveCount);
//  Serial.printf("Elapsed innerWave: %f\n", innerWaveElapsed);
//  Serial.printf("Elapsed innerZeros: %f\n", innerZeroElapsed);
//  Serial.printf("Elapsed out: %f\n", ((float)esp_timer_get_time() - old)/1000000);
//  delay(2000);
  
}

void SawToothWave(int channel, int upTime, int period) {

  int stepBase = round((float)period/BASE_DUTY_INIT_VALUE); // Tempo de cada step do duty base

  int64_t old = esp_timer_get_time();
  
  while(esp_timer_get_time() - old <= (int64_t)upTime) {
    for(int i = 0; (esp_timer_get_time() - old <= (int64_t)upTime) && (innerWavePoints[i] > 0); i++) {

      ledcWrite(channel, innerWavePoints[i]);
      Serial.printf("0, 80, %d, %d\n", innerWavePoints[i], duty);

      int64_t oldInner = esp_timer_get_time();

      // Espera o stepTime para decair o duty
      while(esp_timer_get_time() - oldInner <= (int64_t)stepBase) {}
    }

  }
  
}

void TriangleWave(int channel, int dutyInicial, int upTime, int frequency) {
  int dutyLocal = 0;
  for(; dutyLocal <= duty; dutyLocal++) {
    ledcWrite(channel, dutyLocal);
    Serial.println(dutyLocal);
  }
    for(; dutyLocal >= 0; dutyLocal--) {
    ledcWrite(channel, dutyLocal);

    Serial.println(dutyLocal);
  }
}


void SineWave(int channel, int upTime, int period) {

  int stepBase = round((float)period/8); // Tempo de cada step do duty base
  int actDuty = 0;

  int64_t old = esp_timer_get_time();
  int waveCount = 0;

  while(esp_timer_get_time() - old <= (int64_t)upTime) {
    for(int i = 0; (esp_timer_get_time() - old <= (int64_t)upTime) && (i < 8); i++) {
      actDuty = (int)(duty * sinePoints[i]);
      ledcWrite(channel, actDuty);
      Serial.printf("0, 255, %d, %d\n", actDuty, duty); 
  
      int64_t oldStep = esp_timer_get_time();
  
      // Espera o stepTime para decair o duty
      while(esp_timer_get_time() - oldStep <= (int64_t)stepBase);
    }
    waveCount++;
  }

  Serial.println(waveCount);
  delay(2000);
}

void ParseSerialParams(String serialData) {
  // 0:0:30:10:1000000:1000000
  // Extrai o innerWaveForm
  innerWaveForm = serialData.substring(0, serialData.indexOf(":")).toInt();
  serialData.remove(0, serialData.indexOf(":") + 1);
  // Extrai o outterWaveForm
  outterWaveForm = serialData.substring(0, serialData.indexOf(":")).toInt();
  serialData.remove(0, serialData.indexOf(":") + 1);
  // Extrai o localDuty
  duty_pct = serialData.substring(0, serialData.indexOf(":")).toInt();
  duty = ((float)duty_pct / 100) * 255;
  serialData.remove(0, serialData.indexOf(":") + 1);
  // Extrai o frequency
  innerFrequency = serialData.substring(0, serialData.indexOf(":")).toInt();
  serialData.remove(0, serialData.indexOf(":") + 1);
  // Extrai o upTime
  uptime = serialData.substring(0, serialData.indexOf(":")).toInt();
  serialData.remove(0, serialData.indexOf(":") + 1);
  // Extrai o wait
  wait = serialData.substring(0, serialData.indexOf(":")).toInt();

  TrapezoidalWaveVectorGenerator(duty);
}

void loop() {  
  // Defaults
  // waveForm = 1 (Dente-de-serra)
  // duty_pct = 2% (0.02 * 255 -> aprox 5)

  if(Serial.available()) { // innerWave, outterWave, localDuty, frequency, upTime, wait
    String serialData = Serial.readString();
    ParseSerialParams(serialData);
  }

    PulseTrainGenerator(innerWaveForm, outterWaveForm, duty, innerFrequency, uptime, wait, ledChannel1);

//  for (int i = 0; i < (sizeof(freqList) / sizeof(freqList[0])); i++) {
//    PulseTrainGenerator(innerWaveForm, outterWaveForm, duty, (int)((float)freqList[i]/1.5), noteTimes[i] * BASE_COMPASS, wait, ledChannel1);
//  }
//
//  delay(1000);
  
//   if(waveForm == 0) { // Onda Quadrada
//     SquareWave(ledChannel1);
//   }
//   else if(waveForm == 1) { // Onda Dente-de-serra
//   }
//   else if (waveForm == 2) { // Onda triangular
//     TriangleWave(ledChannel1);
//     for(int i = 0; i <= 100; i++) Serial.println(0);
// ////    TriangleWave(ledChannel2);
// //    for(int i = 0; i <= 2000; i++) Serial.println(0);
//   }
//   else {
//     SineWave(ledChannel1);
//   }
  
}