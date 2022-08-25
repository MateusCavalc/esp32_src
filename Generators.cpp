#include "Generators.h"
#include "Arduino.h"

void GenerateWaveVector() {
  int duty = ((float)params.duty_pct / 100) * 255;
  
  switch(params.innerWaveForm) {
    case 0:
      GenerateTrapezoidalWaveVector(false, duty); // Crescente ( /| )
      break;
    case 1:
      GenerateTrapezoidalWaveVector(true, duty);  // Decaimento ( |\ )
      break;
    case 2:
    case 3:
      GenerateSawToothWaveVector(duty);
      break;
    default:
      break;
  }
}

void GenerateTrapezoidalWaveVector(bool inv, int dutyInicial) {

  int finalDutyPct = 20;
  int finalDuty = ((float)finalDutyPct / 100) * dutyInicial;
  
  float dutyScaleStep = (float)dutyInicial/configs.BASE_DUTY_INIT_VALUE; // Escala entre os valores de duty e dutyBase (ajuste de slope)
  int i = 0;

  if(inv) {
    for(float dutyLocal = (float)finalDuty; dutyLocal <= dutyInicial; dutyLocal += dutyScaleStep, i++) {
      bufferPoints[i] = ceil(dutyLocal);
    }
  }
  else {
    for(float dutyLocal = (float)dutyInicial; dutyLocal >= finalDuty; dutyLocal -= dutyScaleStep, i++) {
      bufferPoints[i] = ceil(dutyLocal);
    }
  }
  
}

void GenerateSawToothWaveVector(int dutyInicial) {
  
  float dutyScaleStep = (float)dutyInicial/configs.BASE_DUTY_INIT_VALUE; // Escala entre os valores de duty e dutyBase (ajuste de slope)
  int i = 0;
  
  for(float dutyLocal = (float)dutyInicial; dutyLocal >= 0; dutyLocal -= dutyScaleStep, i++) {
    bufferPoints[i] = ceil(dutyLocal);
  }
}

void GenerateOutterSquareWave(int innerWave, int outterWave, int localDuty, int frequency, int upTime, int wait, int channel) {
  int InnerPeriod = ((1.0/frequency)*(1000000));
  
    switch(innerWave) {
      case 0:
      case 1:
        Square_SquareWave(channel, upTime, InnerPeriod, localDuty);
        break;
      case 2:
        SawToothWave(channel, upTime, InnerPeriod);
        break;
      case 3:
//        SquareWave(channel, localDuty, upTime, InnerPeriod);
        break;
      case 4:
//        SineWave(channel, upTime, InnerPeriod, outterWave);
        break;
      default:
        break;
    }

    RestWave(channel, wait, InnerPeriod);
}

void GenerateOutterSawToothWave(int innerWave, int outterWave, int localDuty, int frequency, int upTime, int wait, int channel) {
  int InnerPeriod = ((1.0/frequency)*(1000000));
  
    switch(innerWave) {
      case 0:
        Square_SquareWave(channel, upTime, InnerPeriod, localDuty);
        break;
      case 1:
        TrapezoidalWave(channel, upTime, frequency, InnerPeriod);
        break;
      case 2:
//        Square_SquareWave(channel, upTime, InnerPeriod);
        break;
      case 3:
        SawTooth_SquareWave(channel, upTime, InnerPeriod);
        break;
      case 4:
//        SineWave(channel, upTime, InnerPeriod, outterWave);
        break;
      default:
        break;
    }

    RestWave(channel, wait, InnerPeriod);
}

void GeneratePulseTrain(int innerWave, int outterWave, int localDuty, int frequency, int upTime, int wait, int channel) {
//  SquareWave(channel, localDuty, upTime, InnerPeriod);
  int InnerPeriod = ((1.0/frequency)*(1000000));
  
//  SawToothWave(channel, upTime, InnerPeriod);
//  SineWave(channel, upTime, InnerPeriod);
    switch(outterWave) {
      case 0:
          GenerateOutterSquareWave(innerWave, outterWave, localDuty, frequency, upTime, wait, channel);
          break;
      case 1:
//        GenerateOutterTrapezoidalWave(channel, upTime, frequency, InnerPeriod);
        break;
      case 2:
        GenerateOutterSawToothWave(innerWave, outterWave, localDuty, frequency, upTime, wait, channel);
        break;
      case 3:
//        GenerateOutterSquareWave(innerWave, outterWave, localDuty, frequency, upTime, wait, channel);
        break;
      case 4:
//        GenerateOutterSineWave(channel, upTime, InnerPeriod);
        break;
      default:
        break;
    }
//    RestWave(channel, wait, InnerPeriod);
  }
