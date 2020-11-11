#ifndef DISPLAYCONTROL_H
#define DISPLAYCONTROL_H

#include "Arduino.h"
#include "LedControl.h"

class DisplayControl: public LedControl {
  const byte colonPin;
  char oldText[6];
  
  public:
    DisplayControl(int dataIn, int clk, int load, byte colon) :
      LedControl(dataIn, clk, load, 1),
      colonPin(colon)
      {
      }

    void setup(){
      pinMode(colonPin, OUTPUT);
      digitalWrite(colonPin, LOW);
      sprintf(oldText, "------");
      
      setScanLimit(0, 6);
      shutdown(0, false);
      setIntensity(0, 8); //0-15
      clearDisplay(0);
    }

    void turnColonOn(bool isOn){
      (isOn) ? digitalWrite(colonPin, HIGH) : digitalWrite(colonPin, LOW);
    }

    void loop(){
      
    }

    void displayCharArray(char *text){
      if (strcmp(oldText, text) != 0){
        for( int i=0; i<6; i++){
          setChar(0, i, text[i], false);
        }
        strcpy(oldText, text);
      }
    }

};

#endif