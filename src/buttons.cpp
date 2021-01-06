#include "buttons.h"

Button::Button(byte attachTo, callBackFunction callback_shortPress) :
  pin(attachTo),
  shortPressFunction(callback_shortPress),
  longPressFunction(nullptr)
{
}

Button::Button(byte attachTo, callBackFunction callback_shortPress, callBackFunction callback_longPress) :
  pin(attachTo),
  shortPressFunction(callback_shortPress),
  longPressFunction(callback_longPress)
{
}

void Button::setup() {
  pinMode(pin, INPUT_PULLUP);
  state = HIGH;
  actionSent = false;
}

void Button::loop() {
  int prevState = state;
  state = digitalRead(pin);
  if (prevState == HIGH && state == LOW) {
    buttonDownMs = millis();
    actionSent = false;
  }
  else if (prevState == LOW && state == HIGH) {
    if (millis() - buttonDownMs < 25) {
    // ignore this for debounce
    }
    else if (millis() - buttonDownMs < 1000) {
      actionSent = true;
      shortPressFunction();
    }
    else  {
      // actionSent = true;
      if (longPressFunction != nullptr){
        longPressFunction();
      }
      // longClick();
    }
  }
  else if (state == LOW && millis()-buttonDownMs >= 1000){
    if (actionSent == false){
      if (longPressFunction != nullptr){
        longPressFunction();
      }
      actionSent = true;
    }
  }
}