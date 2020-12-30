#include <Arduino.h>
#include "ssid_info.h"
#include "eepromLayout.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

// Classes
#include "MainMenu.h"
#include "buttons.h"
#include "DisplayControl.h"
#include "EasyBuzzer.h"

char hostname[16];
const char* boardName = "WODTimer";

DisplayControl ledDisplay(2);
MainMenu mainMenu(ledDisplay);
PowerStartControlButton pwrBtn(5, mainMenu);
MenuControlButton menuBtn(12, mainMenu);
MinusButton minBtn(4, mainMenu);
PlusButton plusBtn(3, mainMenu); 
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

void setup() {
  //Serial.begin(115200);
  if (!wakeFromDeepSleep()){
    ledDisplay.setup();
    delay(200);
    //Serial.println("Button D3/GPIO0 not pressed, going to DeepSleep for 3s");
    // ESP.deepSleep(3*1000000, WAKE_RF_DISABLED); //Final version
    ESP.deepSleep(3*1000000, WAKE_RF_DEFAULT);
    delay(100);
  }
  
  EEPROM.begin(50);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.forceSleepBegin();
  
  sprintf(hostname, "%s-%06x", boardName,  ESP.getChipId());

  ledDisplay.setup();
  ledDisplay.start();
  pwrBtn.setup();
  menuBtn.setup();
  minBtn.setup();
  plusBtn.setup();
  mainMenu.setup();
}

void loop() {
  pwrBtn.loop();
  menuBtn.loop();
  minBtn.loop();
  plusBtn.loop();
  mainMenu.loop();
  ledDisplay.loop();
  EasyBuzzer.update();
}
