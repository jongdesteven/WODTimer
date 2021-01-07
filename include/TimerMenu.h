#ifndef TIMERMENU_H
#define TIMERMENU_H

#include "DisplayControl.h"
#include "MenuOption.h"
#include "TimerClock.h"
#include "eepromLayout.h"

class TimerMenu {

private:
  DisplayControl &displayLed;
  MenuOption (&menuOptions)[4];
  char displayText[6];
  int activeMenu;
  enum MenuMode {
    MENUSTART = 0,
    INTERVAL1 = 1,
    INTERVAL2 = 2,
    NR_OF_ROUNDS = 3
  }menuMode;
  enum ChangeDigit {
    MINUTE_TENS = 0,
    MINUTES = 1,
    SECOND_TENS = 2,
    SECONDS = 3,
    ROUNDS_TENS = 4,
    ROUNDS = 5
  }changeDigit;
  
  void goDeepSleep();
  void displayMenu();
  void changeTimerMode(int changes);
  void incrementIntervalRounds();
  void decrementIntervalRounds();

public:
  TimerMenu(DisplayControl &displayLedToAttach, MenuOption (&menuOptionsToAttach)[4]);
  void setup();
  void loop();
  // To be called by Start Button short press
  // returns pointer to menuOption to start, otherwise nullptr
  int returnMenu();
  // To be called by Menu button
  void advanceMenu();
  // To be called by button +
  void incrementOption();
  // To be called by button -
  void decrementOption();
};
#endif
