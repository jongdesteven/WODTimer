#include "DisplayControl.h"

DisplayControl::DisplayControl(const byte load_csPin) :
  csPin(load_csPin)
  {
  }

void DisplayControl::setup(){
  displayText = "------";
  displayRefresh = false;
  lastBlinkChangeMs = millis();
  blinkingSegmentOn = false;
  colonIsOn = false;

  begin(csPin, 1, 10000000);
  shutdown(0, true);
}

String DisplayControl::getText(){
  return displayText;
}

void DisplayControl::start(){
  int intensity = EEPROM.read(EEPROM_BRIGHTNESS_ADDR);
  if (intensity < 0 || intensity > 15) { 
    intensity = 5;
  }
  setScanLimit(0, 7); //6 digits, Colon to digit7
  setIntensity(0, intensity);
  shutdown(0, false);
}

void DisplayControl::displayCharArray(String text, bool colonOn){
  colonIsOn = colonOn;
  if ( displayText.compareTo(text) != 0 ){
    displayText = text;
    displayRefresh = true;
    blinkingSegments = 0;
  }
}

void DisplayControl::displayCharArray(String text, byte segmentsToBlink, bool colonOn){
  colonIsOn = colonOn;
  if ( displayText.compareTo(text) != 0 || segmentsToBlink != blinkingSegments){
    displayText = text;
    blinkingSegments = segmentsToBlink;
    displayRefresh = true;
  }
}

void DisplayControl::forceDisplayUpdate(){
  displayRefresh = true;
}

void DisplayControl::loop(){
  if (displayRefresh) {
    for( int i=0; i<6; i++){
      setChar(0, i, displayText.charAt(i), false);
    }
    setChar(0, 6, ' ', colonIsOn);
    displayRefresh = false;
  }
  else if ( blinkingSegments > 0 && millis() - lastBlinkChangeMs >= 500){
    for( int i=0; i<6; i++){
      if ( blinkingSegmentOn && blinkingSegments & 0x01<<(5-i)  ){
        setChar(0, i, ' ', false);
      }
      else {
        setChar(0, i, displayText.charAt(i), false);
      }
    }
    setChar(0, 6, ' ', colonIsOn);
    blinkingSegmentOn = !blinkingSegmentOn;
    lastBlinkChangeMs = millis();
  }
}
