#ifndef MENUOPTION_H
#define MENUOPTION_H

#include "eepromLayout.h"

class MenuOption {
private:
  const char* displayName; //2 green characters
  int startTimeInterval1Sec;
  int startTimeInterval2Sec;
  int nrOfRounds;
  bool countDirectionUp;
  bool includesInterval;
  EepromLayout eepromAddress;

public:
  MenuOption(const char* name, int time1, int time2, int rounds, bool countUp, bool interval, EepromLayout eepromAddress);
  void setup();
  const char* getDisplayName();
  int getStartTime1();
  int getStartTime2();
  int getNrOfRounds();
  bool getCountDirectionUp();
  void changeRounds(int change);
  void changeTimeInterval(int interval, int changeSec);
  int saveChanges();
};

#endif
