#ifndef InnerWaves_h
#define InnerWaves_h

#include "Configs.h"

void RestWave(int channel, int _time, int period);  
void TrapezoidalWave(int channel, int upTime, int frequency, int period);
void SquareWave(int channel, int localDuty, int upTime, int frequency);
void SawToothWave(int channel, int upTime, int period);
void TriangleWave(int channel, int dutyInicial, int upTime, int frequency);
void SineWave(int channel, int upTime, int period);

#endif
