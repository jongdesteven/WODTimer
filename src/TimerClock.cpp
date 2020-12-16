#include "TimerClock.h"

TimerClock::TimerClock(DisplayControl &displayLedToAttach, MenuOption *optionToActivate) :
  displayLed(displayLedToAttach),
  activeOption(optionToActivate)
{
}

void TimerClock::beepAtTheEnd() {
  int beepFreq = 0;
  if (beepVolume == 0) return;
  else if (beepVolume == 1) beepFreq = 3000;
  else if ( beepVolume == 2) beepFreq = 2000;

  if (state == PRECOUNTDOWN ){
    if (activeSecond == 10){
      EasyBuzzer.singleBeep(
                  beepFreq, 	// Frequency in hertz(HZ).  
                  1000 	// Duration of the beep in milliseconds(ms). 
                  //done		// [Optional] Function to call when done.
                  );
      Serial.println("Beeeeep!");
    }
    else if (activeSecond >= 7){
      EasyBuzzer.singleBeep(
                  beepFreq, 	// Frequency in hertz(HZ).  
                  500	// Duration of the beep in milliseconds(ms). 
                  //done		// [Optional] Function to call when done.
                  );
      Serial.println("Beep!");
    }
  }
  else if (secondsLeftThisInterval() == 0) {
    EasyBuzzer.singleBeep(
                  beepFreq, 	// Frequency in hertz(HZ).  
                  1000 	// Duration of the beep in milliseconds(ms). 
                  //done		// [Optional] Function to call when done.
                  );
    Serial.println("Beeeeep!");
  }
}

int TimerClock::roundsIn(){
  int roundTime = activeOption->getStartTime1() + activeOption->getStartTime2();
  return ceil( (double)activeSecond / (double)roundTime );  \
}

int TimerClock::secondThisRound() {
  int roundTime = activeOption->getStartTime1() + activeOption->getStartTime2();
  return activeSecond - 1 - ( (roundsIn()-1) * roundTime );
}

int TimerClock::secondThisInterval() {
  int secondThisInterval = 0;
  if ( secondThisRound() < activeOption->getStartTime1() ) {
    secondThisInterval = secondThisRound();
  }
  else {
    secondThisInterval = secondThisRound() - activeOption->getStartTime1() ;
  }
  return secondThisInterval;
}

int TimerClock::intervalsIn() {
  int intervalsIn = 0;
  if ( secondThisRound() < activeOption->getStartTime1() ) {
    intervalsIn = 2*(roundsIn()-1) + 1;
  }
  else {
    intervalsIn = roundsIn() * 2;
  }
  return intervalsIn;
}

int TimerClock::roundsLeft() {
  int nrOfRounds = activeOption->getNrOfRounds();
  if (nrOfRounds == 0){
      nrOfRounds = 1;
  }
  
  return 1 + nrOfRounds - roundsIn();
  
}

int TimerClock::intervalsLeft() {
  int nrOfRounds = activeOption->getNrOfRounds();
  if (nrOfRounds == 0){
      nrOfRounds = 1;
  }
  return 1 + nrOfRounds*2 - intervalsIn();
}

int TimerClock::secondsLeftThisInterval() {
  int secondsLeftThisInterval = 0;
  if ( intervalsIn() %2 ) {
    secondsLeftThisInterval = activeOption->getStartTime1() - secondThisInterval() - 1;
  }
  else {
    secondsLeftThisInterval = activeOption->getStartTime2() - secondThisInterval() - 1;
  }
  return secondsLeftThisInterval;
}

void TimerClock::startClock() {
  switch (state) {
  case PRECOUNTDOWN:
    if(activeOption->getCountDirectionUp()){
      state = RUN_UP;
    }
    else{
      state = RUN_DOWN;
    }
    activeSecond = 1;
    startTimeMs = millis()-1000;
    break;
  case RUN_UP:
  case RUN_DOWN:
    state = PAUSE;
    //displayLed.turnColonOn(false); //TODO: Change to something else?
    break;
  case PAUSE:
    if(activeOption->getCountDirectionUp()){
      state = RUN_UP;
    }
    else{
      state = RUN_DOWN;
    }
    startTimeMs = millis()-(activeSecond*1000);
    break;
  case TIMER_END:
    activeSecond = 0;
    startTimeMs = millis();
    state = PRECOUNTDOWN;
    break;
  }
}

void TimerClock::setup(MenuOption *optionToAttach) {
  activeOption = optionToAttach;
  sprintf(displayText, "      ");
  state = TIMER_END;
  EasyBuzzer.setPin(15);
  beepVolume = EEPROM.read(EEPROM_BEEP_ADDR);
  if (beepVolume < 0 || beepVolume > 2){
    beepVolume = 0;
  }
}

void TimerClock::loop() {
  if ( (millis() - startTimeMs) >= (unsigned long)activeSecond*1000 ) {
    switch (state) {
    case PRECOUNTDOWN :
      // Count down 10-0
      if (activeSecond <= 9) {
        sprintf(displayText,"    %02d", 10-activeSecond);
        // Beep short short long on last 3 seconds
        beepAtTheEnd();
        activeSecond++;
      }
      else {
        beepAtTheEnd();
        startClock();
      }
      displayLed.displayCharArray(displayText, false);
      break;
    case RUN_UP:
      if (roundsLeft() > 0){
        if ( activeOption->getNrOfRounds() > 0){
          sprintf(displayText,"%2d%02d%02d", roundsIn(), secondThisInterval()/60, secondThisInterval()%60);
        }
        else {
          if (((activeSecond)/60) < 60) { //first hour, show "UP"
            sprintf(displayText,"%2s%02d%02d", activeOption->getDisplayName(), secondThisInterval()/60, secondThisInterval()%60);
          }
          else { //passed the hour
            sprintf(displayText,"%02d%02d%02d", secondThisInterval()/3600, (secondThisInterval()/60)%60, secondThisInterval()%60);
          }
        } 
        beepAtTheEnd();
        activeSecond++;
      }
      else {
        state = TIMER_END;
      }
      displayLed.displayCharArray(displayText, true);
      break;
    case RUN_DOWN:
      if (roundsLeft() > 0){
        if (activeOption->getStartTime2() > 0) {
          sprintf(displayText,"%02d%02d%02d", intervalsLeft(), secondsLeftThisInterval()/60, secondsLeftThisInterval()%60);
        }
        else if ( activeOption->getNrOfRounds() > 0){
          sprintf(displayText,"%02d%02d%02d", roundsLeft(), secondsLeftThisInterval()/60, secondsLeftThisInterval()%60);
        }
        else {
          sprintf(displayText,"%2s%02d%02d", activeOption->getDisplayName(), secondsLeftThisInterval()/60, secondsLeftThisInterval()%60);
        }
        beepAtTheEnd();
        activeSecond++;
      }
      else {
        state = TIMER_END;
      }
      displayLed.displayCharArray(displayText, true);
      break;
    case PAUSE:
      displayLed.displayCharArray(displayText, 0b001111, true);
      break; 
    case TIMER_END:
      displayLed.displayCharArray(displayText, true);
      break;
    }
  }
  EasyBuzzer.update();
}
