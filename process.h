#pragma once

/*
  initialize audio processing
  returns a pointer to the (readonly pls) audio processing buffer
*/
float* initProcessing();

/*
  processes an incoming sample
  incomingSignal -> the incoming sample
  returns the resulting signal to be played back through the speakers
*/
float processSample(float incomingSignal);