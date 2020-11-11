#include <Arduino.h>

#include "buttons.h"
#include "DisplayControl.h"
#include "TimerMenu.h"

DisplayControl ledDisplay(13, 14, 15, 1);
TimerMenu cfTimer(ledDisplay);
PowerStartControlButton pwrBtn(4, cfTimer);
MenuControlButton menuBtn(12, cfTimer);
MinusButton minBtn(5, cfTimer);
PlusButton plusBtn(0, cfTimer);

void setup() {
  // put your setup code here, to run once:
  pwrBtn.setup();
  menuBtn.setup();
  minBtn.setup();
  plusBtn.setup();
  cfTimer.setup();
  ledDisplay.setup();
}

void loop() {
  pwrBtn.loop();
  menuBtn.loop();
  minBtn.loop();
  plusBtn.loop();
  cfTimer.loop();
  ledDisplay.loop();

}