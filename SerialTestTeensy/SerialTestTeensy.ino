

const int MESSAGE_SIZE = 32;

unsigned int lastScreenUpdate;
const unsigned int screenUpdatePeriod = 1000; //in ms

void setup(){
  Serial1.begin(9600);
  lastScreenUpdate = millis();
}

//for testing
int iter = 0;

void loop(){
  unsigned int now = millis();

  if (lastScreenUpdate + screenUpdatePeriod <= now) {
    //test code for writing to lcd screen
    char message[MESSAGE_SIZE];
    int i;
    for (i = 0; i < MESSAGE_SIZE; i++) {
      message[i] = '0' + (iter % 10);
    }
    sendMessageToScreen(&message[0], MESSAGE_SIZE);
    lastScreenUpdate = now;
    iter++;
  }
}

void sendMessageToScreen(char* p_text, int length) {
  Serial1.write(0); //signal beginning of message
  Serial1.write(p_text, length);
}