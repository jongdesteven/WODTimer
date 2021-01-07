#include <Arduino.h>
#include "ssid_info.h"
#include "eepromLayout.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

// Classes
#include "buttons.h"
#include "DisplayControl.h"
#include "ConfigMenu.h"
#include "EasyBuzzer.h"
#include "MenuOption.h"
#include "TimerMenu.h"
#include "TimerClock.h"

#define DEEPSLEEP_TIME_US (3*1000000)
#define SLEEP_TIMEOUT_MS  (60*1000)

void returnAction();
void powerAction();
void menuAction();
void incrementAction();
void decrementAction();
bool wakeFromDeepSleep();

void selectMenu();
void displayMenu();

MenuOption menuOptions[4] = { MenuOption("UP", 356400, 0, 0, true, false, EEPROM_UP_ADDR),
                              MenuOption("UP", 45, 0, 2, true, false, EEPROM_UP_RD_ADDR),  
                              MenuOption("dn", 10*60, 0, 0, false, false, EEPROM_DN_ADDR), 
                              MenuOption("nt", 60, 30, 5, false, true, EEPROM_NT_ADDR)};

DisplayControl displayLed(2);
TimerMenu wodTimerMenu(displayLed, menuOptions);
ConfigMenu configMenu(displayLed);
//const char* name, int time1, int time2, int rounds, bool countUp, bool isInterval

TimerClock activeTimer = TimerClock(displayLed);

Button pwrBtn(5, powerAction, returnAction);
Button menuBtn(12, menuAction);
Button minBtn(4, decrementAction);
Button plusBtn(3, incrementAction); 

enum TimerState {
  MENUSTART = 0,
  TIMERMENU = 1,
  TIMER = 2,
  CONFIG = 3,
  SLEEP = 4
}activeState;
TimerState stateDisplayed;

const char* timerName = "  tine"; //Resembles time on 7-seg
const char* configName = "  COnF"; // config
const char* sleepName = "   OFF";

char displayText[6];
char hostname[16];
unsigned long lastActionMs;
const char* boardName = "WODtimer";

bool wakeFromDeepSleep(){
  if (ESP.getResetInfoPtr()->reason == REASON_DEEP_SLEEP_AWAKE){
    pinMode(5, INPUT_PULLUP);
    if (digitalRead(5) == LOW) {
      return true;
    }
    else {
      return false;
    }
  }
  else { 
    return true;
  }
}

void returnAction(){
  activeState = MENUSTART;
}

void powerAction(){
  int selectedMenuOption = -1;

  switch(activeState){
  case MENUSTART:
    break;
  case TIMERMENU:
    selectedMenuOption = wodTimerMenu.returnMenu();
    if (selectedMenuOption >= 0){
      activeTimer.setup(&menuOptions[selectedMenuOption]);
      activeTimer.startClock();
      activeState = TIMER;
    }
    break;
  case TIMER:
    activeTimer.startClock();
    break;
  case CONFIG:
    configMenu.powerAction();
    break;
  case SLEEP:
    break;
  }
  lastActionMs = millis();
}

void menuAction(){
  switch(activeState){
  case MENUSTART:
    selectMenu();
    break;
  case TIMERMENU:
    wodTimerMenu.advanceMenu();
    break;
  case TIMER:
    activeState = TIMERMENU;
    break;
  case CONFIG:
    configMenu.menuAction();
    break;
  case SLEEP:
    break;
  }
  lastActionMs = millis();
}

void incrementAction(){
  switch(activeState){
  case MENUSTART:
    switch(stateDisplayed){
    case MENUSTART: // Does not exist
    case TIMERMENU:
      stateDisplayed = CONFIG;
      break;
    case TIMER:
      break;
    case CONFIG:
      stateDisplayed = SLEEP;
      break;
    case SLEEP:
      stateDisplayed = TIMERMENU;
      break;
    }
    break;
  case TIMERMENU:
    wodTimerMenu.incrementOption();
    break;
  case TIMER:
    break;
  case CONFIG:
    configMenu.incrementAction();
    break;
  case SLEEP:
    break;
  }
  lastActionMs = millis();
}

void decrementAction(){
  switch(activeState){
  case MENUSTART:
    switch(stateDisplayed){
    case MENUSTART: // Does not exist
    case TIMERMENU:
      stateDisplayed = SLEEP;
      break;
    case TIMER:
      break;
    case CONFIG:
      stateDisplayed = TIMERMENU;
      break;
    case SLEEP:
      stateDisplayed = CONFIG;
      break;
    }
    break;
  case TIMERMENU:
    wodTimerMenu.decrementOption();
    break;
  case TIMER:
    break;
  case CONFIG:
    configMenu.decrementAction();
    break;
  case SLEEP:
    break;
  }
  lastActionMs = millis();
}

void selectMenu(){
  switch(stateDisplayed){
  case MENUSTART:
    break;
  case TIMERMENU:
    activeState = stateDisplayed;
    wodTimerMenu.setup();
    break;
  case TIMER: //not possible
    break;
  case CONFIG:
    activeState = stateDisplayed;
    configMenu.setup();
    break;
  case SLEEP:
    displayLed.shutdown(0, true);
    delay(200);
    ESP.deepSleep(3*1000000, WAKE_RF_DEFAULT); // Possibility to enable wifi direct after this.
    delay(100);
    break;
  }
  lastActionMs = millis();
}

void displayMenu(){
  switch(stateDisplayed){
  case MENUSTART: // Does not exist
  case TIMERMENU:
    displayLed.displayCharArray((char*)timerName, false);
    break;
  case TIMER:
    break;
  case CONFIG:
    displayLed.displayCharArray((char*)configName, false);
    break;
  case SLEEP:
    displayLed.displayCharArray((char*)sleepName, false);
    break;
  }
}

void setup() {
  if (!wakeFromDeepSleep()){
    // ESP.deepSleep(DEEPSLEEP_TIME_US, WAKE_RF_DISABLED); //Final version
    ESP.deepSleep(DEEPSLEEP_TIME_US, WAKE_RF_DEFAULT);
    delay(100);
  }
  
  EEPROM.begin(50);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.forceSleepBegin();
  
  sprintf(hostname, "%s-%06x", boardName,  ESP.getChipId());
  activeState = TIMERMENU;
  stateDisplayed = TIMERMENU;

  displayLed.setup();
  displayLed.start();

  //Skip UP, end time is fixed.
  for ( unsigned int i = 1; i < (sizeof(menuOptions)/sizeof(MenuOption)) ; i++)
  {
    menuOptions[i].setup();
  }

  pwrBtn.setup();
  menuBtn.setup();
  minBtn.setup();
  plusBtn.setup();
  wodTimerMenu.setup();
  
  lastActionMs = millis();
}

void loop() {
  pwrBtn.loop();
  menuBtn.loop();
  minBtn.loop();
  plusBtn.loop();
  displayLed.loop();
  EasyBuzzer.update();

  switch(activeState){
  case MENUSTART:
    displayMenu();
    break;
  case TIMERMENU:
    wodTimerMenu.loop();
    break;
  case TIMER:
    activeTimer.loop();
    lastActionMs = millis();
    break;
  case CONFIG:
    configMenu.loop();
    break;
  case SLEEP:
    displayLed.shutdown(0, true);
    delay(200);
    // ESP.deepSleep(DEEPSLEEP_TIME_US, WAKE_RF_DISABLED); //Final version
    ESP.deepSleep(DEEPSLEEP_TIME_US, WAKE_RF_DEFAULT);
    delay(100);
    break;
  }

  if ( (millis() - lastActionMs) >= SLEEP_TIMEOUT_MS){
    activeState = SLEEP;
  }

}
