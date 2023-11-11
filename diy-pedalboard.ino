

const int MAX_MESSAGE_SIZE = 32;

unsigned int lastScreenUpdate;
const char screenUpdatePeriod = 1000; //in ms

void setup(){
  Serial1.begin(9600);
  lastScreenUpdate = millis();
}

//for testing
int iter = 0;

void loop(){
  if (lastScreenUpdate + screenUpdatePeriod <= millis()) {
    char message[MAX_MESSAGE_SIZE];
    int i;
    for (i = 0; i < MAX_MESSAGE_SIZE; i++) {
      message[i] = 'a' + (iter++ % 26);
    }
    sendMessageToScreen(&message[0]);
    lastScreenUpdate = millis();
  }
}

void sendMessageToScreen(char* text) {
  Serial1.write(text, sizeof(MAX_MESSAGE_SIZE));
}