#include "ConfigMenu.h"

ConfigMenu::ConfigMenu(DisplayControl &displayToAttach) :
displayLed(displayToAttach)
{
}

void ConfigMenu::turnOffWifi(){
  int disconnect_tries = 0;
  Serial.print("Turning Off Wifi");
  if (WiFi.status() != WL_CONNECTED){
    wifiManager.stopConfigPortal();
  }
  // WiFi.disconnect();
  // WiFi.mode(WIFI_OFF);
  // WiFi.forceSleepBegin();

  while (WiFi.status() == WL_CONNECTED && (disconnect_tries++ < 30)){
    //Disconnect in 3sec
    delay(100);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConfigModeDisplayed = WIFI_CONFIG_ON;
    wifiConfigMode = WIFI_CONFIG_ON;
    Serial.println("WiFi Connected");
  }
  else {
    wifiConfigModeDisplayed = WIFI_CONFIG_OFF;
    wifiConfigMode = WIFI_CONFIG_OFF;
    Serial.println("WiFi Disconnected");
  }
}

void ConfigMenu::setupWifiManager() {
  Serial.print("Wifi: ");
  // WiFi.forceSleepWake();
  // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  Serial.println("Turned On");
  //Serial.setDebugOutput(true);
  delay(1000);

  if (WiFi.status() == WL_CONNECTED){
    Serial.println("WiFi connected: disconnecting");
    WiFi.disconnect();
  }

  Serial.println("WifiManager starting");
  wifiManager.resetSettings();

  wifiManager.setHostname("WODTimer_AP");
  wifiManager.setConfigPortalBlocking(false);

	if(wifiManager.autoConnect("WODTimer_AP")){
		Serial.println("WifiManager connected:)");
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());
	}
	else {
		Serial.println("Configportal running");
	}
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected");
    wifiConfigModeDisplayed = WIFI_CONFIG_ON;
    wifiConfigMode = WIFI_CONFIG_ON;
  }
  else {
    Serial.println("WiFi NOT Connected");
    wifiConfigModeDisplayed = WIFI_CONFIG_OFF;
    wifiConfigMode = WIFI_CONFIG_OFF;
  }
  wifiOnStartTimeMs = millis();
}

 void ConfigMenu::setup(){
  activeMenu = MENUSTART;
  menuModeDisplayed = WIFI;
  EasyBuzzer.setPin(15);

  displayBrightness = EEPROM.read(EEPROM_BRIGHTNESS_ADDR);
  if (displayBrightness < 0 || displayBrightness > 15) displayBrightness = 0;
  beepVolume = EEPROM.read(EEPROM_BEEP_ADDR);
  if (beepVolume < 0 || beepVolume > 3) beepVolume = 0;

  if (WiFi.getMode() == WIFI_OFF){
   wifiConfigMode = WIFI_CONFIG_OFF;
   wifiConfigModeDisplayed = WIFI_CONFIG_OFF;
  }
  else {
    wifiConfigMode = WIFI_CONFIG_ON;
    wifiConfigModeDisplayed = WIFI_CONFIG_ON;
  }
  wifiOnStartTimeMs = 0;
 }

void ConfigMenu::loop(){
  switch(activeMenu){
  case MENUSTART: // Does not exist
    displayMenu();
    break;
  case WIFI:
    displayWifiConfig();
    break;
  case OTA:
    break;
  case BRIGHTNESS:
    displayBrightnessMenu();
    break;
  case BEEP:
    displayBeepMenu();
  }
}

void ConfigMenu::displayBeepMenu(){
  sprintf(displayText, "%2dbEEP", beepVolume);
  displayLed.displayCharArray(displayText, false);
  EasyBuzzer.update();
}

void ConfigMenu::displayBrightnessMenu(){
  sprintf(displayText, "%2ddisp", EEPROM.read(EEPROM_BRIGHTNESS_ADDR));
  displayLed.displayCharArray(displayText, false);
}

void ConfigMenu::displayWifiConfig(){
  switch (wifiConfigModeDisplayed){
  case WIFI_CONFIG_OFF:
    switch (wifiConfigMode) {
    case WIFI_CONFIG_OFF:
      sprintf(displayText, "vifiof");
      displayLed.displayCharArray(displayText, true);
      break;
    case WIFI_CONFIG_ON:
      sprintf(displayText, "vifiof");
      displayLed.displayCharArray(displayText, 0b000011, true);
      break;
    }
    break;
  case WIFI_CONFIG_ON:
    switch (wifiConfigMode) {
    case WIFI_CONFIG_OFF:
      sprintf(displayText, "vifion");
      displayLed.displayCharArray(displayText, 0b000011, true);
      break;
    case WIFI_CONFIG_ON:
      if (WiFi.status() == WL_CONNECTED) {
        sprintf(displayText, "vifion");
        displayLed.displayCharArray(displayText, true);
      }
      else {
        sprintf(displayText, "notCon");
        displayLed.displayCharArray(displayText, true);
       
        // Turn off again after Timeout
        if (WiFi.status() != WL_CONNECTED && (millis() - wifiOnStartTimeMs >= WIFI_CONN_TIMEOUT_MS)){
          //Connection timeout
          Serial.println("Wifi connection timeout, no connection");
          turnOffWifi();
        }
      }
      break;
    }
    break;  
  }
  wifiManager.process();
}

void ConfigMenu::displayMenu(){
  switch(menuModeDisplayed){
  case MENUSTART: // Does not exist
  case WIFI:
    displayLed.displayCharArray((char*)wifiName, false);
    break;
  case OTA:
    displayLed.displayCharArray((char*)otaName, false);
    break;
  case BRIGHTNESS:
    displayLed.displayCharArray((char*)brightnessName, false);
    break;
  case BEEP:
    displayLed.displayCharArray((char*)beepName, false);
    break;
  }
}

void ConfigMenu::returnAction(){
  activeMenu = MENUSTART;
}

void ConfigMenu::powerAction(){
  switch(activeMenu){
  case MENUSTART:
    break;
  case WIFI:
    activeMenu = MENUSTART;
    break;
  case OTA:
    activeMenu = MENUSTART;
    break;
  case BRIGHTNESS:
    activeMenu = MENUSTART;
    if (displayBrightness != EEPROM.read(EEPROM_BRIGHTNESS_ADDR)){
      EEPROM.write(EEPROM_BRIGHTNESS_ADDR, displayBrightness);
      EEPROM.commit();
    }
    break;
  case BEEP:
    activeMenu = MENUSTART;
    if (beepVolume != EEPROM.read(EEPROM_BEEP_ADDR)){
      EEPROM.write(EEPROM_BEEP_ADDR, beepVolume);
      EEPROM.commit();
    }
    break;
  }
}

void ConfigMenu::menuAction(){
  switch(activeMenu){
  case MENUSTART:
    switch(menuModeDisplayed){
    case MENUSTART: // Does not exist
      break;
    case WIFI:
      activeMenu = menuModeDisplayed;
      switch (wifiConfigMode) {
      case WIFI_CONFIG_ON:
        wifiConfigModeDisplayed = WIFI_CONFIG_ON;
        break;
      case WIFI_CONFIG_OFF:
        wifiConfigModeDisplayed = WIFI_CONFIG_OFF;
        break;
      }
      break;
    case OTA:
      break;
    case BRIGHTNESS:
      activeMenu = menuModeDisplayed;
      break;
    case BEEP:
      activeMenu = menuModeDisplayed;
      break;
    }
    break;
  case WIFI:
    switch (wifiConfigModeDisplayed){
    case WIFI_CONFIG_OFF:
      wifiConfigMode = wifiConfigModeDisplayed;
      turnOffWifi();
      break;
    case WIFI_CONFIG_ON:
      wifiConfigMode = wifiConfigModeDisplayed;
      setupWifiManager();
      break;
    }
    break;
  case OTA:
    break;
  case BRIGHTNESS:
    break;
  case BEEP:
    break;
  }
}

void ConfigMenu::incrementAction(){
  switch(activeMenu){
  case MENUSTART:
    switch(menuModeDisplayed){
    case MENUSTART: // Does not exist
    case WIFI:
      menuModeDisplayed = OTA;
      break;
    case OTA:
      menuModeDisplayed = BRIGHTNESS;
      break;
    case BRIGHTNESS:
      menuModeDisplayed = BEEP;
      break;
    case BEEP:
      menuModeDisplayed = WIFI;
      break;
    }
    break;
  case WIFI:
    switch (wifiConfigModeDisplayed){
    case WIFI_CONFIG_OFF:
      wifiConfigModeDisplayed = WIFI_CONFIG_ON;
      break;
    case WIFI_CONFIG_ON:
      wifiConfigModeDisplayed = WIFI_CONFIG_OFF;
      break;
    }
    break;
  case OTA:
    break;
  case BRIGHTNESS:
    if (displayBrightness < 15){
      displayBrightness++;
    }
    displayLed.setIntensity(0, displayBrightness);
    break;
  case BEEP:
    if (beepVolume < 2){
      beepVolume++;
      if (beepVolume == 1) EasyBuzzer.singleBeep(3000, 200);
      else if ( beepVolume == 2) EasyBuzzer.singleBeep(2000, 200);
    }
    break;
  }
}

void ConfigMenu::decrementAction(){
switch(activeMenu){
  case MENUSTART:
    switch(menuModeDisplayed){
    case MENUSTART: // Does not exist
    case WIFI:
      menuModeDisplayed = BEEP;
      break;
    case OTA:
      menuModeDisplayed = WIFI;
      break;
    case BRIGHTNESS:
      menuModeDisplayed = OTA;
      break;
    case BEEP:
      menuModeDisplayed = BRIGHTNESS;
      break;
    }
    break;
  case WIFI:
    switch (wifiConfigModeDisplayed){
    case WIFI_CONFIG_OFF:
      wifiConfigModeDisplayed = WIFI_CONFIG_ON;
      break;
    case WIFI_CONFIG_ON:
      wifiConfigModeDisplayed = WIFI_CONFIG_OFF;
      break;
    }
    break;
  case OTA:
    break;
  case BRIGHTNESS:
    if (displayBrightness > 0){
      displayBrightness--;
    }
    displayLed.setIntensity(0, displayBrightness);
    break;
  case BEEP:
    if (beepVolume > 0){
      beepVolume--;
      if (beepVolume == 1) EasyBuzzer.singleBeep(3000, 200);
      else if ( beepVolume == 2) EasyBuzzer.singleBeep(2000, 200);
    }
    break;
  }
}
