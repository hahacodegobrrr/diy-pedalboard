void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    digitalWrite(13, Serial.read());
  }
  delay(100);
}