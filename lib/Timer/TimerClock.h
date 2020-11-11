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
  bool preCountDownOn;
  enum State {
    STOPPED = 0,
    PRECOUNTDOWN = 1,
    RUNNING = 2
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
        if (preCountDownOn && secondsLeftThisInterval() < 4){
          EasyBuzzer.singleBeep(
                      5000, 	// Frequency in hertz(HZ).  
                      500 	// Duration of the beep in milliseconds(ms). 
                      //done		// [Optional] Function to call when done.
                      );
        }
      }
      EasyBuzzer.update();
    }

    void preCountDown() {
      // Count down 10-0
      sprintf(displayText,"    %02d", 11-activeSecond);
      // Beep short short long on last 3 seconds
      displayLed.displayCharArray(displayText);
      beepAtTheEnd();
    }

    void countDown() {
      sprintf(displayText,"%02d%02d%02d", roundsLeft(), secondsLeftThisInterval()/60, secondsLeftThisInterval()%60);
    }

    void countUp() {
      //Todo: intervals support
      if ((activeSecond)/60 < 60) { //first hour, show "UP"
        sprintf(displayText,"%2s%02d%02d", activeOption.getDisplayName(), (activeSecond)/60, (activeSecond)%60);
      }
      else { //passed the hour
        sprintf(displayText,"%02d%02d%02d", (activeSecond)/3600, ((activeSecond)/60)%60, (activeSecond)%60);
      }
    }
    
  public:

    TimerClock(MenuOption &optionToActivate, DisplayControl &displayLedToAttach) :
      activeOption(optionToActivate),
      displayLed(displayLedToAttach)
    {
    }

    int roundsLeft(){
      int roundTime = (activeOption.getStartTime1() + activeOption.getStartTime2());

      if (activeOption.hasRounds()){
        return ceil(((activeOption.getNrOfRounds()/2 * roundTime) - activeSecond) / roundTime);
      }
      else {
        return ceil(((activeOption.getNrOfRounds() * roundTime) - activeSecond) / roundTime);
      }
    }

    int secondsLeftThisInterval(){
      if (roundsLeft() == 0) return 0; //timer end show 0;
      if (roundsLeft()%2) { //Interval 1
        return activeOption.getStartTime1()-(activeSecond % activeOption.getStartTime1());
      }
      else { //Interval 2
        return activeOption.getStartTime2()-(activeSecond % activeOption.getStartTime1());
      }
    }

    void startClock() {
      // public to skip precountdown
      switch (state) {
          case PRECOUNTDOWN:
            state = RUNNING;
            activeSecond = 1;
            startTimeMs = millis();
            displayLed.turnColonOn(true);
            break;
          case RUNNING:
            //activeSecond = 0;
            state = STOPPED;
            displayLed.turnColonOn(true);
            break;
          case STOPPED:
            state = PRECOUNTDOWN;
            activeSecond = 1;
            startTimeMs = millis();
            displayLed.turnColonOn(false);
            break;
      }
    }

    bool timerRunning(){
      return state ? true : false;
    }

    void setup(MenuOption &optionToAttach) {
      activeOption = optionToAttach;
      state = STOPPED;
    }

    void loop() {
      switch (state) {
        case PRECOUNTDOWN:
          if ( (millis() - startTimeMs) >= activeSecond*1000 ) {
            preCountDown();
            if (activeSecond - 11 == 0) {
              startClock();
            }
            activeSecond++;
          }
          break;
        case RUNNING:
          if ( (millis() - startTimeMs) >= activeSecond*1000 ) {
            if (activeOption.getCountDirectionUp()) {
              countUp();
            }
            else {
              countDown();
            }
            if (roundsLeft() == 0){
              state = STOPPED;
            }
            activeSecond++;
          }
          break;
        case STOPPED:
          // Display Reset mode
          if( activeOption.getCountDirectionUp() ) {
            sprintf(displayText,"%2s%02d%02d", activeOption.getDisplayName(), (activeSecond)/60, (activeSecond)%60);
          }
          else {
            sprintf(displayText,"%02d%02d%02d", roundsLeft(), secondsLeftThisInterval()/60, secondsLeftThisInterval()%60);
          }
          break; 
      }
      //displayText on display
      displayLed.displayCharArray(displayText);
      beepAtTheEnd();
    }
};

#endif