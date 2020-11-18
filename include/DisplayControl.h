#ifndef DISPLAYCONTROL_H
#define DISPLAYCONTROL_H

#include "Arduino.h"
#include "LedControl.h"

class DisplayControl: public LedControl_HW_SPI {
  
private:
  const byte colonPin;
  const byte csPin;
  char displayText[6];
  bool displayRefresh;
  byte blinkingSegments;
  unsigned long lastBlinkChangeMs;
  bool blinkingSegmentOn;
  bool colonOn;
  
  public:
    DisplayControl(int load_csPin, byte colon);
    void turnColonOn(bool isOn);
    char* getText();
    void displayCharArray(char *text);
    /* char text[6], segmentsToBlink is bit mask 1=blink */
    void displayCharArray(char *text, byte segmentsToBlink);
    void forceDisplayUpdate();
    void setup();
    void loop();
};

#endif
