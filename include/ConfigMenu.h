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

  void displayConfig();
  void setupWifiManager();

public:
  ConfigMenu(DisplayControl &displayToAttach);
  void loop();
  void setup();

};

#endif