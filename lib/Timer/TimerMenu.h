#ifndef TIMERMENU_H
#define TIMERMENU_H

#include "DisplayControl.h"
#include "MenuOption.h"
#include "TimerClock.h"

class TimerMenu {
  DisplayControl &displayLed;
  char displayText[6];
  int activeMenu;
  enum ChangeInterval {
    MENUSTART = 0,
    INTERVAL1 = 1,
    INTERVAL2 = 2,
    NR_OF_ROUNDS = 3,
    TIMER_RUNNING = 4
  }changeMode;
  enum ChangeDigit {
    MINUTE_TENS = 1,
    MINUTES = 2,
    SECOND_TENS = 3,
    SECONDS = 4,
    ROUNDS_TENS = 5,
    ROUNDS = 6
  }changeDigit;
  
  //const char* name, int time1, int time2, int rounds, bool countUp, bool isInterval
  MenuOption menuOptions[4] = {MenuOption("UP", 0, 0, 0, true, false),
                              MenuOption("UP", 45, 0, 2, true, false),  
                              MenuOption("dn", 10*60, 0, 0, false, false), 
                              MenuOption("nt", 60, 30, 5, false, true)};
  TimerClock activeTimer = TimerClock(menuOptions[0], displayLed);
  //pass display instance to timerclock

  private:

    void displayMenu(){
      //ToDo: Blinking changeDigit
      switch (changeMode){
      case MENUSTART:
        if ( menuOptions[activeMenu].hasRounds() ) {
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

    // To be called by Menu Button short press
    void changeTimerMode() {
      //cycle between MenuOptions
      activeMenu = (activeMenu+1)%sizeof(menuOptions);
      changeDigit = MINUTE_TENS;
      changeMode = MENUSTART;
    }

    void incrementIntervalRounds(){
      switch(changeDigit){
      case MINUTE_TENS: //minute tens
        menuOptions[activeMenu].changeTimeInterval(changeMode, 10*10*60);
        break;
      case MINUTES:
        menuOptions[activeMenu].changeTimeInterval(changeMode, 10*60);
        break;
      case SECOND_TENS:
        menuOptions[activeMenu].changeTimeInterval(changeMode, 60);
        break;
      case SECONDS:
        menuOptions[activeMenu].changeTimeInterval(changeMode, 1);
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
        menuOptions[activeMenu].changeTimeInterval(changeMode, -10*10*60);
        break;
      case MINUTES:
        menuOptions[activeMenu].changeTimeInterval(changeMode, -10*60);
        break;
      case SECOND_TENS:
        menuOptions[activeMenu].changeTimeInterval(changeMode, -60);
        break;
      case SECONDS:
        menuOptions[activeMenu].changeTimeInterval(changeMode, -1);
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
      changeDigit = MINUTE_TENS; 
      changeMode = INTERVAL1;
      
    }
  
    void loop() {
      if (activeTimer.roundsLeft() > 0){
        activeTimer.loop();
      }
      else {
        displayMenu();
      }
    }

    // To be called by Power/Start Button short press
    void startTheTimer(){
      switch (changeMode) {
      case MENUSTART:
        activeTimer.setup(menuOptions[activeMenu]);
        activeTimer.startClock();
        changeMode = TIMER_RUNNING;
        break;
      case INTERVAL1:
      case INTERVAL2:
      case NR_OF_ROUNDS:
        changeMode = MENUSTART;
        break;
      case TIMER_RUNNING:
        if (activeTimer.timerRunning()) {
          activeTimer.startClock();
        }
        else {
          changeMode = MENUSTART;
        }
        break;
      }
    }
    
    // Switch between intervals or rounds
    void changeMenuMode() {
      // If at end of setting, switch to next Interval or Rounds, if applicable
      if (changeDigit == SECONDS || changeDigit == ROUNDS) {
        switch (changeMode){
        case INTERVAL1:
          if (menuOptions[activeMenu].hasInterval()){
            changeMode = INTERVAL2;
            changeDigit = MINUTE_TENS;
            break;
          } 
          else if ( menuOptions[activeMenu].hasRounds()) {
            changeMode = NR_OF_ROUNDS;
            changeDigit = ROUNDS_TENS;
            break;
          }
          changeDigit = MINUTE_TENS; //stay at Interval1
          break;
        case INTERVAL2:
          changeMode = NR_OF_ROUNDS;
          changeDigit = ROUNDS_TENS;
          break;
        case NR_OF_ROUNDS:
          changeMode = INTERVAL1;
          changeDigit = MINUTE_TENS;
          break;
        case MENUSTART:
          changeMode = INTERVAL1;
          changeDigit = MINUTE_TENS;
          break;
        case TIMER_RUNNING:
          break;
        }
      }
      else {
        // Move to next digit to change
        switch (changeDigit) {
        case MINUTE_TENS:
          changeDigit = MINUTES;
          break;
        case MINUTES:
          changeDigit = SECOND_TENS;
          break;
        case SECOND_TENS:
          changeDigit = SECONDS;
          break;
        case SECONDS:
          changeDigit = MINUTE_TENS;
          break;
        case ROUNDS_TENS:
          changeDigit = ROUNDS;
          break;
        case ROUNDS:
          changeDigit = MINUTE_TENS;
        } 
      }

      // Move through menu, from menuStart
      switch (changeMode) {
      case MENUSTART:
        changeMode = INTERVAL1;
        changeDigit = MINUTE_TENS;
        break;
      case INTERVAL1:
      case INTERVAL2:
      case NR_OF_ROUNDS:
        break;
      case TIMER_RUNNING:
        break;
      }
    }

    // To be called by button +
    void incrementOption() {
      switch (changeMode) {
      case MENUSTART:
        changeTimerMode();
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
      switch (changeMode) {
      case MENUSTART:
        changeTimerMode();
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