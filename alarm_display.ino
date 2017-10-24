#include "clock_display.h"
#include "wifi_clock.h"

ClockDisplay display;
WifiClock clock;

unsigned long timeNow = 0;
unsigned long timeLast = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Clock starting up...");
  // put your setup code here, to run once:
  if(display.init(4, 0, 0x70)) {
    Serial.println("Error starting display.");
  } 
  else {
    Serial.println("Display started.");
  }
}

void loop() {
  if (clock.tick()) {
    display.setTime(clock.getMinutes(), clock.getSeconds(), clock.getColon());
    display.writeDisplay();
  }
  

}
