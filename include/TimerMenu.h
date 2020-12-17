#ifndef TIMERMENU_H
#define TIMERMENU_H

#include "DisplayControl.h"
#include "MenuOption.h"
#include "TimerClock.h"
#include "eepromLayout.h"

class TimerMenu {

private:
  DisplayControl &displayLed;
  char displayText[6];
  int activeMenu;
  unsigned long lastActionMs;
  enum MenuMode {
    MENUSTART = 0,
    INTERVAL1 = 1,
    INTERVAL2 = 2,
    NR_OF_ROUNDS = 3,
    TIMER_RUNNING = 4
  }menuMode;
  enum ChangeDigit {
    MINUTE_TENS = 0,
    MINUTES = 1,
    SECOND_TENS = 2,
    SECONDS = 3,
    ROUNDS_TENS = 4,
    ROUNDS = 5
  }changeDigit;
  //const char* name, int time1, int time2, int rounds, bool countUp, bool isInterval
  MenuOption menuOptions[4] = {MenuOption("UP", 356400, 0, 0, true, false, EEPROM_UP_ADDR),
                              MenuOption("UP", 45, 0, 2, true, false, EEPROM_UP_RD_ADDR),  
                              MenuOption("dn", 10*60, 0, 0, false, false, EEPROM_DN_ADDR), 
                              MenuOption("nt", 60, 30, 5, false, true, EEPROM_NT_ADDR)};
  TimerClock activeTimer = TimerClock(displayLed, &menuOptions[0]);
  
  void goDeepSleep();
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
