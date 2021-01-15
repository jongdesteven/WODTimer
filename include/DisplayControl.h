#ifndef DISPLAYCONTROL_H
#define DISPLAYCONTROL_H

#include <Arduino.h>
#include "LedControl_HW_SPI.h"
#include "eepromLayout.h"

#define BLINKSPEED_MS 500
#define TEMPMESSAGE_MIN_MS 2000
#define SCROLLSPEED_MS  500

class DisplayControl: public LedControl_HW_SPI {
  
private:
  const byte csPin;
  String displayText;
  String tempMessage;
  bool displayRefresh;
  byte blinkingSegments;
  unsigned long lastBlinkChangeMs;
  unsigned long longTextLastScrollMs;
  unsigned long startTempMessageMs;
  unsigned int tempMessageStartIndex;
  bool tempMessageOn;
  bool blinkingSegmentOn;
  bool colonIsOn;
  
  public:
    DisplayControl(const byte load_csPin);
    String getText();
    void displayCharArray(String text, bool colonOn);
    /* char text[6], segmentsToBlink is bit mask 1=blink */
    void displayCharArray(String text, byte segmentsToBlink, bool colonOn);
    void displayTempMessage(String text);
    void forceDisplayUpdate();
    void setup();
    void loop();
    void start();
};

#endif
