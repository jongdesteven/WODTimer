#ifndef CONFIGMENU_H
#define CONFIGMENU_H

//needed for WifiManager Library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include "DisplayControl.h"

#define WIFI_CONN_TIMEOUT_MS 300000

class ConfigMenu {

private:
  DisplayControl &displayLed;
  WiFiManager wifiManager;
  char displayText[6];

  enum MenuMode {
    MENUSTART = 0,
    WIFI = 1,
    OTA = 2,
    BRIGHTNESS = 3
  }activeMenu;
  MenuMode menuModeDisplayed;

  enum WifiConfig {
    WIFI_CONFIG_OFF = 0,
    WIFI_CONFIG_ON= 1
  }wifiConfigMode;
  WifiConfig wifiConfigModeDisplayed;
  
  const char* wifiName = "  vifi"; //
  const char* otaName = "  OtA "; // 
  const char* brightnessName = "  disp"; // 
  
  unsigned long wifiOnStartTimeMs;

  void setupWifiManager();
  void turnOffWifi();
  
  void displayMenu();
  void displayWifiConfig();
  void displayBrightnessMenu();

  int displayBrightness = 0;


public:
  ConfigMenu(DisplayControl &displayToAttach);
  
  void returnAction();
  void powerAction();
  void menuAction();
  void incrementAction();
  void decrementAction();
  
  void setup();
  void loop();

};

#endif
