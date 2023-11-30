#ifndef FSM_H
#define FSM_H
#include <stdint.h>

struct State {
  uint8_t id;
  uint16_t knob1;
  uint16_t knob2;
  uint16_t knob3;
  struct State* nextState;
};

struct State* getNextState(struct State* currentState);

#endif