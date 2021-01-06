#include <Arduino.h>
#include "ssid_info.h"
#include "eepromLayout.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

// Classes
#include "buttons.h"
#include "DisplayControl.h"
#include "EasyBuzzer.h"
#include "TimerMenu.h"
#include "ConfigMenu.h"

void returnAction();
void powerAction();
void menuAction();
void incrementAction();
void decrementAction();
bool wakeFromDeepSleep();

void selectMenu();
void displayMenu();

DisplayControl displayLed(2);
TimerMenu cfTimer(displayLed);
ConfigMenu configMenu(displayLed);
Button pwrBtn(5, powerAction, returnAction);
Button menuBtn(12, menuAction);
Button minBtn(4, decrementAction);
Button plusBtn(3, incrementAction); 

enum MenuMode {
  MENUSTART = 0,
  TIMER = 1,
  CONFIG = 2,
  SLEEP = 3
}activeMenu;

MenuMode menuModeDisplayed;
const char* timerName = "  tine"; //Resembles time on 7-seg
const char* configName = "  COnF"; // config
const char* sleepName = "   OFF";

char displayText[6];
char hostname[16];
const char* boardName = "WODTimer";

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
  activeMenu = MENUSTART;
}

void powerAction(){
  switch(activeMenu){
  case MENUSTART:
    break;
  case TIMER:
    cfTimer.startTheTimer();
    break;
  case CONFIG:
    configMenu.powerAction();
    break;
  case SLEEP:
    break;
  }
}

void menuAction(){
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
  case SLEEP:
    break;
  }
}

void incrementAction(){
  switch(activeMenu){
  case MENUSTART:
    switch(menuModeDisplayed){
    case MENUSTART: // Does not exist
    case TIMER:
      menuModeDisplayed = CONFIG;
      break;
    case CONFIG:
      menuModeDisplayed = SLEEP;
      break;
    case SLEEP:
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
  case SLEEP:
    break;
  }
}

void decrementAction(){
switch(activeMenu){
  case MENUSTART:
    switch(menuModeDisplayed){
    case MENUSTART: // Does not exist
    case TIMER:
      menuModeDisplayed = SLEEP;
      break;
    case CONFIG:
      menuModeDisplayed = TIMER;
      break;
    case SLEEP:
      menuModeDisplayed = CONFIG;
      break;
    }
    break;
  case TIMER:
    cfTimer.decrementOption();
    break;
  case CONFIG:
    configMenu.decrementAction();
    break;
  case SLEEP:
    break;
  }
}

void selectMenu(){
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
  case SLEEP:
    displayLed.shutdown(0, true);
    delay(200);
    ESP.deepSleep(3*1000000, WAKE_RF_DISABLED);
    delay(100);
    break;
  }
}

void displayMenu(){
  switch(menuModeDisplayed){
  case MENUSTART: // Does not exist
  case TIMER:
    displayLed.displayCharArray((char*)timerName, false);
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
    // ledDisplay.setup();
    // delay(200);
    // ESP.deepSleep(3*1000000, WAKE_RF_DISABLED); //Final version
    ESP.deepSleep(3*1000000, WAKE_RF_DEFAULT);
    delay(100);
  }
  
  EEPROM.begin(50);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.forceSleepBegin();
  
  sprintf(hostname, "%s-%06x", boardName,  ESP.getChipId());
  activeMenu = TIMER;
  menuModeDisplayed = TIMER;

  displayLed.setup();
  displayLed.start();
  pwrBtn.setup();
  menuBtn.setup();
  minBtn.setup();
  plusBtn.setup();
  cfTimer.setup();
}

void loop() {
  pwrBtn.loop();
  menuBtn.loop();
  minBtn.loop();
  plusBtn.loop();
  displayLed.loop();
  EasyBuzzer.update();

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
  case SLEEP:
    break;
  }

}
