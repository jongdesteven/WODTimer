#include "DisplayControl.h"

DisplayControl::DisplayControl(const byte load_csPin, const byte colon) :
  colonPin(colon),
  csPin(load_csPin)
  {
  }

void DisplayControl::setup(){
  //pinMode(colonPin, OUTPUT);
  //digitalWrite(colonPin, LOW);
  
  
  sprintf(displayText, "------");
  displayRefresh = false;
  lastBlinkChangeMs = millis();
  blinkingSegmentOn = false;
  colonIsOn = false;

  Serial.print("Starting MAX7219..");

  begin(csPin, 1, 10000000);
  setScanLimit(0, 7); //6 digits, Colon to digit7
  shutdown(0, false);
  setIntensity(0, 0); //0-15
  clearDisplay(0);
  Serial.println("..Started");
}

char* DisplayControl::getText(){
  return displayText;
}

void DisplayControl::displayCharArray(char *text, bool colonOn){
  colonIsOn = colonOn;
  if (strcmp(displayText, text) != 0){
    strcpy(displayText, text);
    displayRefresh = true;
    blinkingSegments = 0;
  }
}

void DisplayControl::displayCharArray(char *text, byte segmentsToBlink, bool colonOn){
  colonIsOn = colonOn;
  if (strcmp(displayText, text) != 0 || segmentsToBlink != blinkingSegments){
    strcpy(displayText, text);
    blinkingSegments = segmentsToBlink;
    displayRefresh = true;
  }
}

void DisplayControl::forceDisplayUpdate(){
  displayRefresh = true;
}

void DisplayControl::loop(){
  if (displayRefresh) {
    Serial.print("Refr: |");
    for( int i=0; i<6; i++){
      setChar(0, i, displayText[i], false);
    }
    //if(colonIsOn) ? digitalWrite(colonPin, HIGH) : digitalWrite(colonPin, LOW);
    setChar(0, 6, ' ', colonIsOn);

    //Debug
    for (int i = 0; i < 2; i++) {
      Serial.print((char)displayText[i]);
    }
    for (int i = 2; i < 4; i++) {
      Serial.print((char)displayText[i]);
    }
    if (colonIsOn) Serial.print(":");
    else Serial.print(' ');
    for (int i = 4; i < 6; i++) {
      Serial.print((char)displayText[i]);
    }
    Serial.println("|");
    //Debug end
    displayRefresh = false;
  }
  else if ( blinkingSegments > 0 && millis() - lastBlinkChangeMs >= 500){
    Serial.print("Blnk: |");
    for( int i=0; i<6; i++){
      if ( blinkingSegmentOn && blinkingSegments & 0x01<<(5-i)  ){
        setChar(0, i, ' ', false);
        
        Serial.print(" ");
        if (i == 3 && colonIsOn) Serial.print(":");
      }
      else {
        setChar(0, i, displayText[i], false);

        Serial.print(displayText[i]);
        if (i == 3 && colonIsOn) Serial.print(":");
      }
    }
    Serial.println("|");

    //(colonIsOn) ? digitalWrite(colonPin, HIGH) : digitalWrite(colonPin, LOW);
    setChar(0, 6, ' ', colonIsOn);
    
    blinkingSegmentOn = !blinkingSegmentOn;
    lastBlinkChangeMs = millis();
  }
  
}
