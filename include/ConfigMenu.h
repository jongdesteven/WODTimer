#ifndef CONFIGMENU_H
#define CONFIGMENU_H

#include <ArduinoOTA.h>

#include "DisplayControl.h"
#include "eepromLayout.h"
#include "EasyBuzzer.h"
#include "ssid_info.h"

class ConfigMenu {

private:
  DisplayControl &displayLed;
  char displayText[6];
  bool otaConnectMessageSent;

  enum MenuMode {
    MENUSTART = 0,
    OTA = 1,
    BRIGHTNESS = 2,
    BEEP = 3,
    MESH = 4
  }activeMenu;
  MenuMode menuModeDisplayed;

  const char* otaName = "  OtA ";
  const char* brightnessName = "  disp";
  const char* beepName = "  bEEP";
  const char* meshName = "  SESH";
  
  void turnOnOTA();
  
  void displayMenu();
  void displayBrightnessMenu();
  void displayBeepMenu();
  void displayOtaMenu();
  void displayMeshMenu();
  void otaProgressCallback(unsigned int progress, unsigned int total);

  byte displayBrightness = 0;
  byte beepVolume = 0;

  bool meshActive = false; // Change to default off

public:
  ConfigMenu(DisplayControl &displayToAttach);
  
  void returnAction();
  void powerAction();
  void menuAction();
  void incrementAction();
  void decrementAction();

  bool meshNetworkActive();
  
  void setup();
  void loop();

};

#endif
