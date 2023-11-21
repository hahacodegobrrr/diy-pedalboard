#ifndef TUNERUTILS_H
#define TUNERUTILS_H
#include <stdint.h>

struct TunerNote {
  uint8_t note;
  float error;
};

struct TunerNote* getNote(uint16_t frequency);

uint8_t* generateDisplayMessage();

#endif
