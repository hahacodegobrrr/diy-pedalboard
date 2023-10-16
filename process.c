#include "process.h"
#include "delay.h"

//might have to change this later
const float INC_BIAS = 1.66; //incoming positive bias voltage of the signal

float* p_signalBuffer; //points to a 1-second long signal buffer
unsigned int signalLength; //in samples
unsigned int signalHead; //position of current sample read/write in buffer

float* initProcessing() {
  signalLength = (int)(getSampleRate() * sizeof(float));
  p_signalBuffer = malloc(signalLength);
  return p_signalBuffer;
}

float processSample(float incomingSignal) {
  incomingSignal -= INC_BIAS;

  //update signal buffer
  p_signalBuffer[signalHead * sizeof(float)] = incomingSignal;
  
  incomingSignal = applyDelayToSample(incomingSignal);

  //push signalhead to next buffer position
  signalHead++;
  if (signalHead >= signalLength)
    signalHead = 0;
  
  return incomingSignal;
}