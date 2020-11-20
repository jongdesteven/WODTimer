#include "ConfigMenu.h"

ConfigMenu::ConfigMenu(DisplayControl &displayToAttach) :
displayLed(displayToAttach)
{
}

void ConfigMenu::setupWifiManager() {
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
}

void ConfigMenu::selectMenu(){
  switch(activeMenu){
  case MENUSTART: // Does not exist
  case WIFI:
    //activeMenu = menuModeDisplayed;
    break;
  case OTA:
    break;
  }
}

void ConfigMenu::displayMenu(){
  switch(menuModeDisplayed){
  case MENUSTART: // Does not exist
  case WIFI:
    displayLed.displayCharArray((char*)wifiName);
    break;
  case OTA:
    displayLed.displayCharArray((char*)otaName);
    break;
  }
}

void ConfigMenu::returnAction(){
  activeMenu = MENUSTART;
}

void ConfigMenu::selectAction(){
  switch(activeMenu){
  case MENUSTART:
    selectMenu();
    break;
  case WIFI:
    break;
  case OTA:
    break;
  }
}

void ConfigMenu::menuAction(){
  switch(activeMenu){
  case MENUSTART:
    break;
  case WIFI:
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
    break;
  case OTA:
    break;
  }
}
