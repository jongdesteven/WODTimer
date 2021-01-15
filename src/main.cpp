#include <Arduino.h>
#include "ssid_info.h"
#include "eepromLayout.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "painlessMesh.h"

// Classes
#include "buttons.h"
#include "DisplayControl.h"
#include "ConfigMenu.h"
#include "EasyBuzzer.h"
#include "MenuOption.h"
#include "TimerMenu.h"
#include "TimerClock.h"

#define DEEPSLEEP_TIME_US (5*1000000)
#define SLEEP_TIMEOUT_MS  (60*1000)

#define  MESH_SSID           "WODtimerMesh"
#define  MESH_PASSWORD       "WDTmrPssWrd"
#define  MESH_PORT           5555
#define  MESH_DELIMITER      ";"
#define  MESH_START_DELAY_MS 2000

void returnAction();
void powerAction();
void menuAction();
void incrementAction();
void decrementAction();
bool wakeFromDeepSleep();
void goDeepSleep();

 // CallBacks for painlessMesh
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

// mesh Functions
void startMeshNetwork();
void startTimerAsHost(MenuOption *menuOptionToSend);
int numberOfConnectedNodes();

void selectMenu();
void displayMenu();

painlessMesh mesh;

MenuOption menuOptions[4] = { MenuOption("UP", 356400, 0, 0, true, EEPROM_UP_ADDR),
                              MenuOption("UP", 45, 0, 2, true, EEPROM_UP_RD_ADDR),  
                              MenuOption("dn", 10*60, 0, 0, false, EEPROM_DN_ADDR), 
                              MenuOption("nt", 60, 30, 5, false, EEPROM_NT_ADDR)};
MenuOption remoteMenuOption;
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
unsigned long remoteStartTimeMs;
const char* boardName = "WODtimer";
bool meshActive = false;
bool meshConnected = false;

void goDeepSleep(){
  if (configMenu.meshNetworkActive()){
    mesh.stop();
    mesh.setRoot();
  }
  displayLed.shutdown(0, true);
  delay(100);
  WiFi.disconnect( true );
  delay( 1 );
  ESP.deepSleep(DEEPSLEEP_TIME_US, WAKE_RF_DEFAULT);
  delay(100);
}

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
      if (configMenu.meshNetworkActive()){
        //send start command
        startTimerAsHost(&menuOptions[selectedMenuOption]);
      }
      else {
        activeTimer.startClock();
      }
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
    ESP.deepSleep(DEEPSLEEP_TIME_US, WAKE_RF_DEFAULT); // Possibility to enable wifi direct after this.
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

void startMeshNetwork(){
  WiFi.forceSleepWake();
  delay(1);
  WiFi.persistent(false);
  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);

}

void startTimerAsHost(MenuOption *menuOptionToSend){
  String msg = String(menuOptionToSend->getDisplayName());
  msg += String(MESH_DELIMITER);
  msg += String(menuOptionToSend->getStartTime1());
  msg += String(MESH_DELIMITER);
  msg += String(menuOptionToSend->getStartTime2());
  msg += String(MESH_DELIMITER);
  msg += String(menuOptionToSend->getNrOfRounds());
  msg += String(MESH_DELIMITER);
  if(menuOptionToSend->getCountDirectionUp()){
    msg += String(1);
  }
  else {
    msg += String(0);
  }
  msg += String(MESH_DELIMITER);
  remoteStartTimeMs = mesh.getNodeTime() + MESH_START_DELAY_MS;
  msg += String(remoteStartTimeMs);
  
  mesh.sendBroadcast(msg);
}

void receivedCallback(uint32_t from, String& msg){
  int last = 0;
  int next = 0;
  //MenuOption data
  String name = "--";
  unsigned long time1 = 0;
  unsigned long time2 = 0;
  int rounds= 0;
  bool countUp = false;

  if ( (next = msg.indexOf(MESH_DELIMITER, last)) != -1){
    name = msg.substring(last, next);
    last = next + 1;
  } else { return; }
  if ( (next = msg.indexOf(MESH_DELIMITER, last)) != -1){
    time1 = msg.substring(last, next).toInt();
    last = next + 1;
  } else { return; }
  if ( (next = msg.indexOf(MESH_DELIMITER, last)) != -1){
    time2 = msg.substring(last, next).toInt();
    last = next + 1;
  } else { return; }
  if ( (next = msg.indexOf(MESH_DELIMITER, last)) != -1){
    rounds = msg.substring(last, next).toInt();
    last = next + 1;
  } else { return; }
  if ( (next = msg.indexOf(MESH_DELIMITER, last)) != -1){
    if (msg.substring(last, next).compareTo("1") == 0){
      countUp = true;
    }
    last = next + 1;
  } else { return; }
  if ( (next = msg.indexOf(MESH_DELIMITER, last)) == -1){ //Last item
    remoteStartTimeMs = (unsigned long)msg.substring(last).toInt();
  } else { return; }

  remoteMenuOption.initialize(name, time1, time2, rounds, countUp, EEPROM_DO_NOT_SAVE);
  activeTimer.setup(&remoteMenuOption);
  activeState = TIMER;
}

void newConnectionCallback(uint32_t nodeId){
  if (!meshConnected){
    displayLed.displayTempMessage("connected");
    meshConnected = true;
  }
}
void changedConnectionCallback(){

}
void nodeTimeAdjustedCallback(int32_t offset){
  displayLed.displayTempMessage("tine sync");
}

void delayReceivedCallback(uint32_t from, int32_t delay){

}

void setup() {
  if (!wakeFromDeepSleep()){
    // ESP.deepSleep(DEEPSLEEP_TIME_US, WAKE_RF_DISABLED); //Final version
    ESP.deepSleep(DEEPSLEEP_TIME_US, WAKE_RF_DEFAULT);
    delay(100);
  }
  
  EEPROM.begin(50); //TODO: make dynamic
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);
  
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

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  pwrBtn.setup();
  menuBtn.setup();
  minBtn.setup();
  plusBtn.setup();
  wodTimerMenu.setup();
  
  lastActionMs = millis();
  remoteStartTimeMs = 0;
}

void loop() {
  pwrBtn.loop();
  menuBtn.loop();
  minBtn.loop();
  plusBtn.loop();
  displayLed.loop();
  EasyBuzzer.update();

  // Start Mesh network when requested in config
  if (configMenu.meshNetworkActive()){
    if ( WiFi.getMode() == WIFI_OFF ){
      startMeshNetwork();
    }
    mesh.update();
  }

  // Start timer on remote request
  if (remoteStartTimeMs != 0 && mesh.getNodeTime() >= remoteStartTimeMs){
    activeTimer.startClock();
    remoteStartTimeMs = 0;
  }

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
    goDeepSleep();
    break;
  }

  if ( (millis() - lastActionMs) >= SLEEP_TIMEOUT_MS){
    activeState = SLEEP;
  }

}
