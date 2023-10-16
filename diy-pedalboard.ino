//include C files
extern "C" {
  #include "process.h"
  #include "delay.h"
  #include "bitcrusher.h"
}

//pinout
const char INPUT_PIN = A0;
const char OUTPUT_PIN = A1;

unsigned long lastSampleTime = 0;

void setup() {
  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  initDelay();
}

void loop() {
  unsigned long t = micros();
  if (t - lastSampleTime > (float)1000000 / getSampleRate() || t < lastSampleTime) {
    float sample = analogRead(INPUT_PIN);
    float output = processSample(sample);
    writeSignalToSpeaker(output);
    lastSampleTime = t;
  }
}

void writeSignalToSpeaker(float output) {

}