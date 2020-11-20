#ifndef CONFIGMENU_H
#define CONFIGMENU_H

//needed for WifiManager Library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include "DisplayControl.h"

class ConfigMenu {

private:
  DisplayControl &displayLed;
  WiFiManager wifiManager;
  char displayText[6];
  enum MenuMode {
    MENUSTART = 0,
    WIFI = 1,
    OTA = 2
  }activeMenu;
  MenuMode menuModeDisplayed;
  const char* wifiName = "  conn"; //Resembles WOD on 7-seg
  const char* otaName = "  OtA "; // config

  void displayConfig();
  void setupWifiManager();
  
  void selectMenu();
  void displayMenu();

public:
  ConfigMenu(DisplayControl &displayToAttach);
  
  void returnAction();
  void selectAction();
  void menuAction();
  void incrementAction();
  void decrementAction();
  
  void setup();
  void loop();

};

#endif
