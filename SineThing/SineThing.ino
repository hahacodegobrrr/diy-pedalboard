#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

AudioInputI2S audioInput;
AudioAnalyzeNoteFrequency notefreq;
AudioSynthWaveformSine sinewave;
AudioOutputI2S audioOutput;
AudioControlSGTL5000 audioShield;

AudioConnection patchCord0(audioInput, 0, notefreq, 0);
AudioConnection patchCord2(sinewave, 0, audioOutput, 0);

void setup() {
  AudioMemory(30);
  audioShield.enable();
  audioShield.inputSelect(AUDIO_INPUT_LINEIN);
  audioShield.volume(0.5);
  notefreq.begin(.15);
}

void loop() {
  if (notefreq.available()) {
    float note = notefreq.read();
    Serial.println(note);
    sinewave.frequency(note * 2);  
  }
}