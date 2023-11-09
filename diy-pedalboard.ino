

const int MAX_MESSAGE_SIZE = 32;

void setup(){
  Serial.begin(9600);
}

int iter = 0;

void loop(){
  char message[MAX_MESSAGE_SIZE];
  int i;
  for (i = 0; i < MAX_MESSAGE_SIZE; i++) {
    message[i] = 'a' + (iter++ % 26);
  }
  sendMessageToScreen(&message[0]);
  delay(1000);
}

void sendMessageToScreen(char* text) {
  Serial.write(text, sizeof(MAX_MESSAGE_SIZE));
}