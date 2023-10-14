#include <stdlib.h>
#include "delay.h"
#include "bitcrusher.h"

float* p_signal; //ptr to array holding signal samples
unsigned int signalLength; //in samples
unsigned short delayTime; //in ms
unsigned char delayFeedback = 1; //% of signal to re-feed

unsigned int signalHead; //the point where the signal is being written

void updateParameters(unsigned short a, unsigned char b);

void init() {
    updateParameters(0, 0);
}

void updateParameters(unsigned short delayTimeMs, unsigned char feedback) {
    delayTime = delayTimeMs;
    signalLength = (int)((float)delayTime / 1000 * getSampleRate() * sizeof(float));
    p_signal = (float*)malloc(signalLength);
    if (feedback != delayFeedback) {
        signalHead = 0;
        feedback = delayFeedback;
    }



}

float writeSample(float input) {
    //get the current sample
    float* p_currentSample = p_signal + signalHead * sizeof(float);
    float currentSample = *p_currentSample;

    //attenuate existing data and add to input
    currentSample *= (float)delayFeedback / 100;
    currentSample += input;

    //update signalhead and send sample back to caller
    signalHead++;
    if (signalHead >= signalLength)
        signalHead = 0;

    return currentSample;
}