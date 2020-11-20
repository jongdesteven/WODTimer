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