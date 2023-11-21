#include "FSM.h"
#include <stdio.h>


int main() {

  printf("hello fuckers\n");


  struct State* currState = getNextState(0);

  int i;
  for (i = 0; i < 10; i++) {
    printf("%d, %d\n", currState, (*currState).id);
    currState = getNextState(currState);
  }





  return 0;
}