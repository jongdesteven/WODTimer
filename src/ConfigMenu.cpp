#include "ConfigMenu.h"

ConfigMenu::ConfigMenu(DisplayControl &displayToAttach) :
displayLed(displayToAttach)
{
}

void ConfigMenu::otaProgressCallback(unsigned int progress, unsigned int total){
  //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  String text = String( (progress/total)*99, DEC ) + " OtA";
  displayLed.displayCharArray(text,false);
}

void ConfigMenu::turnOnOTA(){
  char hostname[16];
  unsigned int connectTimeOutMs = millis();
  sprintf(hostname, "%s-%06x", "WODtimer",  ESP.getChipId());
  //Serial.print("Wifi: ");
  WiFi.hostname(hostname);
  ArduinoOTA.setHostname(hostname);
  WiFi.forceSleepWake();
  WiFi.begin(SSID_INFO, SSID_INFO_PASS); // For OTA connect to my WiFi

  // No authentication by default
  // ArduinoOTA.setPassword("admin");
  
  // ArduinoOTA.onStart([]() {
  //   String type;
  //   if (ArduinoOTA.getCommand() == U_FLASH) {
  //     type = "sketch";
  //   } else { // U_FS
  //     type = "filesystem";
	//   }
	//   // NOTE: if updating FS this would be the place to unmount FS using FS.end()
	//   //Serial.println("Start updating " + type);
  // });
  // ArduinoOTA.onEnd([]() {
  //   //Serial.println("\nEnd");
  // });
  //ArduinoOTA.onProgress(otaProgressCallback);

  // ArduinoOTA.onError([](ota_error_t error) {
  //   //Serial.printf("Error[%u]: ", error);
  //   if (error == OTA_AUTH_ERROR) {
  //     //Serial.println("Auth Failed");
  //   } else if (error == OTA_BEGIN_ERROR) {
  //     //Serial.println("Begin Failed");
  //   } else if (error == OTA_CONNECT_ERROR) {
  //     //Serial.println("Connect Failed");
  //   } else if (error == OTA_RECEIVE_ERROR) {
  //     //Serial.println("Receive Failed");
  //   } else if (error == OTA_END_ERROR) {
  //     //Serial.println("End Failed");
  //   }
  // });
  ArduinoOTA.begin();
}

void ConfigMenu::setup(){
  activeMenu = MENUSTART;
  menuModeDisplayed = BRIGHTNESS;
  EasyBuzzer.setPin(15);
  otaConnectMessageSent = false;

  displayBrightness = EEPROM.read(EEPROM_BRIGHTNESS_ADDR);
  if (displayBrightness < 0 || displayBrightness > 15) {
    displayBrightness = 0;
  }
  beepVolume = EEPROM.read(EEPROM_BEEP_ADDR);
  if (beepVolume < 0 || beepVolume > 3){
    beepVolume = 0; 
  } 
 }

void ConfigMenu::loop(){
  switch(activeMenu){
  case MENUSTART: // Does not exist
    displayMenu();
    break;
  case OTA:
    displayOtaMenu();
    break;
  case BRIGHTNESS:
    displayBrightnessMenu();
    break;
  case BEEP:
    displayBeepMenu();
    break;
  case MESH:
    displayMeshMenu();
    break;
  }
}

void ConfigMenu::displayBeepMenu(){
  sprintf(displayText, "%2dbEEP", beepVolume);
  displayLed.displayCharArray(displayText, false);
  //EasyBuzzer.update();
}

void ConfigMenu::displayBrightnessMenu(){
  sprintf(displayText, "%2ddisp", displayBrightness);
  displayLed.displayCharArray(displayText, false);
}


void ConfigMenu::displayMenu(){
  switch(menuModeDisplayed){
  case MENUSTART: // Does not exist
  case OTA:
    displayLed.displayCharArray((char*)otaName, false);
    break;
  case BRIGHTNESS:
    displayLed.displayCharArray((char*)brightnessName, false);
    break;
  case BEEP:
    displayLed.displayCharArray((char*)beepName, false);
    break;
  case MESH:
    displayLed.displayCharArray((char*)meshName, false);
    break;
  }
}

void ConfigMenu::displayOtaMenu(){
  if (WiFi.status() == WL_CONNECTED) {
    sprintf(displayText, "On OtA");
    if (!otaConnectMessageSent )
    displayLed.displayTempMessage( (String("IP ") + String(WiFi.localIP()[3])) );
    otaConnectMessageSent = true;
  }
  else{
    sprintf(displayText, "Of OtA");
  }
  displayLed.displayCharArray(String(displayText), false);
  ArduinoOTA.handle();
}

void ConfigMenu::displayMeshMenu(){
  if (meshActive){
    sprintf(displayText, "OnSESH");
  }
  else {
    sprintf(displayText, "OfSESH");
  }
  displayLed.displayCharArray(String(displayText), false);
}

void ConfigMenu::returnAction(){
  activeMenu = MENUSTART;
}

void ConfigMenu::powerAction(){
  switch(activeMenu){
  case MENUSTART:
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
  case MESH:
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
    case OTA:
      activeMenu = menuModeDisplayed;
      turnOnOTA();
      break;
    case BRIGHTNESS:
      activeMenu = menuModeDisplayed;
      break;
    case BEEP:
      activeMenu = menuModeDisplayed;
      break;
    case MESH:
      activeMenu = menuModeDisplayed;
    }
    break;
  case OTA:
    turnOnOTA();
    break;
  case BRIGHTNESS:
    break;
  case BEEP:
    break;
  case MESH:
    break;
  }
}

void ConfigMenu::incrementAction(){
  switch(activeMenu){
  case MENUSTART:
    switch(menuModeDisplayed){
    case MENUSTART: // Does not exist
      break;
    case OTA:
      menuModeDisplayed = BRIGHTNESS;
      break;
    case BRIGHTNESS:
      menuModeDisplayed = BEEP;
      break;
    case BEEP:
      menuModeDisplayed = MESH;
      break;
    case MESH:
      menuModeDisplayed = OTA;
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
  case MESH:
    //meshActive != meshActive;
    meshActive = true;
    break;
  }
}

void ConfigMenu::decrementAction(){
switch(activeMenu){
  case MENUSTART:
    switch(menuModeDisplayed){
    case MENUSTART: // Does not exist
      break;
    case OTA:
      menuModeDisplayed = MESH;
      break;
    case BRIGHTNESS:
      menuModeDisplayed = OTA;
      break;
    case BEEP:
      menuModeDisplayed = BRIGHTNESS;
      break;
    case MESH:
      menuModeDisplayed = BEEP;
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
  case MESH:
    // meshActive != meshActive;
    meshActive = true;
    break;
  }
}

bool ConfigMenu::meshNetworkActive(){
  return meshActive;
}