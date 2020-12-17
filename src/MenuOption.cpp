#include "MenuOption.h"

MenuOption::MenuOption(const char* name, int time1, int time2, int rounds, bool countUp, bool interval, EepromLayout address) :
  displayName(name),
  startTimeInterval1Sec(time1),
  startTimeInterval2Sec(time2),
  nrOfRounds(rounds),
  countDirectionUp(countUp),
  includesInterval(interval),
  eepromAddress(address)
{
}

void MenuOption::setup(){
  bool changes = false;
  if ( EEPROM.read(eepromAddress+0)<0 || EEPROM.read(eepromAddress+0)>59){
    changes = true;
  } 
  else if ( EEPROM.read(eepromAddress+1)<0 || EEPROM.read(eepromAddress+1)>59){
    changes = true;
  }
  else {
    startTimeInterval1Sec = EEPROM.read(eepromAddress+0)*60 + EEPROM.read(eepromAddress+1);
  }

  if (startTimeInterval2Sec > 0){
    if ( EEPROM.read(eepromAddress+2)<0 || EEPROM.read(eepromAddress+2)>59){
      changes = true;
    } 
    else if ( EEPROM.read(eepromAddress+3)<0 || EEPROM.read(eepromAddress+3)>59){
      changes = true;
    }
    else {
      startTimeInterval2Sec = EEPROM.read(eepromAddress+2)*60 + EEPROM.read(eepromAddress+3);
    }
  }

  if (nrOfRounds > 0){
    if ( EEPROM.read(eepromAddress+4)<1 || EEPROM.read(eepromAddress+4)>99){
      changes = true;
    } 
    else {
      nrOfRounds = EEPROM.read(eepromAddress+4);
    }
  }

  if (changes){
    saveChanges();
  }
  
}

const char* MenuOption::getDisplayName(){
  return displayName;
}

int MenuOption::getStartTime1(){
  return startTimeInterval1Sec;
}

int MenuOption::getStartTime2(){
  return startTimeInterval2Sec;
}

int MenuOption::getNrOfRounds(){
  return nrOfRounds;
}

bool MenuOption::getCountDirectionUp(){
  return countDirectionUp;
}

void MenuOption::changeRounds(int change){
  // do not add rounds if it never had
  if (nrOfRounds > 0){
    nrOfRounds += change;
    if (nrOfRounds < 1) { nrOfRounds = 1;}
    if (nrOfRounds > 99) { nrOfRounds = 99; }
  }
}

void MenuOption::changeTimeInterval(int interval, int changeSec){
  switch (interval){
  case 1:
    startTimeInterval1Sec += changeSec;
    if (startTimeInterval1Sec < 1){ startTimeInterval1Sec = 1; }
    if (startTimeInterval1Sec > 3599){ startTimeInterval1Sec = 3599; }
    break;
  case 2:
    startTimeInterval2Sec += changeSec;
    if (startTimeInterval2Sec < 0){ startTimeInterval2Sec = 0; }
    if (startTimeInterval2Sec > 3599){ startTimeInterval2Sec = 3599; }
    break; 
  }
}

int MenuOption::saveChanges(){
  bool changes = false;
  if ( (startTimeInterval1Sec/60) != EEPROM.read(eepromAddress+0) ){
    EEPROM.write(eepromAddress+0, startTimeInterval1Sec/60);
    changes = true;
  } 
  if ( (startTimeInterval1Sec%60) != EEPROM.read(eepromAddress+1) ){
    EEPROM.write(eepromAddress+1, startTimeInterval1Sec%60);
    changes = true;
  } 
  if ( (startTimeInterval2Sec/60) != EEPROM.read(eepromAddress+2) ){
    EEPROM.write(eepromAddress+2, startTimeInterval2Sec/60);
    changes = true;
  }
  if ( (startTimeInterval2Sec%60) != EEPROM.read(eepromAddress+3) ){
    EEPROM.write(eepromAddress+3, startTimeInterval2Sec%60);
    changes = true;
  } 
  if ( (nrOfRounds) != EEPROM.read(eepromAddress+4) ){
    EEPROM.write(eepromAddress+4, nrOfRounds);
    changes = true;
  } 

  if (changes){
    return EEPROM.commit();
  }
  else
  {
    return 0;
  }
}
