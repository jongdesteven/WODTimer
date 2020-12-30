#include "buttons.h"

Button::Button(byte attachTo) :
  pin(attachTo)
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
      shortClick();
    }
    else  {
      // actionSent = true;
      // longClick();
    }
  }
  else if (state == LOW && millis()-buttonDownMs >= 1000){
    if (actionSent == false){
      longClick();
      actionSent = true;
    }
  }
}

PowerStartControlButton::PowerStartControlButton(byte attachTo, MainMenu &menuAttach) :
  Button(attachTo),
  menu(menuAttach)
{
}
void PowerStartControlButton::shortClick() {
  // Start Timer
  menu.powerAction();
}
void PowerStartControlButton::longClick() {
  // Wake from/Go To sleep
  menu.returnAction();
}

MenuControlButton::MenuControlButton(byte attachTo, MainMenu &menuAttach) :
  Button(attachTo),
  menu(menuAttach)
{
}
void MenuControlButton::shortClick() {
  //Next changeInterval
  menu.menuAction();
}
void MenuControlButton::longClick() {
  //Do Nothing
}

MinusButton::MinusButton(byte attachTo, MainMenu &menuAttach) :
  Button(attachTo),
  menu(menuAttach)
{
}
void MinusButton::shortClick() {
  menu.decrementAction();
}
void MinusButton::longClick() {
  // Do Nothing
}

PlusButton::PlusButton(byte attachTo, MainMenu &menuAttach) :
  Button(attachTo),
  menu(menuAttach)
{
}
void PlusButton::shortClick() {
  menu.incrementAction();
}
void PlusButton::longClick() {
  // Do nothing
}