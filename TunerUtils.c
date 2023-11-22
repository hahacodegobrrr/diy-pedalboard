#include "TunerUtils.h"
#include <stdint.h>
#include <math.h>

const float ln2 = 0.69314718f;
const float A4 = 440;

struct TunerNote tunerNote;

struct TunerNote* getNote(uint16_t frequency) {
  float freqTo12Tone = (float)(fmod(log(frequency / A4) / ln2, 1)) * 12;
  
  float error = fmod(freqTo12Tone, 1);
  uint8_t note;
  if (error >= 0.5) {
    error -= 1; //wrap around to next note's error
    note = ((uint8_t)freqTo12Tone + 1) % 12;
  } else {
    note = (uint8_t)freqTo12Tone;
  }

  tunerNote.note = note;
  tunerNote.error = error;

  //theoretically stays the same, but fuck it
  return &tunerNote;
}

uint8_t* generateDisplayMessage() {
  return 0;
}