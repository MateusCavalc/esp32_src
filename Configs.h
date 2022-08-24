#ifndef Configs_h
#define Configs_h

struct GlobalParams {
   int innerWaveForm;
   int outterWaveForm;
   int duty_pct; // %
   int innerFrequency; // Hz
   int uptime; // Micro
   int wait;
};

// GPIO pins
struct Pins {
  int PWM_PIN_1;
  int PWM_PIN_2;
  int TONE_PIN;
};

// PWM channels
struct Channels {
  int TONE_CHANNEL;
  int PADS_CHANNEL_1;
  int PADS_CHANNEL_2;
};

// General configs
struct Configs {
  int PWM_FREQ; // Hz
  int PWM_RESOLUTION; // bits
  int BASE_DUTY_INIT_VALUE; // máx 255
};

extern int bufferPoints[256];
extern const float sinePoints[];

extern GlobalParams params;
extern Pins pins;
extern Channels channels;
extern Configs configs;

#endif
