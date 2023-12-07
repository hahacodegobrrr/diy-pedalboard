#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

AudioInputI2S audioInput;
AudioAnalyzeNoteFrequency notefreq;
AudioAnalyzeFFT1024 fft;
AudioSynthWaveformSine sinewave1;
AudioSynthWaveformSine sinewave2;
AudioSynthWaveformSine sinewave3;
AudioMixer4 mixer1;
AudioMixer4 mixer2;
AudioOutputI2S audioOutput;
AudioControlSGTL5000 audioShield;

const uint16_t FFT_BIN_WIDTH = 43; //hz

AudioConnection patchCord0(audioInput, 0, notefreq, 0);
AudioConnection patchCord1(audioInput, 0, fft, 0);
AudioConnection patchCord2(sinewave1, 0, mixer1, 0);
AudioConnection patchCord3(sinewave2, 0, mixer1, 1);
AudioConnection patchCord4(mixer1, 0, mixer2, 0);
AudioConnection patchCord5(sinewave3, 0, mixer2, 1);
AudioConnection patchCord6(mixer2, audioOutput);

void setup() {
  AudioMemory(30);
  Serial.begin(9600);
  audioShield.enable();
  audioShield.inputSelect(AUDIO_INPUT_LINEIN);
  audioShield.volume(0.5);
  notefreq.begin(.15);
  fft.windowFunction(AudioWindowHanning1024);
}

void loop() {
  if (notefreq.available()) {
    float mostProminentFreq = notefreq.read();
    Serial.println(mostProminentFreq);
    while (mostProminentFreq < 80) {
      mostProminentFreq *= 2;
    }
    while (mostProminentFreq > 1000) {
      mostProminentFreq /= 2;
    }
    // sinewave1.frequency(mostProminentFreq * 1.6666666);
    sinewave2.frequency(mostProminentFreq);
    sinewave3.frequency(mostProminentFreq * 2);
  }

  if (fft.available()) {
    int16_t highestBin = -1;
    float highestIntensity = 0.0008;
    uint8_t i;
    for (i = 1; i < 100; i++) {
      float f = fft.read(i);
      if (f > highestIntensity) {
        highestIntensity = f;
        highestBin = i;
      }
    }
    uint16_t mostProminentFreq = highestBin * FFT_BIN_WIDTH;
    Serial.print(highestBin);
    Serial.print('=');
    Serial.println(highestIntensity);
    if (highestBin == -1) {
      sinewave1.frequency(20);
      sinewave2.frequency(20);
      sinewave3.frequency(20);
      return;
    }
  }
}