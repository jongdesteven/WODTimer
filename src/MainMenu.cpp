#include "MainMenu.h"

MainMenu::MainMenu(DisplayControl &displayToAttach) :
displayLed(displayToAttach),
cfTimer(displayToAttach),
configMenu(displayToAttach)
{
}

void MainMenu::setup(){
  // Start with Main Menu
  //activeMenu = MENUSTART;
  //menuModeDisplayed = TIMER;

  // Start with Timer Mode
  activeMenu = TIMER;
  menuModeDisplayed = TIMER;
  cfTimer.setup();
}

void MainMenu::loop(){
  switch(activeMenu){
  case MENUSTART:
    displayMenu();
    break;
  case TIMER:
    cfTimer.loop();
    break;
  case CONFIG:
    configMenu.loop();
    break;
  }
}

void MainMenu::selectMenu(){
  switch(menuModeDisplayed){
  case MENUSTART:
    break;
  case TIMER:
    activeMenu = menuModeDisplayed;
    cfTimer.setup();
    break;
  case CONFIG:
    activeMenu = menuModeDisplayed;
    configMenu.setup();
    break;
  }
}

void MainMenu::displayMenu(){
  switch(menuModeDisplayed){
  case MENUSTART: // Does not exist
  case TIMER:
    displayLed.displayCharArray((char*)timerName, false);
    break;
  case CONFIG:
    displayLed.displayCharArray((char*)configName, false);
    break;
  }
}

void MainMenu::returnAction(){
  activeMenu = MENUSTART;
}

void MainMenu::powerAction(){
  switch(activeMenu){
  case MENUSTART:
    break;
  case TIMER:
    cfTimer.startTheTimer();
    break;
  case CONFIG:
    configMenu.powerAction();
    break;
  }
}

void MainMenu::menuAction(){
  switch(activeMenu){
  case MENUSTART:
    selectMenu();
    break;
  case TIMER:
    cfTimer.advanceMenu();
    break;
  case CONFIG:
    configMenu.menuAction();
    break;
  }
}

void MainMenu::incrementAction(){
  switch(activeMenu){
  case MENUSTART:
    switch(menuModeDisplayed){
    case MENUSTART: // Does not exist
    case TIMER:
      menuModeDisplayed = CONFIG;
      break;
    case CONFIG:
      menuModeDisplayed = TIMER;
      break;
    }
    break;
  case TIMER:
    cfTimer.incrementOption();
    break;
  case CONFIG:
    configMenu.incrementAction();
    break;
  }
}

void MainMenu::decrementAction(){
switch(activeMenu){
  case MENUSTART:
    switch(menuModeDisplayed){
    case MENUSTART: // Does not exist
    case TIMER:
      menuModeDisplayed = CONFIG;
      break;
    case CONFIG:
      menuModeDisplayed = TIMER;
      break;
    }
    break;
  case TIMER:
    cfTimer.decrementOption();
    break;
  case CONFIG:
    configMenu.decrementAction();
    break;
  }
}
  
