// Freeverb - High quality reverb effect
//
//
// The SD card may connect to different pins, depending on the
// hardware you are using.  Uncomment or configure the SD card
// pins to match your hardware.
//
// Data files to put on your SD card can be downloaded here:
//   http://www.pjrc.com/teensy/td_libs_AudioDataFiles.html
//
// This example code is in the public domain.

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

const int input = AUDIO_INPUT_LINEIN;

// GUItool: begin automatically generated code
AudioMixer4              mixer1;         //xy=332,167
AudioEffectFreeverb      freeverb1;      //xy=497,105
AudioMixer4              mixer2;         //xy=650,190
AudioInputI2S            audioInput;
AudioOutputI2S           i2s1;           //xy=815,198
AudioConnection          patchCord1(audioInput, 0, mixer1, 0);
AudioConnection          patchCord2(audioInput, 1, mixer1, 1);
AudioConnection          patchCord3(mixer1, freeverb1);
AudioConnection          patchCord4(mixer1, 0, mixer2, 1);
AudioConnection          patchCord5(freeverb1, 0, mixer2, 0);
AudioConnection          patchCord6(mixer2, 0, i2s1, 0);
AudioConnection          patchCord7(mixer2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=236,248
// GUItool: end automatically generated code


void setup() {
  Serial.begin(9600);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(10);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(input);
  sgtl5000_1.volume(0.5);

  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.5);
  mixer2.gain(0, 0.9); // hear 90% "wet"
  mixer2.gain(1, 0.1); // and  10% "dry"
}

void loop() {}