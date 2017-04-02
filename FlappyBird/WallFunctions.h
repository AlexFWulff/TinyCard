//Created by Alex Wulff: www.AlexWulff.com
SSD1306_Mini oled;
volatile int currentSpritePosition = 0;

//Data about the walls are stored in a struct to make accessing data easier.
struct Wall {
  //The position of the hole is randomly generated.
  int holePosition = random(0, 7);
  int column = 0;
};

//This function draws a wall at a specific column. a-f are the rows to draw individual blocks.
//drawWallSequence(0, 1,1,1,1,1,1,1,1) would yield a wall with no gaps
//drawWallSequence(0, 0,0,0,0,0,0,0,0) would yield a blank wall
void drawWallSequence(int column, int a, int b, int c, int d, int e, int f, int g, int h) {
  
  //If the wall isn't within the 16 available columns, then we don't want to try and draw it.
  if (column < 0 || column > 15) {
    return;
  }

  //We need to map our 16 column system to the 128 pixels that the display understands
  int origianlColumn = column;
  column = column * 8;
  
  //If a one is passed for the specific row, it means draw a wall. Otherwise, draw a blank space.
  //drawImage() takes 5 arguments: (variable to draw, x position, y position, image width, image height)
  //Both the y position and image height are in increments of 8, so the height of our walls is actually 8 and not 1
  //'wall' is the wall bitmap included in the folder. 'blank' is just a blank 8x8 tile to overwrite what's on the screen
  if (a == 1) {
    oled.drawImage(wall,column,0,8,1);
  } else {
    oled.drawImage(blank,column,0,8,1);
  }

  if (b == 1) {
    oled.drawImage(wall,column,1,8,1);
  } else {
    oled.drawImage(blank,column,1,8,1);
  }

  if (c == 1) {
    oled.drawImage(wall,column,2,8,1);
  } else {
    oled.drawImage(blank,column,2,8,1);
  }

  if (d == 1) {
    oled.drawImage(wall,column,3,8,1);
  } else {
    oled.drawImage(blank,column,3,8,1);
  }

  if (e == 1) {
    oled.drawImage(wall,column,4,8,1);
  } else {
    oled.drawImage(blank,column,4,8,1);
  }

  if (f == 1) {
    oled.drawImage(wall,column,5,8,1);
  } else {
    oled.drawImage(blank,column,5,8,1);
  }

  if (g == 1) {
    oled.drawImage(wall,column,6,8,1);
  } else {
    oled.drawImage(blank,column,6,8,1);
  }

  if (h == 1) {
    oled.drawImage(wall,column,7,8,1);
  } else {
    oled.drawImage(blank,column,7,8,1);
  }

  //We don't want our bird to disappear when a wall goes over it, so it's re-drawn every time there is a wall at column 0
  if (origianlColumn == 0 || origianlColumn > 15) {
    oled.drawImage(BirdSprite,0,currentSpritePosition,8,1);
  }
}

//drawWall() takes a Wall struct and draws it in its proper column.
//The holePosition indicates where the 2-block hole is in the wall.
//As the hole is two blocks wide, there are only 7 possible positions.
void drawWall(Wall wall) {
  switch (wall.holePosition) {
    case 0: drawWallSequence(wall.column, 0,0,1,1,1,1,1,1); break;
    case 1: drawWallSequence(wall.column, 1,0,0,1,1,1,1,1); break;
    case 2: drawWallSequence(wall.column, 1,1,0,0,1,1,1,1); break;
    case 3: drawWallSequence(wall.column, 1,1,1,0,0,1,1,1); break;
    case 4: drawWallSequence(wall.column, 1,1,1,1,0,0,1,1); break;
    case 5: drawWallSequence(wall.column, 1,1,1,1,1,0,0,1); break;
    case 6: drawWallSequence(wall.column, 1,1,1,1,1,1,0,0); break;
  }
}
