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

uint8_t days = 0;
uint8_t hour = 0;
uint8_t minute = 0;
bool colon = false;

void loop() {
  // put your main code here, to run repeatedly:
  timeNow = millis();
  auto diff = timeNow - timeLast;

  if (diff > 1000) {
    ++minute;
    timeLast = timeNow;
    colon = true;
  }

  if (minute >= 60) {
    ++hour;
    minute %= 60;
  }

  if (hour >= 24) {
    ++days;
  }

  if (timeNow == timeLast) {
    Serial.print("Time is: ");
    Serial.print(hour, 10);
    Serial.print(":");
    Serial.print(minute, 10);
    Serial.print(":");
    Serial.print(colon, 10);
    Serial.print(" ");
    Serial.print(diff, 10);
    Serial.println();
    display.setTime(hour, minute, colon);
    display.writeDisplay();
  }
  else if (diff > 500 && colon) {
    colon = false;
    display.setColon(colon);
    display.writeDisplay();
  }
  

}
