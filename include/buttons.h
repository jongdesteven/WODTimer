#ifndef BUTTONS_H
#define BUTTONS_H

#include "TimerMenu.h"

class Button { 
private:
  const byte pin;
  int state;
  unsigned long buttonDownMs;

  protected:
    virtual void shortClick() = 0;
    virtual void longClick() = 0;

  public:
    Button(byte attachTo);
		void setup();
		void loop();
};

class PowerStartControlButton: public Button {
private:
  TimerMenu &menu;

public:
	PowerStartControlButton(byte attachTo, TimerMenu &menuAttach);

protected:
  void shortClick();
  void longClick();
};

class MenuControlButton: public Button {
private:
  TimerMenu &menu;
  
public:
	MenuControlButton(byte attachTo, TimerMenu &menuAttach);

protected:
  void shortClick();
  void longClick();
};

class MinusButton: public Button {
private:
  TimerMenu &menu;
  
public:
	MinusButton(byte attachTo, TimerMenu &menuAttach);

protected:
  void shortClick();
  void longClick();
};

class PlusButton: public Button {
private:
  TimerMenu &menu;
  
public:
	PlusButton(byte attachTo, TimerMenu &menuAttach);

protected:
  void shortClick();
  void longClick();
};

#endif