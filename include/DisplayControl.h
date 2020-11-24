#ifndef DISPLAYCONTROL_H
#define DISPLAYCONTROL_H

#include "Arduino.h"
#include "LedControl_HW_SPI.h"

class DisplayControl: private LedControl_HW_SPI {
  
private:
  const byte colonPin;
  const byte csPin;
  char displayText[6];
  bool displayRefresh;
  byte blinkingSegments;
  unsigned long lastBlinkChangeMs;
  bool blinkingSegmentOn;
  bool colonIsOn;
  
  public:
    DisplayControl(const byte load_csPin, const byte colon);
    char* getText();
    void displayCharArray(char *text, bool colonOn);
    /* char text[6], segmentsToBlink is bit mask 1=blink */
    void displayCharArray(char *text, byte segmentsToBlink, bool colonOn);
    void forceDisplayUpdate();
    void setup();
    void loop();
};

#endif
