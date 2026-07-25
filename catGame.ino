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

byte obstacle[8] = {
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110,
  B01110,
  B00100
};

const int button = 2; //pin connected to 2 on board 

int charPos = 1; //start position at the bottom row --> top row == 0
int obsPos = 15; //place at the end of the screen 

bool didJump = false; //default to bottom row when button is not pressed 

unsigned long jumpStartTime; //storing the exact time the character jumps
const int jumpTime = 400; //jump duration 

unsigned long lastMoveTime = 0;
const int obsSpeed = 300;

void setup() {
  lcd.init();
  lcd.backlight();

  lcd.createChar(0, catman);//initilise sprites in memory
  lcd.createChar(1, obstacle); 

  pinMode(button, INPUT_PULLUP); //INTPUT_PULLUP is the resistor that changes the voltage to determine high/low voltage movement on board 
  //high resistance = slows current --> no movement, low resistance = increases current --> movement
  
  lcd.setCursor(0, charPos); //initilise character to bottom row
  lcd.write(byte(0)); //prints the character out at position 0


  lcd.setCursor(obsPos, 1);
  lcd.write(byte(1));

  // lcd.clear();//clear old data when program is initilised
}

void loop() {
  // lcd.clear(); //need to clear screen to change character position 

  if (digitalRead(button) == LOW && !didJump){ //if button is pressed then
    didJump = true;
    jumpStartTime = millis(); // save start time to compare it to when player stops pressing button

    lcd.setCursor(0, charPos); //erase the character at it's current position
    lcd.print(" ");

    charPos = 0; // move char to the top row

    lcd.setCursor(0, charPos);//print char
    lcd.write(byte(0));




  }// do not need to include else if as player can spam the button so the char stays on the top row to avoid obstacles

  if (didJump && millis() - jumpStartTime > jumpTime){
    //if the char did not jump and the (duration of the jump - start jump time) > jump time, then the current is high, which means that the jump is over 
    // therefore, clear position 
      lcd.setCursor(0, charPos);
      lcd.print(" ");

      charPos = 1; // put character back to bottom row
      lcd.setCursor(0, charPos);
      lcd.write(byte(0));

      didJump = false; // initilise jump condition again 



  }


  if (millis() - lastMoveTime > obsSpeed){
    lastMoveTime = millis();

      lcd.setCursor(obsPos, 1); //clear current position of obstacle on the bottom row
      lcd.print(" ");

      obsPos--; //decrement position of obstacle --> move to the left

      if (obsPos < 0){//ensure that the obstacle doesn't leave the screen 
        obsPos = 15; //take it back to beginning position
      }

      lcd.setCursor(obsPos, 1);//print it out to new position
      lcd.write(byte(1));
  }

  // lcd.setCursor(charPos, 0);
  // lcd.write(byte(0));

  // charPos++;//increment character by one space

  // if (charPos > 15){
  //   charPos = 0; // set character back to starting pos if it goes to the end of the display


  // }

  // delay(300);





















}