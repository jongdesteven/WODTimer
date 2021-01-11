#ifndef MENUOPTION_H
#define MENUOPTION_H

#include <Arduino.h>
#include "eepromLayout.h"

class MenuOption {
private:
  String displayName; //2 green characters
  unsigned long startTimeInterval1Sec = 60;
  unsigned long startTimeInterval2Sec = 0;
  int nrOfRounds = 0;
  bool countDirectionUp = true;
  EepromLayout eepromAddress = EEPROM_DO_NOT_SAVE;

public:
  MenuOption(String name, unsigned long time1, unsigned long  time2, int rounds, bool countUp, EepromLayout eeAddress);
  MenuOption();
  void initialize(String name, unsigned long time1, unsigned long  time2, int rounds, bool countUp, EepromLayout eeAddress);
  void setup();
  String getDisplayName();
  unsigned long getStartTime1();
  unsigned long getStartTime2();
  int getNrOfRounds();
  bool getCountDirectionUp();
  void changeRounds(int change);
  void changeTimeInterval(int interval, int changeSec);
  int saveChanges();
};

#endif
