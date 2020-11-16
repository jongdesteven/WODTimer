#ifndef TIMERCLOCK_H
#define TIMERCLOCK_H

#include "DisplayControl.h"
#include "MenuOption.h"
#include "EasyBuzzer.h"

class TimerClock {
  MenuOption &activeOption;
  DisplayControl &displayLed;
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
  
//Todo: Add reference to display
private:
  void beepAtTheEnd() {
    if (secondsLeftThisInterval() == 0) {
      EasyBuzzer.singleBeep(
                    5000, 	// Frequency in hertz(HZ).  
                    1000 	// Duration of the beep in milliseconds(ms). 
                    //done		// [Optional] Function to call when done.
                    );
    }
    else {
      if (state == PRECOUNTDOWN && secondsLeftThisInterval() < 4){
        EasyBuzzer.singleBeep(
                    5000, 	// Frequency in hertz(HZ).  
                    500 	// Duration of the beep in milliseconds(ms). 
                    //done		// [Optional] Function to call when done.
                    );
      }
    }
    EasyBuzzer.update();
  }

  int roundsIn(){
    int roundTime = activeOption.getStartTime1() + activeOption.getStartTime2();
    return ceil( activeSecond / roundTime );
  }

  int secondThisRound() {
    int roundTime = activeOption.getStartTime1() + activeOption.getStartTime2();
    return activeSecond - ( (roundsIn()-1) * roundTime );
  }

  int secondThisInterval() {
    int secondThisInterval = 0;
    if ( secondThisRound() <= activeOption.getStartTime1() ) {
      secondThisInterval = secondThisRound();
    }
    else {
      secondThisInterval = secondThisRound() - activeOption.getStartTime1() ;
    }
    return secondThisInterval;
  }

  int intervalsIn() {
    int intervalsIn = 0;
    if ( secondThisRound() <= activeOption.getStartTime1() ) {
      intervalsIn = 2*(roundsIn()-1) + 1;
    }
    else {
      intervalsIn = roundsIn() * 2;
    }
    return intervalsIn;
  }

  int roundsLeft() {
    return 1 + activeOption.getNrOfRounds() - roundsIn();
  }

  int intervalsLeft() {
    return 1 + activeOption.getNrOfRounds()*2 - intervalsIn();
  }

  int secondsLeftThisInterval() {
    int secondsLeftThisInterval = 0;
    if ( intervalsIn() %2 ) {
      secondsLeftThisInterval = activeOption.getStartTime1() - secondThisInterval();
    }
    else {
      secondsLeftThisInterval = activeOption.getStartTime2() - secondThisInterval();
    }
    return secondsLeftThisInterval;
  }
  
public:
  TimerClock(MenuOption &optionToActivate, DisplayControl &displayLedToAttach) :
    activeOption(optionToActivate),
    displayLed(displayLedToAttach)
  {
  }

  void startClock() {
    // public to skip precountdown
    Serial.print("startClock: from ");
    Serial.print(state);
    Serial.print(" to ");
    switch (state) {
    case PRECOUNTDOWN:
      if(activeOption.getCountDirectionUp()){
        state = RUN_UP;
      }
      else{
        state = RUN_DOWN;
      }
      activeSecond = 1;
      startTimeMs = millis();
      displayLed.turnColonOn(true);
      break;
    case RUN_UP:
    case RUN_DOWN:
      state = PAUSE;
      displayLed.turnColonOn(false); //TODO: Change to something else?
      break;
    case PAUSE:
      if(activeOption.getCountDirectionUp()){
        state = RUN_UP;
      }
      else{
        state = RUN_DOWN;
      }
      startTimeMs = millis()-(activeSecond*1000);
      break;
    case TIMER_END:
      activeSecond = 1;
      displayLed.turnColonOn(false);
      startTimeMs = millis();
      state = PRECOUNTDOWN;
      break;
    }
    Serial.println(state);
  }

  void setup(MenuOption &optionToAttach) {
    activeOption = optionToAttach;
    state = TIMER_END;
  }

  void loop() {
    if ( (millis() - startTimeMs) >= (unsigned long)activeSecond*1000 ) {
      switch (state) {
      case PRECOUNTDOWN :
        // Count down 10-0
        sprintf(displayText,"    %02d", 11-activeSecond);
        // Beep short short long on last 3 seconds
        activeSecond++;
        if (activeSecond - 12 >= 0) {
          startClock();
        } 
          Serial.print("Timer Loop PRECOUNTDOWN:");
          Serial.print(activeSecond);
          Serial.print(" roundsLeft: ");
          Serial.println(roundsLeft());
        break;
      case RUN_UP:
        if ( activeOption.getNrOfRounds() > 0){
          sprintf(displayText,"%2d%02d%02d", roundsIn(), secondThisInterval()/60, secondThisInterval()%60);
        }
        else {
          if (((activeSecond)/60) < 60) { //first hour, show "UP"
          sprintf(displayText,"%2s%02d%02d", activeOption.getDisplayName(), secondThisInterval()/60, secondThisInterval()%60);
          }
          else { //passed the hour
            sprintf(displayText,"%02d%02d%02d", secondThisInterval()/3600, (secondThisInterval()/60)%60, secondThisInterval()%60);
          }
        } 
        if (roundsLeft() <= 0){
          state = TIMER_END;
        }
          Serial.print("Timer Loop RUN_UP: ");
          Serial.print(activeSecond);
          Serial.print(" roundsIn: ");
          Serial.println(roundsIn());

        activeSecond++;
        break;
      case RUN_DOWN:
        sprintf(displayText,"%02d%02d%02d", roundsLeft(), secondsLeftThisInterval()/60, secondsLeftThisInterval()%60);
        if (roundsLeft() <= 0){
          state = TIMER_END;
        }
        Serial.print("Timer Loop RUN_DOWN: ");
        Serial.print(secondsLeftThisInterval());
        Serial.print(" roundsLeft: ");
        Serial.println(roundsLeft());
        Serial.print(" secondsleft: ");
        Serial.println(secondsLeftThisInterval());
        activeSecond++;
        break;
      case PAUSE:
        // TODO Blink Colon ?
        //Stop Clock
        // if( activeOption.getCountDirectionUp() ) {
        //   sprintf(displayText,"%2s%02d%02d", activeOption.getDisplayName(), (int)(activeSecond)/60, (int)(activeSecond)%60);
        // }
        // else {
        //   sprintf(displayText,"%02d%02d%02d", roundsLeft(), (int)secondsLeftThisInterval()/60, (int)secondsLeftThisInterval()%60);
        // }
        break; 
      case TIMER_END:
        break;
      }
    }
    //displayText on display
    displayLed.displayCharArray(displayText);
    beepAtTheEnd();
  }
};

#endif