#include "process.h"
#include "delay.h"

//probably have to change this later
const float INC_BIAS = 1.66; //incoming positive bias voltage of the signal

float processSample(float incomingSignal) {
  incomingSignal -= INC_BIAS;



  incomingSignal = applyDelayToSample(incomingSignal);

  return incomingSignal;
}