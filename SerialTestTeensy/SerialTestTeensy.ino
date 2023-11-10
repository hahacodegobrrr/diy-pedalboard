
void setup() {
  Serial1.begin(9600);
}

bool light = false;

void loop() {
  Serial1.write(light);
  Serial.println(light);
  light = !light;
  delay(1000);
}