#pragma once

/*
    initializes delay module
*/
void initDelay();

/*
    updates parameters of delay module
    delayTime -> duration of delay, in ms
    feedback -> runs from 0 to 100, percentage of feedback to add to signal
*/
void updateParameters(unsigned short delayTimeMs, unsigned char feedback);

/*
    applies the delay effect to the incoming sample
    input -> the incoming sample
    returns input sample with the delay effect applied
*/
float applyDelayToSample(float input);