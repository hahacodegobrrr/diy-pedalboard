// Delay demonstration example, Teensy Audio Library
//   http://www.pjrc.com/teensy/td_libs_Audio.html
// 
// Creates a chirp on the left channel, then
// three delayed copies on the right channel.
//
// Requires the audio shield:
//   http://www.pjrc.com/store/teensy3_audio.html
//
// This example code is in the public domain.

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

const int input = AUDIO_INPUT_LINEIN;

// GUItool: begin automatically generated code
AudioInputI2S            audioInput;
AudioEffectEnvelope      envelope1;      //xy=232,156
AudioEffectDelay         delay1;         //xy=393,238
AudioMixer4              mixer1;         //xy=532,205
AudioOutputI2S           i2s1;           //xy=611,61
AudioConnection          patchCord1(audioInput, envelope1);
AudioConnection          patchCord2(envelope1, delay1);
AudioConnection          patchCord3(envelope1, 0, i2s1, 0);
AudioConnection          patchCord4(delay1, 0, mixer1, 0);
AudioConnection          patchCord5(delay1, 1, mixer1, 1);
AudioConnection          patchCord6(delay1, 2, mixer1, 2);
AudioConnection          patchCord7(audioInput, 0, mixer1, 3);
AudioConnection          patchCord8(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=195,272
// GUItool: end automatically generated code

void setup() {
  // allocate enough memory for the delay
  AudioMemory(1000);
  
  // enable the audio shield
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(input);
  sgtl5000_1.volume(0.5);
  

  // create 3 delay taps, which connect through a
  // mixer to the right channel output
  delay1.delay(1, 1000);
}

void loop() {
  envelope1.noteOn();
  delay(500);
  envelope1.noteOff();
  delay(50);
}