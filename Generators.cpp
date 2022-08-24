#include "Arduino.h"
#include "Generators.h"

void SawToothWaveVectorGenerator(int dutyInicial) {
  
  float dutyScaleStep = (float)dutyInicial/BASE_DUTY_INIT_VALUE; // Escala entre os valores de duty e dutyBase (ajuste de slope)
  int i = 0;
  
  for(float dutyLocal = (float)dutyInicial; dutyLocal >= 0; dutyLocal -= dutyScaleStep, i++) {
    innerWavePoints[i] = ceil(dutyLocal);
  }
}

void TrapezoidalWaveVectorGenerator(bool inv, int dutyInicial) {

  int finalDutyPct = 20;
  int finalDuty = ((float)finalDutyPct / 100) * dutyInicial;
  
  float dutyScaleStep = (float)dutyInicial/BASE_DUTY_INIT_VALUE; // Escala entre os valores de duty e dutyBase (ajuste de slope)
  int i = 0;

  if(inv) {
    for(float dutyLocal = (float)finalDuty; dutyLocal <= dutyInicial; dutyLocal += dutyScaleStep, i++) {
      innerWavePoints[i] = ceil(dutyLocal);
    }
  }
  else {
    for(float dutyLocal = (float)dutyInicial; dutyLocal >= finalDuty; dutyLocal -= dutyScaleStep, i++) {
      innerWavePoints[i] = ceil(dutyLocal);
    }
  }
  
}

void PulseTrainGenerator(int innerWave, int outterWave, int localDuty, int frequency, int upTime, int wait, int channel) {
  int InnerPeriod = ((1.0/frequency)*(1000000));
//  SquareWave(channel, localDuty, upTime, InnerPeriod);
  
//  SawToothWave(channel, upTime, InnerPeriod);
//  SineWave(channel, upTime, InnerPeriod);
    switch(innerWave) {
      case 0:
      case 1:
        TrapezoidalWave(channel, upTime, frequency, InnerPeriod);
        break;
      case 2:
        SawToothWave(channel, upTime, InnerPeriod);
        break;
      case 3:
        SquareWave(channel, localDuty, upTime, InnerPeriod);
        break;
      case 4:
        SineWave(channel, upTime, InnerPeriod);
        break;
      default:
        break;
    }
    RestWave(channel, wait, InnerPeriod);
  }
