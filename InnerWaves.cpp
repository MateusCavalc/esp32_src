#include "Arduino.h"
#include "InnerWaves.h"

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

//  Serial.println(waveCount);
//  delay(2000);
}
