#ifndef BUTTONS_H
#define BUTTONS_H

#include "MainMenu.h"

class Button { 
private:
  const byte pin;
  int state;
  bool actionSent;
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
  MainMenu &menu;

public:
	PowerStartControlButton(byte attachTo, MainMenu &menuAttach);

protected:
  void shortClick();
  void longClick();
};

class MenuControlButton: public Button {
private:
  MainMenu &menu;
  
public:
	MenuControlButton(byte attachTo, MainMenu &menuAttach);

protected:
  void shortClick();
  void longClick();
};

class MinusButton: public Button {
private:
  MainMenu &menu;
  
public:
	MinusButton(byte attachTo, MainMenu &menuAttach);

protected:
  void shortClick();
  void longClick();
};

class PlusButton: public Button {
private:
  MainMenu &menu;
  
public:
	PlusButton(byte attachTo, MainMenu &menuAttach);

protected:
  void shortClick();
  void longClick();
};

#endif