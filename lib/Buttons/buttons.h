#ifndef BUTTONS_H
#define BUTTONS_H

#include "TimerMenu.h"

class Button {
  const byte pin;
  int state;
  unsigned long buttonDownMs;

  protected:
    virtual void shortClick() = 0;
    virtual void longClick() = 0;

  public:
    Button(byte attachTo) :
      pin(attachTo)
    {
    }

		void setup() {
      pinMode(pin, INPUT_PULLUP);
      state = HIGH;
    }

		void loop() {
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

};

class PowerStartControlButton: public Button {
  TimerMenu &menu;
	public:
		PowerStartControlButton(byte attachTo, TimerMenu &menuAttach) :
			Button(attachTo),
      menu(menuAttach)
		{
		}

	protected:
		void shortClick() {
    // Start Timer
    menu.startTheTimer();
		}

		void longClick() {
      // Wake from/Go To sleep
		}
};

class MenuControlButton: public Button {
  TimerMenu &menu;
  public:
    MenuControlButton(byte attachTo, TimerMenu &menuAttach) :
     Button(attachTo),
     menu(menuAttach)
    {
    }

  protected:
    void shortClick() {
      //Next changeInterval
      menu.advanceMenu();
    }

    void longClick() {
      //Do Nothing
    }
};

class MinusButton: public Button {
  TimerMenu &menu;
  public:
    MinusButton(byte attachTo, TimerMenu &menuAttach) :
     Button(attachTo),
     menu(menuAttach)
    {
    }

  protected:
    void shortClick() {
      menu.decrementOption();
    }

    void longClick() {
      // Do Nothing
    }
};

class PlusButton: public Button {
  TimerMenu &menu;
  public:
    PlusButton(byte attachTo, TimerMenu &menuAttach) :
      Button(attachTo),
      menu(menuAttach)
    {
    }

  protected:
    void shortClick() {
      menu.incrementOption();
    }

    void longClick() {
      // Do nothing
    }
};

#endif