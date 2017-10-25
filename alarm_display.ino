#include <ESP8266WiFi.h>

#include "clock_display.h"
#include "wifi_clock.h"

/* 
 *  secret.h contains defines for ssid and password
 *  #define SSID_NAME "your ssid"
 *  #define SSID_PASS "your password"
 */
#include "secret.h"

ClockDisplay display;
WifiClock clock;

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
}

void loop() {
  if (clock.tick()) {
    display.setTime(clock.getHours(), clock.getMinutes(), clock.getColon());
    display.writeDisplay();
  }
  

}
