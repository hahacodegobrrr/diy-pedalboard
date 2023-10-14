#include <stdlib.h>
#include "delay.h"
#include "bitcrusher.h"

float* p_signal; //ptr to array holding signal samples
int signalLength; //in samples
unsigned short delayTime; //in ms
unsigned char delayFeedback; //% of signal to re-feed

void init() {
    updateParameters(0, 0);
}

void updateParameters(unsigned short delayTimeMs, unsigned char feedback) {
    delayTime = delayTimeMs;
    
}