#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Creating character
// It's 8 rows x 5 columns 
// 1= ON, 0 = OFF --> B for binary representation
//   01010,
  // 01110,
  // 11111,
  // 00100,
  // 11111,
  // 10101,
  // 01010,
  // 10001

byte catman[8] = {
  B01010,
  B01110,
  B11111,
  B00100,
  B11111,
  B10101,
  B01010,
  B10001
};

int charPos = 0; //start position 

void setup() {
  lcd.init();
  lcd.backlight();

  lcd.createChar(0, catman);

  lcd.clear();
}

void loop() {
  lcd.clear(); //need to clear screen to change character position 

  lcd.setCursor(charPos, 0);
  lcd.write(byte(0));

  charPos++;//increment character by one space

  if (charPos > 15){
    charPos = 0; // set character back to starting pos if it goes to the end of the display


  }

  delay(300);





















}