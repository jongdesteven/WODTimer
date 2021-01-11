#ifndef DISPLAYCONTROL_H
#define DISPLAYCONTROL_H

#include <Arduino.h>
#include "LedControl_HW_SPI.h"
#include "eepromLayout.h"

class DisplayControl: public LedControl_HW_SPI {
  
private:
  const byte csPin;
  String displayText;
  bool displayRefresh;
  byte blinkingSegments;
  unsigned long lastBlinkChangeMs;
  bool blinkingSegmentOn;
  bool colonIsOn;
  
  public:
    DisplayControl(const byte load_csPin);
    String getText();
    void displayCharArray(String text, bool colonOn);
    /* char text[6], segmentsToBlink is bit mask 1=blink */
    void displayCharArray(String text, byte segmentsToBlink, bool colonOn);
    void forceDisplayUpdate();
    void setup();
    void loop();
    void start();
};

#endif
