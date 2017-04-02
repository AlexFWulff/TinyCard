
//These two get the display to work
#include "SSD1306_minimal.h"
#include <avr/pgmspace.h>

//These are all images
#include "wall.h"
#include "blank.h"
#include "BirdSprite.h"
#include "WallFunctions.h"
#include "GameOver.h"

//This is to get pin change interrupts
#include "avr/interrupt.h"

const int topButton = 1;
const int leftButton = 3;
const int rightButton = 4;
const int wallArraySize = 10;
const int wallSpacing = 2;

//Stores all the walls on screen
Wall wallArray[wallArraySize];

//All global variables to get the game to work
long lastMillis = millis();
volatile long buttonDebounceTime = millis();
bool alreadyDoneThisCycle = false;
volatile bool needSpriteRefresh = false;
volatile int lastSpritePosition = 0;
long gameCounter = 0;
int arrayCounter = 0;

void setup(){
  // turns on pin change interrupts
  GIMSK = 0b00100000;
  //        xx543210 - turns on pin change interrupts on inputs 3 and 4 (not pins 3 and 4)
  PCMSK = 0b00011000;
  sei();

  pinMode(rightButton, INPUT);
  pinMode(leftButton, INPUT);
  pinMode(topButton, INPUT);

  //Initializes the OLED
  oled.init(0x3c);
  oled.clear();
  oled.startScreen();
  oled.drawImage(BirdSprite,0,currentSpritePosition,8,1);
}

void loop() {
  //Check to see if we need to re-draw the bird
  checkSpriteRefresh();

  //This little function will gradually speed up the game over time.
  //300ms is the initial delay between each time the walls move.
  //Every second (millis()/1000) 10 milliseconds are subtracted from this time
  long currentTiming = 300L - 10L * (millis() / 1000L);

  //We never want the interval to go below 50ms
  if (currentTiming < 50) {
    currentTiming = 50;
  }
  
  //If the last time the walls were moved subracted from the current time is equal to the time interval,
  //then it's time to move the walls.
  if (millis()-lastMillis > currentTiming) {
    moveWalls();
    lastMillis = millis();
    //Game counter is used to determine the spacing between the walls
    gameCounter++;
    //AlreadyDoneThisCycle is to prevent the next if statement to run infinitely before the next wall cycle
    alreadyDoneThisCycle = false;
  }

  //Every four wall cycles, we want to add in a new wall (because an old one has since gone off screen).
  //Calling % 4 == 0 checks to see if gameCounter is divisible by four
  if (gameCounter % wallSpacing == 0 && !alreadyDoneThisCycle) {
  	  //arrayCounter cycles through each of the walls in the global array, making sure that this function overwrites
  	  //the wall that has been on-screen for the most amount of time
      if (arrayCounter > wallArraySize-1) {
        arrayCounter = 0;
      }
      
      //These lines birth a new wall and set its column to the farthest column to the right
      Wall toAddToArray;
      toAddToArray.column = 15;
      wallArray[arrayCounter] = toAddToArray;
      arrayCounter++;
      alreadyDoneThisCycle = true;
  }
}

//Displays the game over screen forever
void gameOver() {
  while(1==1) {
    oled.drawImage(GameOver,0,0,128,8);
    delay(1000);
  }
}

//We're not updating the bird in the ISR because it leads to weird behavior. Interrupting drawing routines
//of images and such can lead to blocks in wrong places. Try putting this in the ISR and see what happens!
void checkSpriteRefresh() {
  if (needSpriteRefresh) {
    oled.drawImage(BirdSprite,0,currentSpritePosition,8,1);
    oled.drawImage(blank,0,lastSpritePosition,8,1);
    needSpriteRefresh = false;
  }
}

//This is called every time the state of pins 3 or 4 change (both on rising and falling edges),
//regardless of what's going on. It simulates the effect of having multiple threads without actually having them!
//Every time an update is needed, needSpriteRefresh is set true. Whenever checkSpriteRefresh() is called, the position is updated!
ISR(PCINT0_vect) {
  //If the left button is pressed and we're sure it's not an error, then move the bird up one if it can go up
  if (digitalRead(leftButton)==HIGH && currentSpritePosition > 0 && millis() - buttonDebounceTime > 200L) {
    lastSpritePosition = currentSpritePosition;
    currentSpritePosition--;
    needSpriteRefresh = true;
    buttonDebounceTime = millis();
  }
  
  //If the right button is pressed and we're sure it's not an error, then move the bird down one if it can go up
  if (digitalRead(rightButton)==HIGH && currentSpritePosition < 7 && millis() - buttonDebounceTime > 200L) {
    lastSpritePosition = currentSpritePosition;
    currentSpritePosition++;
    needSpriteRefresh = true;
    buttonDebounceTime = millis();
  }
}

//This is a very important function responsible for moving the walls one column to the left every time this is called.
void moveWalls() {
  //A for loop is used to cycle through all four walls in the array
  for (int i = 0; i < wallArraySize; i++) {
    Wall gotWall = wallArray[i];

    //Decrementing the column makes a wall move to the left when it's displayed
    gotWall.column--;

    //This if statement checks to see if the wall is between 0-15, otherwise it will be off-screen
    if (!(gotWall.column < 0 || gotWall.column > 16)) {
      //Replace the old wall with the new one
      wallArray[i] = gotWall;
      //Check to see if we need to re-display the bird on-screen
      checkSpriteRefresh();
      //Finally, draw the wall in it's new position!
      drawWall(gotWall);
      //Check again to see if the bird needs to be re-drawn
      checkSpriteRefresh();
      //Draw a blank wall where the old wall used to be, essentially erasing it
      drawWallSequence(gotWall.column+1, 0,0,0,0,0,0,0,0);
    }

    //This statement will clear a wall that used to be in column 0
    //Waiting 8 seconds after start is necessary so the other three walls that don't 
    //start on the screen (column = 0 initially) don't make the bird flicker.
    if (gotWall.column < 0 && millis() > 8000L) {
      checkSpriteRefresh();
      drawWallSequence(0, 0,0,0,0,0,0,0,0);
    }

    //Checking for game over. Based on the hole position, we can determine where the bird is supposed to be.
    //If it's not there, then that means that the bird is where the wall is, so it's game over.
    if (gotWall.column == 0 && millis() > 8000L) {
      switch (gotWall.holePosition) {
        case 0: 
          if (currentSpritePosition != 0 && currentSpritePosition != 1) {
            gameOver();
          }
        break; 
        case 1: 
          if (currentSpritePosition != 1 && currentSpritePosition != 2) {
            gameOver();
          }
        break;
        case 2: 
          if (currentSpritePosition != 2 && currentSpritePosition != 3) {
            gameOver();
          }
        break;
        case 3: 
          if (currentSpritePosition != 3 && currentSpritePosition != 4) {
            gameOver();
          }
        break;
        case 4: 
          if (currentSpritePosition != 4 && currentSpritePosition != 5) {
            gameOver();
          }
        break;
        case 5: 
          if (currentSpritePosition != 5 && currentSpritePosition != 6) {
            gameOver();
          }
        break;
        case 6: 
          if (currentSpritePosition != 6 && currentSpritePosition != 7) {
            gameOver();
          }
        break;
      }
    }
    
  }
}
