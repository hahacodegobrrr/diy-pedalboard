

const int MAX_MESSAGE_SIZE = 32;

long lastScreenUpdate;
const char screenUpdatePeriod = 100; //in ms

void setup(){
  Serial.begin(9600);
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
  Serial.write(text, sizeof(MAX_MESSAGE_SIZE));
}