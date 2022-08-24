#include <soc/rtc.h>
#include "InnerWaves.h"

// Definição de notas musicais
#define C_0 10
#define D_0 15
#define E_0 20
#define F_0 25
#define G_0 30
#define A_0 40
#define B_0 50
#define C_1 60

#define BASE_COMPASS 300000

const int freqList[] = {C_0, C_0, G_0, E_0, A_0, A_0, C_0, G_0};
const int noteTimes[] = {2, 2, 2, 2, 2, 2, 2, 2};

const float sinePoints[] = {0.000000,0.188255,0.611260,0.950484,0.950484,0.611260,0.188255,0.000000};


// the number of the LED pin
const int pwmPin1 = 16;    // 16 corresponds to GPIO16
const int pwmPin2 = 17;    // 17 corresponds to GPIO17
const int TONE_OUTPUT_PIN = 18;  // 18 corresponds to GPIO18

int potValue = 0;
int count = 0;

int perfil = 0;

int innerWaveForm = 1;
int outterWaveForm = 0;
int innerFrequency = 1;
int duty_pct = 30;
int duty = ((float)duty_pct / 100) * 255;
int uptime = 1000000; //micro
int wait = 100000; //micro

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
  TrapezoidalWaveVectorGenerator(true, duty);

  // configure LED PWM functionalitites
  ledcSetup(ledChannel1, freq, resolution);
  ledcSetup(ledChannel2, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(pwmPin1, ledChannel1);
  ledcAttachPin(pwmPin2, ledChannel2);
  ledcAttachPin(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);
}

void loop() {  
  // Defaults
  // waveForm = 1 (Dente-de-serra)
  // duty_pct = 2% (0.02 * 255 -> aprox 5)

  if(Serial.available()) { // innerWave, outterWave, localDuty, frequency, upTime, wait
    String serialData = Serial.readString();
    ParseSerialParams(serialData);
  }

//    PulseTrainGenerator(innerWaveForm, outterWaveForm, duty, innerFrequency, uptime, wait, ledChannel1);

  for (int i = 0; i < (sizeof(freqList) / sizeof(freqList[0])); i++) {
    PulseTrainGenerator(innerWaveForm, outterWaveForm, duty, (int)((float)freqList[i]/2), noteTimes[i] * BASE_COMPASS, wait, ledChannel1);
  }

  delay(1000);
  
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

  switch(innerWaveForm) {
      case 0:
        TrapezoidalWaveVectorGenerator(false, duty);
        break;
      case 1:
        TrapezoidalWaveVectorGenerator(true, duty);
        break;
      default:
        break;
    }
}
