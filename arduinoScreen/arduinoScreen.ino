// Import the Liquid Crystal library
//https://docs.arduino.cc/learn/electronics/lcd-displays
//^link used to wire LCD Screen
#include <LiquidCrystal.h>;
//Initialise the LCD with the arduino. LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // Switch on the LCD screen
  lcd.begin(16, 2);
  // Print these words to my LCD screen
  //lcd.print("HELLO WORLD!");
}

void loop() {
  lcd.print("Hello, world!");
  delay(5000);
  lcd.clear();
  lcd.print("SSH is better");
  delay(5000);
  lcd.clear();

}
