#ifndef Generators_h
#define Generators_h

void SawToothWaveVectorGenerator(int dutyInicial);
void TrapezoidalWaveVectorGenerator(bool inv, int dutyInicial);
void PulseTrainGenerator(int innerWave, int outterWave, int localDuty, int frequency, int upTime, int wait, int channel);

#endif
