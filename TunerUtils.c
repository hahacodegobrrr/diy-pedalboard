#include "TunerUtils.h"
#include <stdint.h>
#include <math.h>

float noteTable[] = {

};

const float ln2 = 0.69314718f;
const float A4 = 440;

struct TunerNote* getNote(uint16_t frequency) {
  float freqTo12Tone = (float)(fmod((log((double)frequency / (double)A4) / ln2), 1));
  return NULL;
}

uint8_t* generateDisplayMessage() {
  return 0;
}