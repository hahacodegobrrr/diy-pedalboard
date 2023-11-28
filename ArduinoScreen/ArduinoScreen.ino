// Import the Liquid Crystal library
//https://docs.arduino.cc/learn/electronics/lcd-displays
//^link used to wire LCD Screen
#include <LiquidCrystal.h>
//Initialise the LCD with the arduino. LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);

const char SCREEN_LENGTH = 16;
const char MESSAGE_SIZE = SCREEN_LENGTH * 2;

char lastMessage[MESSAGE_SIZE];

void setup() {
  // Switch on the LCD screen
  lcd.begin(16, 2);
  Serial.begin(9600);
  // Print these words to my LCD screen
  //lcd.print("HELLO WORLD!");
}

void loop() {
  if (Serial.available() >= MESSAGE_SIZE + 1) {
    while(Serial.read() != 0); //ensure read begins at front of message
    int i;
    int r;
    for (r = 0; r < 1; r++) {
      for (i = 0; i < SCREEN_LENGTH; i++) {
        char c = Serial.read();
        if (c != lastMessage[i]) {
          lcd.setCursor(i, r);
          lcd.print(c);
          lastMessage[i] = c;
        }
      }
    }
  }
}
