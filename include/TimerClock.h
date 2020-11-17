#ifndef TIMERCLOCK_H
#define TIMERCLOCK_H

#include "DisplayControl.h"
#include "MenuOption.h"
#include "EasyBuzzer.h"

class TimerClock {
//Todo: Add reference to display
private:
  DisplayControl &displayLed;
  MenuOption *activeOption;
  char displayText[6];
  unsigned long startTimeMs;
  enum State {
    PAUSE,
    PRECOUNTDOWN,
    RUN_UP,
    RUN_DOWN,
    TIMER_END
  }state;
  int activeSecond;
  
  void beepAtTheEnd();
  int roundsIn();
  int secondThisRound();
  int secondThisInterval();
  int intervalsIn();
  int roundsLeft();
  int intervalsLeft();
  int secondsLeftThisInterval();
    
public:
  TimerClock(DisplayControl &displayLedToAttach, MenuOption *optionToActivate);
  void startClock();
  void setup(MenuOption *optionToAttach);
  void loop();
};
#endif
