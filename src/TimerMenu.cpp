#include "TimerMenu.h"

void TimerMenu::displayMenu(){
  //ToDo: Blinking changeDigit
  int blinkingSegment = 0;
  bool colonShown = false;
  switch (menuMode){
  case MENUSTART:
    if ( menuOptions[activeMenu].getNrOfRounds() > 0 ) {
      sprintf(displayText,"%2s  rd", menuOptions[activeMenu].getDisplayName().c_str());
    } else {
      sprintf(displayText,"%2s    ", menuOptions[activeMenu].getDisplayName().c_str());
    }
    blinkingSegment = 0;
    colonShown = false;
    break;
  case INTERVAL1:
    sprintf(displayText,"1 %02d%02d", (int)(menuOptions[activeMenu].getStartTime1()/60), (int)(menuOptions[activeMenu].getStartTime1()%60));
    if (changeDigit == MINUTES) blinkingSegment = 0b001100;
    else if (changeDigit == SECONDS) blinkingSegment = 0b000011;
    colonShown = true;
    break;
  case INTERVAL2:
    sprintf(displayText,"2 %02d%02d", (int)(menuOptions[activeMenu].getStartTime2()/60), (int)(menuOptions[activeMenu].getStartTime2()%60));
    if (changeDigit == MINUTES) blinkingSegment = 0b001100;
    else if (changeDigit == SECONDS) blinkingSegment = 0b000011;
    colonShown = true;
    break;
  case NR_OF_ROUNDS:
    sprintf(displayText,"rd  %02d", menuOptions[activeMenu].getNrOfRounds());
    blinkingSegment = 0b000011;
    colonShown = false;
    break;
  }
  displayLed.displayCharArray(displayText, blinkingSegment, colonShown);
}

// To be called from + / - button
void TimerMenu::changeTimerMode(int changes) {
  activeMenu = ( activeMenu+changes ) % ( sizeof(menuOptions)/sizeof(MenuOption) );
  changeDigit = MINUTES;
  menuMode = MENUSTART;
}

void TimerMenu::incrementIntervalRounds(){
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

void TimerMenu::decrementIntervalRounds(){
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

TimerMenu::TimerMenu(DisplayControl &displayLedToAttach, MenuOption (&menuOptionsToAttach)[4]):
  displayLed(displayLedToAttach),
  menuOptions(menuOptionsToAttach)
{
}

void TimerMenu::setup() {
  activeMenu = 0;
  changeDigit = MINUTES;
  menuMode = MENUSTART;
}

void TimerMenu::loop() {
  switch (menuMode){
  case INTERVAL1:
  case INTERVAL2:
  case NR_OF_ROUNDS:
  case MENUSTART:
    displayMenu();
    break;
  }
}

// To be called by Power/Start Button short press
//Sequence: Timer <-> Start <-> int1, int2, rnds
int TimerMenu::returnMenu(){
  switch (menuMode) {
  case MENUSTART:
    return activeMenu;
    break;
  case INTERVAL1:
  case INTERVAL2:
  case NR_OF_ROUNDS:
    menuMode = MENUSTART;
    menuOptions[activeMenu].saveChanges();
    break;
  }
  return -1;
}

void TimerMenu::advanceMenu(){
  switch (menuMode){
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
}

// To be called by button +
void TimerMenu::incrementOption() {
  switch (menuMode) {
  case MENUSTART:
    changeTimerMode(1);
    break;
  case INTERVAL1:
  case INTERVAL2:
  case NR_OF_ROUNDS:
    incrementIntervalRounds();
    break;
  }
  displayLed.forceDisplayUpdate();
}

// To be called by button -
void TimerMenu::decrementOption() {
  switch (menuMode) {
  case MENUSTART:
    changeTimerMode(-1);
    break;
  case INTERVAL1:
  case INTERVAL2:
  case NR_OF_ROUNDS:
    decrementIntervalRounds();
    break;
  }
  displayLed.forceDisplayUpdate();
}
