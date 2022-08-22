#include <soc/rtc.h>

const float sinePoints[] = {0.000063,0.000566,0.003081,0.007596,0.014094,0.022549,0.032926,0.045184,0.059273,0.075137,
                            0.092712,0.111927,0.132704,0.154960,0.178606,0.203546,0.229680,0.256902,0.285103,0.314169,
                            0.343983,0.374426,0.405374,0.436704,0.468288,0.500000,0.531712,0.563296,0.594626,0.625574,
                            0.656017,0.685831,0.714897,0.743098,0.770320,0.796454,0.821394,0.845040,0.867296,0.888073,
                            0.907288,0.924863,0.940727,0.954816,0.967074,0.977451,0.985906,0.992404,0.996919,0.999434,
                            0.999937,0.998427,0.994911,0.989401,0.981921,0.972500,0.961177,0.947997,0.933013,0.916285,
                            0.897881,0.877875,0.856347,0.833385,0.809079,0.783530,0.756839,0.729113,0.700465,0.671010,
                            0.640866,0.610155,0.579001,0.547528,0.515864,0.484136,0.452472,0.420999,0.389845,0.359134,
                            0.328990,0.299535,0.270887,0.243161,0.216470,0.190921,0.166615,0.143653,0.122125,0.102119,
                            0.083715,0.066987,0.052003,0.038823,0.027500,0.018079,0.010599,0.005089,0.001573,0.000063};


// the number of the LED pin
const int pwmPin1 = 16;  // 16 corresponds to GPIO16
const int pwmPin2 = 17;  // 17 corresponds to GPIO17
const int potPin = 34;

int potValue = 0;
int count = 0;

int perfil = 0;

int innerWaveForm = 1;
int outterWaveForm = 1;
int innerFrequency = 1;
int duty_pct = 50;
int duty = ((float)duty_pct / 100) * 255;
int uptime = 1000000; //micro
int wait = 1000000; //micro

// setting PWM properties
int freq = 2000;

const int BASE_DUTY_INIT_VALUE = 50; // 30% do dutyMax (255)

int innerWavePoints[256];

const int ledChannel1 = 0;
const int ledChannel2 = 1;
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
  int finalDuty = ((float)duty_pct / 100) * duty;
  
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
  TrapezoidalWave(channel, upTime, InnerPeriod);
//  SawToothWave(channel, upTime, InnerPeriod);
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
  int stepBase = round((float)period/BASE_DUTY_INIT_VALUE); // Tempo de cada step do duty base
 
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

void TrapezoidalWave(int channel, int upTime, int period) {
  //  Serial.println("TrapezoidalWave");

  int waveCount = 0;

  int stepBase = round((float)period/BASE_DUTY_INIT_VALUE); // Tempo de cada step do duty base

  int64_t old = esp_timer_get_time();
  float innerWaveElapsed = 0.0;
  float innerZeroElapsed = 0.0;

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


void SineWave(int channel, int dutyInicial, int upTime, int frequency) {
//  int localDuty = 0;
//  int actDuty = 0;
//  for(; localDuty <= duty; localDuty++) {
//    for(int i = 0; i < 100; i++) {
//      actDuty = (int)(localDuty * sinePoints[i]);
//      ledcWrite(channel, actDuty);
//      Serial.println(actDuty);
//    }
//    for(float i = 0; i <= pctVibTime*vibTimeMax; i++) {
//      Serial.println(0);
//      delay(2);
//    }
//  }
//  for(; localDuty >= 0; localDuty--) {
//    for(int i = 0; i < 100; i++) {
//      actDuty = (int)(localDuty * sinePoints[i]);
//      ledcWrite(channel, actDuty);
//      Serial.println(actDuty);
//    }
//    for(float i = 0; i <= pctVibTime*vibTimeMax; i++) {
//      Serial.println(0);
//      delay(2);
//    }
//  }
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
  // int innerWave, int outterWave, int localDuty, float frequency, int upTime, int wait, int channel
  PulseTrainGenerator(innerWaveForm, outterWaveForm, duty, innerFrequency, uptime, wait, ledChannel1);
  
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
