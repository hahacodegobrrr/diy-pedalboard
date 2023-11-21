#include "FSM.h"
#include <stdint.h>

//tuner, chorus, delay, freeverb

struct State stateTable[] = {
  {.id = 0, .knob1 = 0, .knob2 = 0, .knob3 = 0, .nextState = &stateTable[1]},
  {.id = 1, .knob1 = 0, .knob2 = 0, .knob3 = 0, .nextState = &stateTable[2]},
  {.id = 2, .knob1 = 0, .knob2 = 0, .knob3 = 0, .nextState = &stateTable[3]},
  {.id = 3, .knob1 = 0, .knob2 = 0, .knob3 = 0, .nextState = &stateTable[0]}
};

struct State* getNextState(struct State* currentState) {
  if (currentState == 0)
    return &stateTable[0];
  return (*currentState).nextState;
}