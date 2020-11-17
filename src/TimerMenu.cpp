#include "TimerMenu.h"


void TimerMenu::displayMenu(){
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
void TimerMenu::changeTimerMode(int changes) {
  //cycle between MenuOptions
  Serial.print("changeTimerMode: from ");
  Serial.print(activeMenu);
  Serial.print(" to ");
  activeMenu = ( activeMenu+changes ) % ( sizeof(menuOptions)/sizeof(MenuOption) );
  changeDigit = MINUTES;
  menuMode = MENUSTART;
  Serial.print(activeMenu);
  Serial.print(" name: ");
  Serial.println(menuOptions[activeMenu].getDisplayName());
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

TimerMenu::TimerMenu(DisplayControl &displayLedToAttach):
  displayLed(displayLedToAttach)
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
  case TIMER_RUNNING:
    activeTimer.loop();
    break;
  }
}

// To be called by Power/Start Button short press
//Sequence: Timer <-> Start <-> int1, int2, rnds
void TimerMenu::startTheTimer(){
  // Serial.print("StartTheTimer: from ");
  // Serial.print(menuMode);
  // Serial.print(" to ");
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
  //Serial.println(menuMode);
}

void TimerMenu::advanceMenu(){
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
  case TIMER_RUNNING:
    break;
  }
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
  case TIMER_RUNNING:
    break;
  }
}
