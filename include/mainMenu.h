#ifndef MAINMENU_H
#define MAINMENU_H

class MainMenu {
  DisplayControl &displayLed;
  TimerMenu cfTimer(displayLed);
  char displayText[6];
  enum MenuMode {
    MENUSTART = 0,
    TIMER = 1,
    CONFIG = 2
  }menuMode;

public:
  MainMenu(DisplayControl &displayToAttach);
  
  void selectAction();
  void menuAction();
  void incrementAction();
  void decrementAction();
  
  void setup();
  void loop();

private:
  void changeMenu();
  void selectMenu();
  void displayMenu();

};

#endif

