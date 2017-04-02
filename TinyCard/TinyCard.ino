
#include "SSD1306_minimal.h"
#include <avr/pgmspace.h>
#include "Menu-1-select.h"
#include "Menu-2-select.h"
#include "Menu-3-select.h"
#include "ContactInfo.h"
#include "Help.h"

const int topButton = 1;
const int leftButton = 3;
const int rightButton = 4;

int menuState = 0;
bool helpMenuShown = false;
bool contactInfoShown = false;

SSD1306_Mini oled;

void setup(){
  pinMode(rightButton, INPUT);
  pinMode(leftButton, INPUT);
  pinMode(topButton, INPUT);
  oled.init(0x3c);
  oled.clear();
  oled.startScreen();
  drawHelpScreen();
  helpMenuShown = true;
}

void loop() {

  if ((helpMenuShown || contactInfoShown) && (digitalRead(topButton) == HIGH || digitalRead(leftButton) == HIGH || digitalRead(rightButton) == HIGH)) {
     drawMenu();
     helpMenuShown = false;
     contactInfoShown = false;
  }
  
  else if (digitalRead(topButton) == HIGH) {
   switch (menuState) {
    case 0:
      drawHelpScreen();
      helpMenuShown = true;
      break;
    case 1:
      drawContactInfoScreen();
      contactInfoShown = true;
      break;
    case 2:
      break;
   }
  }

  else if (digitalRead(leftButton) == HIGH) {
     if (menuState == 0) {
        menuState = 0;
     }

     else {
        menuState--;
     }
     
     drawMenu();
  }

  else if (digitalRead(rightButton) == HIGH) {
      if (menuState == 2) {
        menuState = 2;
      }

      else {
        menuState++;
      }
      
      drawMenu();
  }
}

void drawMenu() {
  switch (menuState) {
    case 0:
      oled.drawImage(Menu1select, 0,0, 128, 8);
      break;
    case 1:
      oled.drawImage(Menu2select, 0,0, 128, 8);
      break;
    case 2:
      oled.drawImage(Menu3select, 0,0, 128, 8);
      break;
  }
}

void drawContactInfoScreen() {
  oled.drawImage(ContactInfo, 0,0, 128, 8);
}

void drawHelpScreen() {
  oled.drawImage(Help, 0,0, 128, 8);
}

