//include C files
extern "C" {
    #include "process.h"
    #include "delay.h"
    #include "bitcrusher.h"
}

//pinout
const char INPUT_PIN = A0;

unsigned long lastSampleTime = 0;

void setup() {
    initDelay();
}

void loop() {
    unsigned long t = micros();
    if (t - lastSampleTime > (float)1000000 / getSampleRate() || t < lastSampleTime) {
        float sample = analogRead(INPUT_PIN);
        //might need to do extra processing here if the input signal is riding a dc voltage
        processSample(sample);
        lastSampleTime = t;
    }
}