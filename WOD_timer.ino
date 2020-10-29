#include <SevSeg.h>

#define BUTTON_PIN    13
#define BUZZER_PIN    3

#define STOPPEDTIMER  0
#define UPTIMER       1
#define DOWNTIMER     2
#define COUNTDOWNTIMER 3
#define RESETTIMER    4
#define EMOMTIMER     5

#define UPMENU        0
#define DOWNMENU      1
#define EMOMMENU      2

#define BUTTONLONGPRESSED  1
#define BUTTONSHORTPRESSED 2
#define BUTTONLONGRELEASED 3
#define BUTTONNOTCHANGED   4

#define BUZZERSHORTBEEP   500
#define BUZZERLONGBEEP    1000
#define BUZZERFREQUENCY   5000
#define ONESECOND         1000  //1000ms = one second

SevSeg sevseg; //Instantiate a seven segment controller object

unsigned long buttonDebounceDelay = 20;
unsigned long buttonLongPressTime = 2000; //2seconds pressed
unsigned long buttonNotPressedTime = 5000;

struct counter
{
  int timerState;
  int menuState;
  char menuText[2];
  int resetSeconds;
  int resetMinutes;
  int minutes;
  int seconds;
  unsigned long timer;
  bool menuMode;
};

typedef struct counter Counter;

void setup() {
  // put your setup code here, to run once:
  byte numDigits = 6;
  byte digitPins[] = {A0, A1, 12, 9, 8, 6};
  byte segmentPins[] = {11, 7, 4, 2, A4, 10, 5, A5}; //pin3 and 0 switched for buzzer
  bool resistorsOnSegments = false; // Use 'true' if on digit pins
  bool updateWithDelays = false;
  bool leadingZeros = true;
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  //Serial.begin(115200);

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros);
  sevseg.setBrightness(90);
}

void setMenuOption(Counter* count){
  switch (count->menuState) {
    case UPMENU:
      //do nothing
      break;
    case DOWNMENU:
      count->resetMinutes++;
      break;
    case EMOMMENU:
      count->resetMinutes++;
      break;
  }
}

void changeMenuState(Counter* count) {
  //go to next menu option
  switch (count->menuState) {
    case UPMENU:
      count->menuState = DOWNMENU;
      sprintf(count->menuText,"dn");
      count->resetSeconds = 0;
      count->resetMinutes = 1; //down needs at least 1 minute
      break;
    case DOWNMENU:
      count->menuState = EMOMMENU;
      sprintf(count->menuText,"EO");
      count->resetSeconds = 0;
      count->resetMinutes = 1; //emom needs at least 1 round
      break;
    case EMOMMENU:
      count->menuState = UPMENU;
      sprintf(count->menuText,"UP");
      count->resetSeconds = 0;
      count->resetMinutes = 0; //UP starts always at 0
      break;
  }
  count->timer = millis();
}

void changeTimerState(Counter* count) {
  switch (count->timerState) {
    case STOPPEDTIMER:    // Counter Stopped
      count->timerState = RESETTIMER;
      count->seconds = count->resetSeconds;
      count->minutes = count->resetMinutes;
      break;
    case RESETTIMER:
      //prepare for countdowntimer
      count->seconds = 11; //will show 10, because of decrement before showing
      count->minutes = 0;
      count->timerState = COUNTDOWNTIMER;
      break;
    case COUNTDOWNTIMER:    // count down
      //pick the right mode as set in the menu
      switch (count->menuState) {
        case UPMENU:
          count->timerState = UPTIMER;
          break;
        case DOWNMENU:
          count->timerState = DOWNTIMER;
          break; 
        case EMOMMENU:
          count->timerState = EMOMTIMER;
          break;  
      }
      //when changing timer state, also reset the time
      count->seconds = count->resetSeconds;
      count->minutes = count->resetMinutes;
      break;
    case DOWNTIMER:
      count->timerState = STOPPEDTIMER;
      break;
    case UPTIMER:   //counter up
      count->timerState = STOPPEDTIMER;
      break;
    case EMOMTIMER:
      count->timerState = STOPPEDTIMER;
      break;
  }
  // start the clock accurate
  count->timer = millis();
}

void updateMenu(Counter* count){
  static char displayText[6] = "";
  switch (count->menuState){
    case UPMENU: // UP menu
      sprintf(displayText,"%2s    ", count->menuText);
      break;
    case DOWNMENU: // Down menu
      sprintf(displayText,"%2s%02d%  ", count->menuText, count->resetMinutes);      
      break;
    case EMOMMENU: // EMOM menu
      sprintf(displayText,"%2s  %02d", count->menuText, count->resetMinutes);
      break;
  }
  count->seconds = count->resetSeconds;
  count->minutes = count->resetMinutes;
  sevseg.setChars(displayText);
}

void updateTimer(Counter* count) {
  static char displayText[6] = "";
  switch (count->timerState) {
    case STOPPEDTIMER:   // Counter Stopped
      //just dont update anymore.
      if (count->menuState == EMOMMENU) { 
        sprintf(displayText,"%02d00%02d", count->minutes, count->seconds); //no exclusion for the 0:00
      }
      else { 
        sprintf(displayText,"%2s%02d%02d", count->menuText, count->minutes, count->seconds); 
      }
      break;
    case RESETTIMER: // reset timer
      //time will be set to reset values
      count->seconds = count->resetSeconds; //needed here too when coming from menu
      count->minutes = count->resetMinutes;
      if (count->menuState == EMOMMENU) { 
        sprintf(displayText,"%02d01%02d", count->minutes, count->seconds);
      }
      else { 
        sprintf(displayText,"%2s%02d%02d", count->menuText, count->minutes, count->seconds); 
      }
      break;
    case COUNTDOWNTIMER:
      if (millis() >= count->timer) { // 1 second has passed
        count->timer += ONESECOND; //update for next second
        count->seconds--;

        if (count->seconds < 4 && count->seconds > 0 ) {
          tone(BUZZER_PIN, BUZZERFREQUENCY, BUZZERSHORTBEEP);
        }
        else if (count->seconds == 0) {
          tone(BUZZER_PIN, BUZZERFREQUENCY, BUZZERLONGBEEP);
        }
        else if (count->seconds < 0) {
          count->minutes--;
          count->seconds = 0; 
        }

        if (count->minutes < 0) {
          //minutes = 59; don't go lower
          changeTimerState(count);
        }
      }
      sprintf(displayText,"    %02d", count->seconds);
      break;
    case DOWNTIMER:
      if (millis() >= count->timer) { // 1 second has passed
        count->timer += ONESECOND; //update for next second
        count->seconds--;

        if (count->seconds < 0) {
          count->minutes--;
          count->seconds = 59; 
        }

        if (count->minutes < 0) {
          count->minutes = 0; //don't go lower
          count->seconds = 0;
          tone(BUZZER_PIN, BUZZERFREQUENCY, BUZZERLONGBEEP);
          changeTimerState(count); 
        }
      }
      sprintf(displayText,"%2s%02d%02d", count->menuText, count->minutes, count->seconds);
      break;
    case UPTIMER:  
      if (millis() >= count->timer) { // 1 second has passed
        count->timer += ONESECOND; //update for next second
        count->seconds++;

        if (count->seconds == 60) {
          count->minutes++;
          count->seconds = 0;
        }
    
        if (count->minutes == 60) {
          count->minutes = 0;
        }
      }
      sprintf(displayText,"%2s%02d%02d", count->menuText, count->minutes, count->seconds);
      break;
  case EMOMTIMER:
   if (millis() >= count->timer) { // 1 second has passed
      count->timer += ONESECOND; //update for next second
      count->seconds--;

      if (count->seconds < 0) {
        count->minutes--;
        count->seconds = 59;
      }

      if (count->seconds == 0) {
        tone(BUZZER_PIN, BUZZERFREQUENCY, BUZZERLONGBEEP);
        //Serial.print(millis());
      }

      if (count->minutes < 0) {
        count->minutes = 0; //don't go lower
        count->seconds = 0;
        changeTimerState(count); 
      }
    }
    sprintf(displayText,"%02d00%02d", (count->minutes+1), count->seconds); //minutes used as rounds.
    break;
  }
  sevseg.setChars(displayText);
}

int readButtons() {
  static int lastButtonState = HIGH;
  static int buttonState = HIGH;
  static bool buttonLongNotChanged = false;
  static unsigned long lastChangeTime = millis();
  int buttonReturn = BUTTONNOTCHANGED;
  int newRead = digitalRead(BUTTON_PIN);

  //set to not changed, and change only when needed.
  buttonReturn = BUTTONNOTCHANGED;
  
  if (newRead != lastButtonState) { //buttonRead has changed since last time reading button
    lastChangeTime = millis(); //reset the timer
  } 

  //check if longer than debounce delay
  if ((millis() - buttonDebounceDelay) > lastChangeTime){
    if (newRead != buttonState){
      buttonState = newRead;
      if (buttonState == HIGH && buttonLongNotChanged == false){
        //changed from low to high = short press, except when longer time not changed
        buttonReturn = BUTTONSHORTPRESSED;
      }
      else if (buttonState == HIGH && buttonLongNotChanged == true){
        //reset the longpress detected and do not report change
        buttonLongNotChanged = false;
        buttonReturn = BUTTONNOTCHANGED;
      }
      else if (buttonState == LOW && buttonLongNotChanged == true) { //can be combined with above?
        buttonLongNotChanged = false;
        buttonReturn = BUTTONNOTCHANGED;
      }
    }
    else {
      //button not changed, but meeting debounce condition
      if (buttonLongNotChanged == false){
        //first time only
        if (buttonState == LOW){
          if ((millis() - buttonLongPressTime) > lastChangeTime){
            //if button is still pressed and meeting the long press time.
            buttonLongNotChanged = true;
            buttonReturn = BUTTONLONGPRESSED;
          }
        }
        else if (buttonState == HIGH){
          if ((millis() - buttonNotPressedTime) > lastChangeTime){
            //if button is still released and meeting the long not pressed time.
            buttonLongNotChanged = true;
            buttonReturn = BUTTONLONGRELEASED;
          }
        }
      }
    }
  }
 
  //set the current status to be compared next time
  lastButtonState = newRead;
  if (buttonReturn != BUTTONNOTCHANGED){
    Serial.print("button:");
    Serial.print(buttonReturn, DEC);
    Serial.print(buttonState, DEC);
    Serial.print("\n");
  }
  
  return buttonReturn;
}

void beep(unsigned long duration) {
  static unsigned long stopTime = 0;

  if (duration != 0){
    stopTime = millis() + duration;
  }

  if ( millis() < stopTime ){
    tone(BUZZER_PIN, BUZZERFREQUENCY);
  }
  else {
    noTone(BUZZER_PIN);
  }
  
}

void loop() {

static Counter myCounter = {RESETTIMER, UPMENU, "UP", 0, 0, 0, 0, 0, false}; 
int buttonRead = readButtons();

  if (myCounter.menuMode == true){ //handle the menu.
    switch (buttonRead) {
      case BUTTONSHORTPRESSED: //short press
        setMenuOption(&myCounter);
        break;
      case BUTTONLONGPRESSED: //long press
        changeMenuState(&myCounter);
        break;
      case BUTTONLONGRELEASED: // long not pressed
        myCounter.menuMode = false; //back to timer mode
        myCounter.timerState = RESETTIMER; //come back in reset mode
        myCounter.timer = millis(); //handle everything from NOW
        break;
    } 
    updateMenu(&myCounter); 
  } 
  else { //act as timer
    switch (buttonRead) {
    case BUTTONSHORTPRESSED: 
      changeTimerState(&myCounter); 
      break;
    case BUTTONLONGPRESSED: 
      myCounter.menuMode = true; //go to menu mode
      break;
    case BUTTONLONGRELEASED: 
      break;
    } 
    updateTimer(&myCounter); //update the clock with new time 
  } 
  //beep(0); // keep calling to shutdown the beeper
  sevseg.refreshDisplay(); //must run repeatedly

}

