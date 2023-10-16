//include C files
extern "C" {
  #include "process.h"
  #include "delay.h"
  #include "bitcrusher.h"
}

//pinout
const char INPUT_PIN = A0;
// const char OUTPUT_PIN = A1;

unsigned long lastSampleTime = 0;

void setup() {
  //configure pins
  pinMode(INPUT_PIN, INPUT);
  
  //initialize modules
  float* p_signalBuffer = initProcessing();
  initDelay();
}

void loop() {
  unsigned long t = micros();
  if (t - lastSampleTime > (float)1000000 / getSampleRate() || t < lastSampleTime) {
    float sample = analogRead(INPUT_PIN);
    float output = processSample(sample);
    //write to speaker here
    //h bridge to switch polarity, then write wave amplitude?
    lastSampleTime = t;
  }
}