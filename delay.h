#pragma once

/*
    initializes delay module
*/
void init();

/*
    updates parameters of delay module
    delayTime -> duration of delay, in ms
    feedback -> runs from 0 to 100, percentage of feedback to add to signal
*/
void updateParameters(unsigned short, unsigned char);

