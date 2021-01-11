#include "TimerClock.h"

TimerClock::TimerClock(DisplayControl &displayLedToAttach) :
  displayLed(displayLedToAttach)
{
}

void TimerClock::beepAtTheEnd() {
  unsigned int beepFreq = 0;
  unsigned int shortBeep = 500;
  unsigned int longBeep = 1000;
  if (beepVolume == 0) return;
  else if (beepVolume == 1) beepFreq = 3000;
  else if ( beepVolume == 2) beepFreq = 2000;

  switch (state) {
  case PRECOUNTDOWN:
    if (activeSecond >= 7 && activeSecond < 10){
      EasyBuzzer.singleBeep(beepFreq, shortBeep);
    }
    break;
  case RUN_UP: //Beeps at end of interval
    if (activeSecond == 0){
      EasyBuzzer.singleBeep(beepFreq, longBeep);
    }
    else if (secondThisInterval() == activeOption->getStartTime1()){
      if (roundsLeft() == 1){ // End of the timer
        EasyBuzzer.singleBeep(beepFreq, longBeep);
      }
      else { //Round end, rounds left
        EasyBuzzer.singleBeep(beepFreq, shortBeep);
      }
    }
    break;
  case RUN_DOWN: //Beeps at start of interval
    if (roundsLeft() == 0){ // End of the timer
      EasyBuzzer.singleBeep(beepFreq, longBeep);
    }
    else if ( intervalsIn()%2 && secondsLeftThisInterval() == activeOption->getStartTime1() ) {
      if (intervalsIn() == 1){ //Long at start of first interval as start
        EasyBuzzer.singleBeep(beepFreq, longBeep);
      }
      else{ 
        EasyBuzzer.singleBeep(beepFreq, shortBeep);
      }
    }
    else if ( !(intervalsIn()%2) && secondsLeftThisInterval() == activeOption->getStartTime2() ) {
      EasyBuzzer.singleBeep(beepFreq, shortBeep);
    }
    break;
  case PAUSE:
  case TIMER_END:
    break;
  }
}

int TimerClock::roundsIn(){
  int roundTime = activeOption->getStartTime1() + activeOption->getStartTime2();
  return ceil( (double)activeSecond / (double)roundTime ); 
}

int TimerClock::secondThisRound() {
  int roundTime = activeOption->getStartTime1() + activeOption->getStartTime2();
  return activeSecond-1 - ((roundsIn()-1) * roundTime - 1);
  // return activeSecond - 1 - ( (roundsIn()-1) * roundTime );
}

int TimerClock::secondThisInterval() {
  int secondThisInterval = 0;
  if ( secondThisRound() <= activeOption->getStartTime1() ) {
    secondThisInterval = secondThisRound();
  }
  else {
    secondThisInterval = secondThisRound() - activeOption->getStartTime1() ;
  }
  return secondThisInterval;
}

int TimerClock::intervalsIn() {
  int intervalsIn = 0;
  if ( secondThisRound() <= activeOption->getStartTime1() ) {
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
    secondsLeftThisInterval = activeOption->getStartTime1() - secondThisInterval() +1;
  }
  else {
    secondsLeftThisInterval = activeOption->getStartTime2() - secondThisInterval() +1;
  }
  return secondsLeftThisInterval;
}

void TimerClock::startClock() {
  switch (state) {
  case PRECOUNTDOWN:
    if(activeOption->getCountDirectionUp()){
      state = RUN_UP;
      activeSecond = 0;
      startTimeMs = millis();
    }
    else{
      state = RUN_DOWN;
      activeSecond = 1;
      startTimeMs = millis()-1000;
    }
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
  //sprintf(displayText, "gO    ");
  displayText = "gO    ";
  state = TIMER_END;
  EasyBuzzer.setPin(15);
  beepVolume = EEPROM.read(EEPROM_BEEP_ADDR);
  if (beepVolume < 0 || beepVolume > 2){
    beepVolume = 0;
  }

  if ( activeOption->getCountDirectionUp()){ Serial.println("Up");}
  else { Serial.println("Down");}

}

void TimerClock::loop() {
  char textToDisplay[7];
  if ( (millis()-startTimeMs) >= activeSecond*1000 ) {
    switch (state) {
    case PRECOUNTDOWN :
      beepAtTheEnd();
      if (activeSecond >= 10){
        startClock();
      }
      else {
        sprintf(textToDisplay,"    %2d", 10-activeSecond);
        activeSecond++; 
      }
      displayText = String(textToDisplay);
      displayLed.displayCharArray(displayText, false);
      break;
    case RUN_UP:
      beepAtTheEnd();
      if (activeSecond == 0){
        if ( activeOption->getNrOfRounds() == 0){
          sprintf(textToDisplay,"%2s0000", activeOption->getDisplayName().c_str());
        }
        else{
          sprintf(textToDisplay," 10000");
        }
        activeSecond++;
      }
      else if (roundsLeft() > 0){
        if ( activeOption->getNrOfRounds() > 0){
          sprintf(textToDisplay,"%2d%02d%02d", roundsIn(), secondThisInterval()/60, secondThisInterval()%60); 
        }
        else {
          if (((activeSecond)/60) < 60) { //first hour, show "UP"
            sprintf(textToDisplay,"%2s%02d%02d", activeOption->getDisplayName().c_str(), secondThisInterval()/60, secondThisInterval()%60);
          }
          else { //passed the hour
            sprintf(textToDisplay,"%02d%02d%02d", secondThisInterval()/3600, (secondThisInterval()/60)%60, secondThisInterval()%60);
          }
        } 
        activeSecond++;
      }
      else {
        state = TIMER_END;
      }
      displayLed.displayCharArray(String(textToDisplay), true);
      break;
    case RUN_DOWN:
      beepAtTheEnd();
      if (roundsLeft() > 0){
        if (activeOption->getStartTime2() > 0) {
          sprintf(textToDisplay,"%2d%02d%02d", intervalsLeft(), secondsLeftThisInterval()/60, secondsLeftThisInterval()%60);
        }
        else if ( activeOption->getNrOfRounds() > 0){
          sprintf(textToDisplay,"%2d%02d%02d", roundsLeft(), secondsLeftThisInterval()/60, secondsLeftThisInterval()%60);
        }
        else {
          sprintf(textToDisplay,"%2s%02d%02d", activeOption->getDisplayName().c_str(), secondsLeftThisInterval()/60, secondsLeftThisInterval()%60);
        }
        activeSecond++;
      }
      else { //roundsLeft == 0
        if (activeOption->getStartTime2() > 0 || activeOption->getNrOfRounds() > 0){
          sprintf(textToDisplay," 00000");
        }
        else {
          sprintf(textToDisplay,"%2s0000", activeOption->getDisplayName().c_str());
        }
        state = TIMER_END;
      }
      displayText = String(textToDisplay);
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
}
