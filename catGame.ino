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

int catRow = 1; //start position at the bottom row --> top row == 0
int obsCol = 15; //place at the end of the screen 

int obsRow = 1; //obstacle alwys on bottom row 

// Display ref:
// Columns: 0 1 2 3 ... 15
// Row 0:
// Row 1:  

bool didJump = false; //default to bottom row when button is not pressed 

unsigned long jumpStartTime; //storing the exact time the character jumps
const int jumpTime = 500; //jump duration -- ended up increading jump time so cat stays in the top row for longer as there was an issue with collison object

unsigned long lastMoveTime = 0;
const int obsSpeed = 300;

void setup() {
  lcd.init();
  lcd.backlight();

  lcd.createChar(0, catman);//initilise sprites in memory
  lcd.createChar(1, obstacle); 

  pinMode(button, INPUT_PULLUP); //INTPUT_PULLUP is the resistor that changes the voltage to determine high/low voltage movement on board 
  //high resistance = slows current --> no movement, low resistance = increases current --> movement
  
  lcd.setCursor(0, catRow); //initilise character to bottom row
  lcd.write(byte(0)); //prints the character out at position 0


  lcd.setCursor(obsCol, 1);
  lcd.write(byte(1));

  // lcd.clear();//clear old data when program is initilised
}

void loop() {
  // lcd.clear(); //need to clear screen to change character position 

  if (digitalRead(button) == LOW && !didJump){ //if button is pressed then

    delay(50); //need to add delay as there was an issue with the objects coliding
    // cat disappeared sometimes when it jumped over the object and returned back in the same cell
    //but then returned when button was pressed 

    // could have sped up the collisison object speed but that will not fix issues in the future for game speed mode
    didJump = true;
    jumpStartTime = millis(); // save start time to compare it to when player stops pressing button

    lcd.setCursor(0, catRow); //erase the character at it's current position
    lcd.print(" ");

    catRow = 0; // move char to the top row

    lcd.setCursor(0, catRow);//print char
    lcd.write(byte(0));




  }// do not need to include else if as player can spam the button so the char stays on the top row to avoid obstacles

//when cat has jumped, put it back on row 1 
  if (didJump && millis() - jumpStartTime > jumpTime){
    //if the char did not jump and the (duration of the jump - start jump time) > jump time, then the current is high, which means that the jump is over 
    // therefore, clear position 
      lcd.setCursor(0, catRow);
      lcd.print(" ");

      catRow = 1; // put character back to bottom row
      lcd.setCursor(0, catRow);
      lcd.write(byte(0));

      didJump = false; // initilise jump condition again 



  }

// moving obstacle on screen
  if (millis() - lastMoveTime > obsSpeed){
    lastMoveTime = millis();

      lcd.setCursor(obsCol, obsRow); //clear current position of obstacle on the bottom row
      lcd.print(" ");

      obsCol--; //decrement position of obstacle --> move to the left

      if (obsCol < 0){//ensure that the obstacle doesn't leave the screen 
        obsCol = 15; //take it back to beginning position
      }

      lcd.setCursor(obsCol, obsRow);//print it out to new position
      lcd.write(byte(1));
  }


      //detecting collision
      if (obsCol == 0 && catRow == obsRow){
    
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("RIP...");
        lcd.setCursor(4, 1);
        lcd.print("Restart?");

        while (digitalRead(button) == HIGH){//end game
        }

        delay(200);
        
        //reset position variables
        catRow = 1;
        obsCol = 15;
        obsRow = 1;
        didJump = false;

        jumpStartTime = 0;
        lastMoveTime = millis();


        lcd.clear();


        lcd.setCursor(0, catRow);
        lcd.write(byte(0));


        lcd.setCursor(obsCol, obsRow);
        lcd.write(byte(1));

      }

}