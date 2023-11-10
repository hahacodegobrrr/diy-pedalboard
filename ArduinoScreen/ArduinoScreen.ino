// Import the Liquid Crystal library
//https://docs.arduino.cc/learn/electronics/lcd-displays
//^link used to wire LCD Screen
#include <LiquidCrystal.h>
//Initialise the LCD with the arduino. LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const char MAX_MESSAGE_SIZE = 32;


void setup() {
  // Switch on the LCD screen
  lcd.begin(16, 2);
  // Print these words to my LCD screen
  //lcd.print("HELLO WORLD!");
}

void loop() {

  if (Serial.available() >= MAX_MESSAGE_SIZE) {
    int i;
    char message[MAX_MESSAGE_SIZE + 1];
    message[MAX_MESSAGE_SIZE] = '\0';
    for (i = 0; i < MAX_MESSAGE_SIZE; i++) {
      message[i] = Serial.read();
    }
    lcd.clear();
    lcd.print(message);
  }
}
