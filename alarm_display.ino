#include <ESP8266WiFi.h>
#include <Encoder.h>

#include "clock_display.h"
#include "wifi_clock.h"

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
//ClickEncoder knob(0, 2, 14, 4);
Encoder knob(ROTARY_A, ROTARY_B);


void setup() {
  Serial.begin(115200);
  Serial.println("Clock starting up...");
  // put your setup code here, to run once:
  if(display.init(4, 5, 0x70)) {
    Serial.println("Error starting display.");
  } 
  else {
    Serial.println("Display started.");
  }
  Serial.println("Wifi connecting...");
  WiFi.begin(SSID_NAME, SSID_PASS);

  while ( WiFi.status() != WL_CONNECTED) {
    delay( 500 );
    Serial.print(".");
  }
  Serial.println("Wifi connected.");
  pinMode(ROTARY_BUTT, INPUT);
  pinMode(LED_PORT, OUTPUT);

}
long oldPosition = -999;
int oldButtonState = false;

void loop() {
  if (clock.tick()) {
    display.setTime(clock.getHours(), clock.getMinutes(), clock.getColon());
    display.writeDisplay();
  }

  long newPosition = knob.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.print("Rotary: ");
    Serial.println(newPosition);  
  }

  int newButtonState = digitalRead(ROTARY_BUTT);
  if (newButtonState != oldButtonState) {
    oldButtonState = newButtonState;
    Serial.print("Button: ");
    Serial.println(oldButtonState);
    digitalWrite(LED_PORT, oldButtonState ? HIGH : LOW);
  }

  

}
