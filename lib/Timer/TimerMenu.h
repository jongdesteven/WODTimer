#ifndef TIMERMENU_H
#define TIMERMENU_H

#include "DisplayControl.h"
#include "MenuOption.h"
#include "TimerClock.h"

class TimerMenu {

private:
  DisplayControl &displayLed;
  char displayText[6];
  int activeMenu;
  enum MenuMode {
    MENUSTART = 0,
    INTERVAL1 = 1,
    INTERVAL2 = 2,
    NR_OF_ROUNDS = 3,
    TIMER_RUNNING = 4
  }menuMode;
  enum ChangeDigit {
    MINUTE_TENS = 1,
    MINUTES = 2,
    SECOND_TENS = 3,
    SECONDS = 4,
    ROUNDS_TENS = 5,
    ROUNDS = 6
  }changeDigit;
  //const char* name, int time1, int time2, int rounds, bool countUp, bool isInterval
  MenuOption menuOptions[4] = {MenuOption("UP", 356400, 0, 0, true, false),
                              MenuOption("UP", 45, 0, 2, true, false),  
                              MenuOption("dn", 10*60, 0, 0, false, false), 
                              MenuOption("nt", 60, 30, 5, false, true)};
  TimerClock activeTimer = TimerClock(menuOptions[0], displayLed);
  
  void displayMenu();
  void changeTimerMode(int changes);
  void incrementIntervalRounds();
  void decrementIntervalRounds();

public:
  TimerMenu(DisplayControl &displayLedToAttach);
  void setup();
  void loop();
  // To be called by Start Button short press
  //Sequence: Timer <-> Start <-> int1, int2, rnds
  void startTheTimer();
  // To be called by Menu button
  void advanceMenu();
  // To be called by button +
  void incrementOption();
  // To be called by button -
  void decrementOption();
};
#endif
