#ifndef TIMERMENU_H
#define TIMERMENU_H

#include "DisplayControl.h"
#include "MenuOption.h"
#include "TimerClock.h"

class TimerMenu {
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
  //pass display instance to timerclock

private:
  void displayMenu(){
    //ToDo: Blinking changeDigit
    switch (menuMode){
    case MENUSTART:
      if ( menuOptions[activeMenu].getNrOfRounds() > 0 ) {
        sprintf(displayText,"%2s  rd", menuOptions[activeMenu].getDisplayName());
      } else {
        sprintf(displayText,"%2s    ", menuOptions[activeMenu].getDisplayName());
      }
      break;
    case INTERVAL1:
      sprintf(displayText,"1 %02d%02d", menuOptions[activeMenu].getStartTime1()/60, menuOptions[activeMenu].getStartTime1()%60);
      break;
    case INTERVAL2:
      sprintf(displayText,"2 %02d%02d", menuOptions[activeMenu].getStartTime2()/60, menuOptions[activeMenu].getStartTime2()%60);
      break;
    case NR_OF_ROUNDS:
      sprintf(displayText,"rd  %02d", menuOptions[activeMenu].getNrOfRounds());
      break;
    case TIMER_RUNNING:
      //display is controlled elsewhere
      break;
    }
    displayLed.displayCharArray(displayText);
    displayLed.turnColonOn(false);
  }

  // To be called from + / - button
  void changeTimerMode(int changes) {
    //cycle between MenuOptions
    Serial.print("changeTimerMode: from ");
    Serial.print(activeMenu);
    Serial.print(" to ");
    activeMenu = (activeMenu+changes)%(sizeof(menuOptions)/sizeof(MenuOption));
    // changeDigit = MINUTE_TENS;
    changeDigit = MINUTES;
    menuMode = MENUSTART;
    Serial.println(activeMenu);
  }

  void incrementIntervalRounds(){
    switch(changeDigit){
    case MINUTE_TENS: //minute tens
      menuOptions[activeMenu].changeTimeInterval(menuMode, 10*60);
      break;
    case MINUTES:
      menuOptions[activeMenu].changeTimeInterval(menuMode, 60);
      break;
    case SECOND_TENS:
      menuOptions[activeMenu].changeTimeInterval(menuMode, 10);
      break;
    case SECONDS:
      menuOptions[activeMenu].changeTimeInterval(menuMode, 1);
      break;
    case ROUNDS_TENS:
      menuOptions[activeMenu].changeRounds(10);
      break;
    case ROUNDS:
      menuOptions[activeMenu].changeRounds(1);
      break;             
    }
  }

  void decrementIntervalRounds(){
    switch(changeDigit){
    case MINUTE_TENS: //minute tens
      menuOptions[activeMenu].changeTimeInterval(menuMode, -10*60);
      break;
    case MINUTES:
      menuOptions[activeMenu].changeTimeInterval(menuMode, -60);
      break;
    case SECOND_TENS:
      menuOptions[activeMenu].changeTimeInterval(menuMode, -10);
      break;
    case SECONDS:
      menuOptions[activeMenu].changeTimeInterval(menuMode, -1);
      break;
    case ROUNDS_TENS:
      menuOptions[activeMenu].changeRounds(-10);
      break;
    case ROUNDS:
      menuOptions[activeMenu].changeRounds(-1);
      break;       
    }
  }

public:

  TimerMenu(DisplayControl &displayLedToAttach):
    displayLed(displayLedToAttach)
  {
  }
  
  void setup() {
    activeMenu = 0;
    // changeDigit = MINUTE_TENS;
    changeDigit = MINUTES;
    menuMode = MENUSTART;
  }

  void loop() {
    switch (menuMode){
    case INTERVAL1:
    case INTERVAL2:
    case NR_OF_ROUNDS:
    case MENUSTART:
      displayMenu();
      break;
    case TIMER_RUNNING:
      activeTimer.loop();
      break;
    }
  }

  // To be called by Power/Start Button short press
  //Sequence: Timer <-> Start <-> int1, int2, rnds
  void startTheTimer(){
    Serial.print("StartTheTimer: from ");
    Serial.print(menuMode);
    Serial.print(" to ");
    switch (menuMode) {
    case MENUSTART:
      activeTimer.setup(menuOptions[activeMenu]);
      activeTimer.startClock();
      
      menuMode = TIMER_RUNNING;
      break;
    case INTERVAL1:
    case INTERVAL2:
    case NR_OF_ROUNDS:
      menuMode = MENUSTART;
      break;
    case TIMER_RUNNING:
      activeTimer.startClock();
      break;
    }
    Serial.println(menuMode);
  }
  
  void advanceMenu(){
    Serial.print("advanceMenu from: menuMode: ");
    Serial.print(menuMode);
    Serial.print(" changedigit: ");
    Serial.println(changeDigit);
    switch (menuMode){
    case TIMER_RUNNING:
      menuMode = MENUSTART;
      break;
    case MENUSTART:
      if (menuOptions[activeMenu].getNrOfRounds() > 0 || !menuOptions[activeMenu].getCountDirectionUp()){
        changeDigit = MINUTES;
        menuMode = INTERVAL1;
      }
      break;
    case INTERVAL1:
      if (changeDigit == MINUTES){
        changeDigit = SECONDS;
        break;
      }
      if (menuOptions[activeMenu].getStartTime2() > 0){
        changeDigit = MINUTES;
        menuMode = INTERVAL2;
      }
      else if (menuOptions[activeMenu].getNrOfRounds() > 0 ){
        changeDigit = ROUNDS;
        menuMode = NR_OF_ROUNDS;
      }
      else {
        changeDigit = MINUTES;
      }
      break;
    case INTERVAL2:
      if (changeDigit == MINUTES){
        changeDigit = SECONDS;
        break;
      }
      changeDigit = ROUNDS;
      menuMode = NR_OF_ROUNDS;
      break;
    case NR_OF_ROUNDS:
      changeDigit = MINUTES;
      menuMode = INTERVAL1;
      break;
    }
    Serial.print("advanceMenu to: menuMode: ");
    Serial.print(menuMode);
    Serial.print(" changedigit: ");
    Serial.println(changeDigit);
  }

  // To be called by button +
  void incrementOption() {
    switch (menuMode) {
    case MENUSTART:
      changeTimerMode(1);
      break;
    case INTERVAL1:
    case INTERVAL2:
    case NR_OF_ROUNDS:
      incrementIntervalRounds();
      break;
    case TIMER_RUNNING:
      break;
    }
  }

  // To be called by button -
  void decrementOption() {
    switch (menuMode) {
    case MENUSTART:
      changeTimerMode(-1);
      break;
    case INTERVAL1:
    case INTERVAL2:
    case NR_OF_ROUNDS:
      decrementIntervalRounds();
      break;
    case TIMER_RUNNING:
      break;
    }
  }

};

#endif