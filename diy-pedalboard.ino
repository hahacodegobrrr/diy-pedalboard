#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

const int input = AUDIO_INPUT_LINEIN;

AudioInputI2S audioInput;
AudioAnalyzeFFT1024 fft;
AudioOutputI2S audioOutput;

AudioConnection patchCord(audioInput, 0, fft, 0);

AudioControlSGTL5000 audioShield;

void setup(){
  AudioMemory(12);

  audioShield.enable();
  audioShield.inputSelect(input);
  audioShield.volume(0.5);

  fft.windowFunction(AudioWindowHanning1024);

  Serial.begin(9600);
}

void loop(){

  if (fft.available()) {
    int i;
    for (i = 0; i < 40; i++) {
      float n = fft.read(i);
      if (n >= 0.01) {
        Serial.print(n);
      }
    }
  }
}

void sendDataToScreen(char* text) {
  Serial.write(text, sizeof(text));
}