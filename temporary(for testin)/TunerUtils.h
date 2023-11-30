#ifndef TUNERUTILS_H
#define TUNERUTILS_H
#include <stdint.h>

struct TunerNote {
  uint8_t note;
  float error;
};

uint8_t* generateTunerDisplayMessage(uint16_t frequency);

#endif
