#ifndef TUNERUTILS_H
#define TUNERUTILS_H
#include <stdint.h>

struct TunerNote {
  uint8_t note;
  float error;
};

/**
 * @brief Takes a frequency and returns a number from 0 to 12(exclusive)
 * indicating the note (0 is A, 1 is A#, and so on)
 * 
 * @param frequency in hertz
 * @return struct TunerNote* pointer to the tunernote struct
 */
struct TunerNote* getNote(uint16_t frequency);

uint8_t* generateDisplayMessage();

#endif
