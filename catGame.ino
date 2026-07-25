/*
Author: Chloe Fu
Date: July 2026
Description: Simple LCD Cat Runner Game
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/*LCD cat runner game - using an Inter-Integrated Circuit
note* the I2C will make it easier to wire the parts to the board since we'll only need x4 wires to send data and control timing (when the button is pressed)

Core Functions for this program are:
1. Initlising sprite icons on the screen
2. Movement up 
3. Movement down
4. Obstacle moving on screen
5. Detecting a collision
6. Updating the players score
7. Displaying the players score
8. Ending the game
*/

/*Creating the sprites
The LCD is 16 columns x 2 rows --> 32 units

Each unit is 5 columns x 8 rows
When setting values on the screen, it goes by lcd.setCursor(column, row);

To create your own characters, Fill in 1 or 0, (ON or OFF) -- Binary representation
00000
00000
00000
00000
00000
00000
00000
00000

Display reference:
Columns: 0 1 2 3 ... 15
Row:     0
Row:     1
*/
LiquidCrystal_I2C lcd(0x27, 16, 2);

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
  B00101,
  B10101,
  B10111,
  B11100,
  B00100,
  B00100,
  B00100
};

const int button = 2; //button connected to pin 2 on Ardunio board 

int catRow = 1; //character starting position at the start 
int obsCol = 15; //obstacle starting postition at the end
int obsRow = 1; //obstacle set to stay on bottom row 
bool didJump = false; //character default to bottom row when button is not pressed 

unsigned long jumpStartTime; //storing the exact time the character jumps
const int jumpTime = 600; //jump duration -- ended up increasing jump time so cat stays in the top row for longer as there was an issue with collison **when character jumps, just before the object collides, it goes missing on the screen
// the jump time is also a conditon for how long the character can stay in the air for 
unsigned long lastMoveTime = 0; //variable to store when the character last moved 
const int obsSpeed = 300; //speed of obstacle, can increase for different difficulty

unsigned long gameStartTime = 0;
unsigned long score = 0;
bool gameStart = false;

/*Initilising sprites on the screen*/
void initSprites(){
  lcd.setCursor(0, catRow);
  lcd.write(byte(0)); 

  lcd.setCursor(obsCol, 1);
  lcd.write(byte(1));
}

/*Setup
1. First we need to innitilise the sprites in the LCD memory so it can keep track of the movements
   Character is stored in byte 0, 
   Obstacle is stored in byte 1
2. Then we need to initilise the button on the board: INTPUT_PULLUP is the resistor that changes the voltage to determine high/low voltage movement on board 
   High resistance = slows current --> no movement
   Low resistance = increases current --> movement
3. Next we initilise the sprites on the screen
4. Finally, we keep a record of the time in miliseconds as it's more precise than measuring in seconds (note: players can spam the button)
*/
void setup() {
  lcd.init();
  lcd.backlight();

  lcd.createChar(0, catman);
  lcd.createChar(1, obstacle); 

  pinMode(button, INPUT_PULLUP);

  initSprites();
  gameStartTime = millis();
}

/*Moving the character up
For this function, we need to link the button movement to the sprite

1. The condition statement goes as follows: if the button has LOW resistance, then the button has been pressed,
   when the button is pressed, then the character did jump.
2. Therefore, the game has started so we need to store the game starting time.
3. Now we have to erase the character at its starting postition & move it to the top row to reinact a jump movement 
   We also have to reprint the character at its new position
*/
void up(){ 
  if (digitalRead(button) == LOW && !didJump){ 
    if(!gameStart){
      gameStartTime = millis();
      gameStart = true;
    }

    didJump = true;
    jumpStartTime = millis();

    lcd.setCursor(0, catRow);
    lcd.print(" ");

    catRow = 0;

    lcd.setCursor(0, catRow);
    lcd.write(byte(0));
  }
}

/*Movement down
The character needs to move back down to its position, otherwise it'll be up in the air forever.

Therefore, we need to condition its movements by doing the following:
1. If the char did jump and has exceeded the allowed jump air time limit then clear its position and move it back to the bottom row
2. Then we need to initilise the jump condition again --> back to standstill (false)
*/
void down(){

  if (didJump && millis() - jumpStartTime > jumpTime){
      lcd.setCursor(0, catRow);
      lcd.print(" ");

      catRow = 1;
      lcd.setCursor(0, catRow);
      lcd.write(byte(0));

      didJump = false;
  }
}

/*Updating Score
millis() returns the number of ms when the board was powered on

It will be inacurate to calculate the score to include when the board turns on as the player could be afk for a while
therefore, we need to ensure that the score is only calculated once the player actualy starts the game (by pressing the button in the up function)

To update the score in a readable format, we convert it back to seconds --> 1s = 1000ms 
*/
void updateScore(){
  if(gameStart){
    score = (millis() - gameStartTime) / 1000;
  }
}

/*To display score on the screen
We need to ensure that the units displaying the real-time score is updated in real time.
Therefore, we need to empty the score cells and update them again to the current score.
*/
void displayScore(){

  lcd.setCursor(4,0);
  lcd.print("Score: ");

  lcd.setCursor(10,0);
  lcd.print("     ");

  lcd.setCursor(10,0);
  lcd.print(score);
}

/*Moving the obstacle accross the screen at a fixed speed
If the obstacle has shown movement (starting moevemt time - last time it moved) which is greater than the set duration of movement
then: store the current movement time, clear its current position and decrement the positon of the obstacle by 1 so it can go closer to the character

Then we can print out the obstacle on its new position.

We also need to ensure that the obstacle wraps around the screen so if it is less than position 0, we need to place it back to its starting position
e.g. position cannot be -1 as the display limits are 0,1,2,3....15 
*/
void moveObst(){
  if (millis() - lastMoveTime > obsSpeed){
    lastMoveTime = millis();

      lcd.setCursor(obsCol, obsRow);
      lcd.print(" ");

      obsCol--;

      if (obsCol < 0){
        obsCol = 15;
      }

      lcd.setCursor(obsCol, obsRow);
      lcd.write(byte(1));
  }
}

/*Detecting collision
If the obstacle is in the same column and row as the character, then the collision happend. Therefore, game is over.
*/
void detectCollision(){
        //detecting collision
      if (obsCol == 0 && catRow == obsRow){
        gameOver();
    }
}

/*Game over function
When the game is over, we need to clear the screen and initilise it with the game over message, score & nect player movement

The function will remain ended until the player presses the button which will result in a LOW resistance
Therfore, the while loop will continiously check if the button has been left alone (to keep the game in end state)

The delay is needed just in case another jump is acidentally triggered when the player starts the game again 
e.g. they may click the button twice (really fast) by accident when restarting game, which will cause the character to jump by accident 

When the game is over, we need to ensure that we reset the sprites back to its original positions and reset the timers to redo the score.
*/
void gameOver(){
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("GAME OVER!");
        lcd.setCursor(5, 1);
        lcd.print("+");
        lcd.print(score);
        lcd.print(" Pts");

        while (digitalRead(button) == HIGH){
        }

        delay(200);
        
        catRow = 1;
        obsCol = 15;
        obsRow = 1;
        didJump = false;

        jumpStartTime = 0;
        lastMoveTime = millis();

        lcd.clear();
        initSprites();
        gameStartTime = millis();
        score = 0;     
}

/*Main Game Loop*/
void loop() {
  up();
  down();
  moveObst();
  updateScore();
  displayScore();
  detectCollision();
}
