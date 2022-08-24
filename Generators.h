#ifndef Generators_h
#define Generators_h

#include "InnerWaves.h"

void GenerateWaveVector();
void GenerateSawToothWaveVector(int dutyInicial);
void GenerateTrapezoidalWaveVector(bool inv, int dutyInicial);
void GeneratePulseTrain(int innerWave, int outterWave, int localDuty, int frequency, int upTime, int wait, int channel);

#endif
