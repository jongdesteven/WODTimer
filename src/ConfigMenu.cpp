#include "ConfigMenu.h"

ConfigMenu::ConfigMenu(DisplayControl &displayToAttach) :
displayLed(displayToAttach)
{
}

void ConfigMenu::turnOffWifi(){
  int disconnect_tries = 0;
  Serial.print("Turning Off Wifi");
  wifiManager.stopConfigPortal();
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();

  while (WiFi.status() == WL_CONNECTED && (disconnect_tries++ < 30)){
    //Disconnect in 3sec
    delay(100);
    Serial.print(".");
  }
  Serial.println("ended");

}

void ConfigMenu::setupWifiManager() {
  WiFi.forceSleepWake();
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  Serial.setDebugOutput(true);
  delay(1000);
  Serial.println("\n Starting");
  // wm.resetSettings();
  wifiManager.setHostname("WODTimer_AP");
  wifiManager.setConfigPortalBlocking(false);

	if(wifiManager.autoConnect("WODTimer_AP")){
		Serial.println("connected...:)");
		Serial.println("");
		Serial.println("WiFi connected");
		Serial.println("IP address: ");
		Serial.println(WiFi.localIP());
	}
	else {
		Serial.println("Configportal running");
	}
  wifiOnStartTimeMs = millis();
}

 void ConfigMenu::setup(){
  activeMenu = MENUSTART;
  menuModeDisplayed = WIFI;
  wifiOnStartTimeMs = 0;
 }

void ConfigMenu::loop(){
  switch(activeMenu){
  case MENUSTART: // Does not exist
    displayMenu();
  case WIFI:
    displayWifiConfig();
    break;
  case OTA:
    break;
  }
}

void ConfigMenu::displayWifiConfig(){
  switch (wifiConfigModeDisplayed){
  case WIFI_CONFIG_OFF:
    sprintf(displayText, "vifiof");
    if (wifiConfigMode == WIFI_CONFIG_OFF){
      displayLed.displayCharArray(displayText, true);
    }
    else {
      displayLed.displayCharArray(displayText, 0b000011, true);
    }
    break;
  case WIFI_CONFIG_ON:
    sprintf(displayText, "vifion");
    if (wifiConfigMode == WIFI_CONFIG_ON){
      displayLed.displayCharArray(displayText, true);
    }
    else {
      displayLed.displayCharArray(displayText, 0b000011, true);
    }
    wifiManager.process();
    displayWifiProgress();

    if (millis() - wifiOnStartTimeMs >= WIFI_CONN_TIMEOUT_MS){
      //Connection timeout
      turnOffWifi();
      wifiConfigModeDisplayed = WIFI_CONFIG_OFF;
      wifiConfigMode = WIFI_CONFIG_OFF;
    }
    break;
  }
}

void ConfigMenu::displayWifiProgress(){
  if (millis() - wifiOnStartTimeMs >= 2000){
    if (WiFi.status() != WL_CONNECTED) {
      sprintf(displayText, "notCon");
      displayLed.turnColonOn(false);
      displayLed.displayCharArray(displayText, false); 
    }
    else {
      sprintf(displayText, "Connec");
      displayLed.turnColonOn(false);
      displayLed.displayCharArray(displayText, false);
    }  
  } 
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
    }
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
  }
}

void ConfigMenu::decrementAction(){
switch(activeMenu){
  case MENUSTART:
    switch(menuModeDisplayed){
    case MENUSTART: // Does not exist
    case WIFI:
      menuModeDisplayed = OTA;
      break;
    case OTA:
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
  }
}
