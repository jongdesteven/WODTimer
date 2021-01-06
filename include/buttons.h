#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

class Button { 
private:
  const byte pin;
  int state;
  bool actionSent;
  unsigned long buttonDownMs;
  typedef void (*callBackFunction)();
  callBackFunction shortPressFunction;
  callBackFunction longPressFunction;

  public:
    Button(byte attachTo, callBackFunction callback_shortPress);
    Button(byte attachTo, callBackFunction callback_shortPress, callBackFunction callback_longPress);
		void setup();
		void loop();
};

#endif