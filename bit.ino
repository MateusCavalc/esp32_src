#include <soc/rtc.h>
#include "Generators.h"

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

void setup() {

  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("MinScale, MaxScale, Inner, Outter");
//  Serial.println("MinScale, MaxScale, Train");

  memset(bufferPoints, 0, sizeof(bufferPoints));

  GenerateWaveVector();

  // configure LED PWM functionalitites
  ledcSetup(channels.PADS_CHANNEL_1, configs.PWM_FREQ, configs.PWM_RESOLUTION);
  ledcSetup(channels.PADS_CHANNEL_2, configs.PWM_FREQ, configs.PWM_RESOLUTION);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(pins.PWM_PIN_1, channels.PADS_CHANNEL_1);
  ledcAttachPin(pins.PWM_PIN_2, channels.PADS_CHANNEL_2);
  ledcAttachPin(pins.TONE_PIN, channels.TONE_CHANNEL);
}

void loop() {  
  // Defaults
  // waveForm = 1 (Dente-de-serra)
  // duty_pct = 2% (0.02 * 255 -> aprox 5)

  if(Serial.available()) { // innerWave, outterWave, localDuty, frequency, upTime, wait
    String serialData = Serial.readString();
    ParseSerialParams(serialData);
  }

  int duty = ((float)params.duty_pct / 100) * 255;

    GeneratePulseTrain(params.innerWaveForm, params.outterWaveForm, duty, params.innerFrequency, params.uptime, params.wait, channels.PADS_CHANNEL_1);

//  for (int i = 0; i < (sizeof(freqList) / sizeof(freqList[0])); i++) {
//    GeneratePulseTrain(params.innerWaveForm, params.outterWaveForm, duty, (int)((float)freqList[i]/2), noteTimes[i] * BASE_COMPASS, params.wait, channels.PADS_CHANNEL_1);
//  }

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

void ParseSerialParams(String serialData) {
  // 0:0:70:50:1000000:1000000
  // Extrai o innerWaveForm
  params.innerWaveForm = serialData.substring(0, serialData.indexOf(":")).toInt();
  serialData.remove(0, serialData.indexOf(":") + 1);
  // Extrai o outterWaveForm
  params.outterWaveForm = serialData.substring(0, serialData.indexOf(":")).toInt();
  serialData.remove(0, serialData.indexOf(":") + 1);
  // Extrai o localDuty
  params.duty_pct = serialData.substring(0, serialData.indexOf(":")).toInt();
  serialData.remove(0, serialData.indexOf(":") + 1);
  // Extrai o frequency
  params.innerFrequency = serialData.substring(0, serialData.indexOf(":")).toInt();
  serialData.remove(0, serialData.indexOf(":") + 1);
  // Extrai o upTime
  params.uptime = serialData.substring(0, serialData.indexOf(":")).toInt();
  serialData.remove(0, serialData.indexOf(":") + 1);
  // Extrai o wait
  params.wait = serialData.substring(0, serialData.indexOf(":")).toInt();

  GenerateWaveVector();
}
