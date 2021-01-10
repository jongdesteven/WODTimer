#ifndef MENUOPTION_H
#define MENUOPTION_H

#include <Arduino.h>
#include "eepromLayout.h"

class MenuOption {
private:
  char* displayName = "--"; //2 green characters
  unsigned long startTimeInterval1Sec = 60;
  unsigned long startTimeInterval2Sec = 0;
  int nrOfRounds = 0;
  bool countDirectionUp = true;
  EepromLayout eepromAddress = EEPROM_DO_NOT_SAVE;

public:
  MenuOption(const char* name, unsigned long time1, unsigned long  time2, int rounds, bool countUp, EepromLayout eeAddress);
  MenuOption();
  void initialize(const char* name, unsigned long time1, unsigned long  time2, int rounds, bool countUp, EepromLayout eeAddress);
  void setup();
  const char* getDisplayName();
  unsigned long getStartTime1();
  unsigned long getStartTime2();
  int getNrOfRounds();
  bool getCountDirectionUp();
  void changeRounds(int change);
  void changeTimeInterval(int interval, int changeSec);
  int saveChanges();
};

#endif
