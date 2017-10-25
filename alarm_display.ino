#include <ESP8266WiFi.h>
#include <Encoder.h>
#include <Bounce2.h>

#include "clock_display.h"
#include "wifi_clock.h"
#include "alarm_clock.h"


/* 
 *  secret.h contains defines for ssid and password
 *  #define SSID_NAME "your ssid"
 *  #define SSID_PASS "your password"
 */
#include "secret.h"

#define ROTARY_A 0
#define ROTARY_B 2
#define ROTARY_BUTT 14
#define LED_PORT 13

ClockDisplay display;
WifiClock clock;
AlarmClock alarm;
Encoder knob(ROTARY_A, ROTARY_B);
Bounce button;

void setup() {
  Serial.begin(115200);
  Serial.println("Clock starting up...");
  // put your setup code here, to run once:
  if(display.begin(4, 5, 0x70)) {
    Serial.println("Error starting display.");
  } 
  display.setBlink(false);
  Serial.print("Wifi connecting... ");
  WiFi.begin(SSID_NAME, SSID_PASS);

  while ( WiFi.status() != WL_CONNECTED) {
    delay( 500 );
    Serial.print(".");
  }
  Serial.println();
  
  Serial.println("Wifi connected.");
  Serial.println("Initializing LEDs and Buttons");
  pinMode(LED_PORT, OUTPUT);
  pinMode(ROTARY_BUTT, INPUT_PULLUP);
  button.attach(ROTARY_BUTT);
  button.interval(5);
  
  Serial.println("Clock started.");

}

int clockState = 1;
uint32_t oldPosition;
uint32_t alarmUpdateTime;

void loop() {
  auto newRawPosition = knob.read();
  auto newPosition = newRawPosition >> 2;
  int8_t knobDiff = newPosition - oldPosition;
  if (knobDiff) {
    oldPosition = newPosition;
  }
  
  if (clockState) {
    if (clock.tick()) {
      display.setTime(clock.getHours(), clock.getMinutes(), clock.getColon());
      display.writeDisplay();
    }
  }
  else {
    if (knobDiff) {
      alarmUpdateTime = millis();
      alarm.update(knobDiff);
      display.setTime(alarm.getHours(), alarm.getMinutes(), true);
      display.writeDisplay();
    }
    auto sinceChange = millis() - alarmUpdateTime;
    if (sinceChange > 5000) {
      Serial.println("Switching back.");
      clockState = true;
      display.setBlink(false);
    }
  }

  button.update();

  if (button.fell()) {
    clockState = !clockState;
    digitalWrite(LED_PORT, clockState);
    display.setBlink(clockState);
    if (clockState) {
      display.setBlink(false);
      display.setTime(clock.getHours(), clock.getMinutes(), clock.getColon());
      display.writeDisplay();
    }
    else {
      alarmUpdateTime = millis();
      display.setBlink(true);
      display.setTime(alarm.getHours(), alarm.getMinutes(), true);
      display.writeDisplay();
    }
  }
}
