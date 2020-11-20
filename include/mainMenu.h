#ifndef MAINMENU_H
#define MAINMENU_H

#include "DisplayControl.h"
#include "TimerMenu.h"

class MainMenu {
  DisplayControl &displayLed;
  TimerMenu cfTimer;
  char displayText[6];
  enum MenuMode {
    MENUSTART = 0,
    TIMER = 1,
    CONFIG = 2
  }activeMenu;
  MenuMode menuModeDisplayed;
  const char* timerName = "  Vod "; //Resembles WOD on 7-seg
  const char* configName = "  COnF"; // config

public:
  MainMenu(DisplayControl &displayToAttach);
  
  void returnAction();
  void selectAction();
  void menuAction();
  void incrementAction();
  void decrementAction();
  
  void setup();
  void loop();

private:
  void selectMenu();
  void displayMenu();

};

#endif

