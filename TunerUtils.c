#include "TunerUtils.h"
#include <stdint.h>
#include <math.h>

#define MESSAGE_SIZE 32

const float ln2 = 0.69314718f;
const float A4 = 440;

uint8_t noteTable[] = {'A','a','B','C','c','D','d','E','F','f','G','g'};

struct TunerNote tunerNote;
uint8_t message[MESSAGE_SIZE] = {'T','U','N','E','R',' ',' ',' ','N','O','T','E',':',' '};

void getNote(uint16_t frequency) {
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
}

uint8_t* generateTunerDisplayMessage(uint16_t frequency) {
  //message indices 0-13 have already been written to
  getNote(frequency);
  if (noteTable[tunerNote.note] >= 'a') {
    message[14] = noteTable[tunerNote.note] - 32;
    message[15] = '#';
  } else {
    message[14] = noteTable[tunerNote.note];
    message[15] = ' ';
  }
  int i;
  for (i = 0; i < MESSAGE_SIZE / 2; i++) {
    float transI = (i - 7.5) * 0.5 / 7.5; //transforms (0-15) to (-0.5-0.5)
    message[i + MESSAGE_SIZE / 2] = (tunerNote.error > 0)? (transI < tunerNote.error && transI > 0)? '-' : ' ' : (transI > tunerNote.error && transI < 0)? '-' : ' ';
  }

  return &message[0];
}