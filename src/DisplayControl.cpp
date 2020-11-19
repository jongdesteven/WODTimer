#include "DisplayControl.h"

DisplayControl::DisplayControl(const byte load_csPin, const byte colon) :
  colonPin(colon),
  csPin(load_csPin)
  {
  }

void DisplayControl::setup(){
  pinMode(colonPin, OUTPUT);
  digitalWrite(colonPin, LOW);
  
  sprintf(displayText, "------");
  displayRefresh = false;
  lastBlinkChangeMs = millis();
  blinkingSegmentOn = false;
  colonOn = false;

  //begin(csPin, 1, 10000000);
  //setScanLimit(0, 6);
  //shutdown(0, false);
  //setIntensity(0, 8); //0-15
  //clearDisplay(0);
}

void DisplayControl::turnColonOn(bool isOn){
  (isOn) ? digitalWrite(colonPin, HIGH) : digitalWrite(colonPin, LOW);
  colonOn = isOn;
}

char* DisplayControl::getText(){
  return displayText;
}

void DisplayControl::displayCharArray(char *text){
  if (strcmp(displayText, text) != 0){
    strcpy(displayText, text);
    displayRefresh = true;
    blinkingSegments = 0;
  }
}

void DisplayControl::displayCharArray(char *text, byte segmentsToBlink){
  if (strcmp(displayText, text) != 0 || segmentsToBlink != blinkingSegments){
    strcpy(displayText, text);
    blinkingSegments = segmentsToBlink;
    if (segmentsToBlink == 0) displayRefresh = true;
  }
}

void DisplayControl::forceDisplayUpdate(){
  for( int i=0; i<6; i++){
    setChar(0, i, displayText[i], false);
  }
  //Debug
  Serial.print("frcd: |");
  for (int i = 0; i < 2; i++) {
    Serial.print((char)displayText[i]);
  }
  Serial.print(" ");
  for (int i = 2; i < 4; i++) {
    Serial.print((char)displayText[i]);
  }
  if (colonOn) Serial.print(":");
  for (int i = 4; i < 6; i++) {
    Serial.print((char)displayText[i]);
  }
  Serial.println("|");
  //Debug end
}

void DisplayControl::loop(){
  if ( blinkingSegments > 0 && millis() - lastBlinkChangeMs >= 500){
    Serial.print("Blnk: |");
    for( int i=0; i<6; i++){
      if ( blinkingSegmentOn && blinkingSegments & 0x01<<(5-i)  ){
        setChar(0, i, ' ', false);
        Serial.print(" ");
        if (i == 3 && colonOn) Serial.print(":");
      }
      else {
        setChar(0, i, displayText[i], false);
        Serial.print(displayText[i]);
        if (i == 3 && colonOn) Serial.print(":");
      }
    }
    Serial.println("|");
    blinkingSegmentOn = !blinkingSegmentOn;
    lastBlinkChangeMs = millis();
  }
  else if (displayRefresh) {
    Serial.print("Refr: |");
    for( int i=0; i<6; i++){
      setChar(0, i, displayText[i], false);
    }

    //Debug
    for (int i = 0; i < 2; i++) {
      Serial.print((char)displayText[i]);
    }
    Serial.print(" ");
    for (int i = 2; i < 4; i++) {
      Serial.print((char)displayText[i]);
    }
    if (colonOn) Serial.print(":");
    else Serial.print(' ');
    for (int i = 4; i < 6; i++) {
      Serial.print((char)displayText[i]);
    }
    Serial.println("|");
    //Debug end

    displayRefresh = false;
  }
}
