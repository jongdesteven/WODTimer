#include "buttons.h"

Button::Button(byte attachTo) :
  pin(attachTo)
{
}

void Button::setup() {
  pinMode(pin, INPUT_PULLUP);
  state = HIGH;
}

void Button::loop() {
  int prevState = state;
  state = digitalRead(pin);
  if (prevState == HIGH && state == LOW) {
    buttonDownMs = millis();
  }
  else if (prevState == LOW && state == HIGH) {
    if (millis() - buttonDownMs < 50) {
    // ignore this for debounce
    }
    else if (millis() - buttonDownMs < 250) {
      shortClick();
    }
    else  {
      longClick();
    }
  }
}

PowerStartControlButton::PowerStartControlButton(byte attachTo, TimerMenu &menuAttach) :
  Button(attachTo),
  menu(menuAttach)
{
}
void PowerStartControlButton::shortClick() {
  // Start Timer
  menu.startTheTimer();
}
void PowerStartControlButton::longClick() {
  // Wake from/Go To sleep
}

MenuControlButton::MenuControlButton(byte attachTo, TimerMenu &menuAttach) :
  Button(attachTo),
  menu(menuAttach)
{
}
void MenuControlButton::shortClick() {
  //Next changeInterval
  menu.advanceMenu();
}
void MenuControlButton::longClick() {
  //Do Nothing
}

MinusButton::MinusButton(byte attachTo, TimerMenu &menuAttach) :
  Button(attachTo),
  menu(menuAttach)
{
}
void MinusButton::shortClick() {
  menu.decrementOption();
}
void MinusButton::longClick() {
  // Do Nothing
}

PlusButton::PlusButton(byte attachTo, TimerMenu &menuAttach) :
  Button(attachTo),
  menu(menuAttach)
{
}
void PlusButton::shortClick() {
  menu.incrementOption();
}
void PlusButton::longClick() {
  // Do nothing
}