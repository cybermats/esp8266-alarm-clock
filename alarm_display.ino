#include <ESP8266WiFi.h>
#include <Encoder.h>
#include <Bounce2.h>

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
Encoder knob(ROTARY_A, ROTARY_B);
Bounce button;

void setup() {
  Serial.begin(115200);
  Serial.println("Clock starting up...");
  // put your setup code here, to run once:
  if(display.init(4, 5, 0x70)) {
    Serial.println("Error starting display.");
  } 
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

int ledState = 0;
uint32_t oldPosition;

void loop() {
  if (clock.tick()) {
    display.setTime(clock.getHours(), clock.getMinutes(), clock.getColon());
    display.writeDisplay();
  }

  auto newRawPosition = knob.read();
  auto newPosition = newRawPosition >> 2;
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.print("Rotary: ");
    Serial.println(newPosition);  
  }

  button.update();

  if (button.fell()) {
    ledState = !ledState;
    digitalWrite(LED_PORT, ledState);
  }
}
