#include <ESP8266WiFi.h>
#include <Encoder.h>
#include <Bounce2.h>
#include <EEPROM.h>

#include "clock_display.h"
#include "wifi_clock.h"
#include "alarm_clock.h"
#include "time_func.h"
#include "alarm_signal.h"
#include "alarm_state.h"

/* 
 *  secret.h contains defines for ssid and password
 *  #define SSID_NAME "your ssid"
 *  #define SSID_PASS "your password"
 */
#include "secret.h"

#define PIN_ROTARY_A 0
#define PIN_ROTARY_B 2
#define PIN_ROTARY_BUTT 14

#define MQTT_SERVER "ubuntuserver"

#define I2C_PORT 0x70

#define INITIAL_ALARM_TOPIC "cybermats/alarm-clock"
#define ACTUAL_ALARM_TOPIC  "cybermats/alarm-clock"

ClockDisplay display(I2C_PORT);
WifiClock clock;
AlarmClock alarm(5);
AlarmClock initialAlarm(1);
AlarmSignal alarmSignal(INITIAL_ALARM_TOPIC, ACTUAL_ALARM_TOPIC);
AlarmState alarmState;

Encoder knob(PIN_ROTARY_A, PIN_ROTARY_B);
Bounce button;


void setup() {
  Serial.begin(115200);
  Serial.println("Clock starting up...");
  // put your setup code here, to run once:
  display.begin(4, 5);
  display.setBlink(false);
  Serial.print("Wifi connecting... ");
  WiFi.begin(SSID_NAME, SSID_PASS);

  while ( WiFi.status() != WL_CONNECTED) {
    delay( 500 );
    Serial.print(".");
  }
  Serial.println("done");
  Serial.print("Initializing LEDs and Buttons...");
  pinMode(PIN_ROTARY_BUTT, INPUT_PULLUP);
  button.attach(PIN_ROTARY_BUTT);
  button.interval(5);
  Serial.println("done");
  Serial.print("Getting time...");
  clock.tick();
  Serial.println("done");
  Serial.println("Initialize alarm...");
  alarmSignal.begin(clock.getEpochTime(), MQTT_SERVER);
  alarmState.setState(alarmSignal.getAlarmState());
  alarm.begin(alarmSignal.getHours(), alarmSignal.getMinutes());
  initialAlarm.begin(0, alarmSignal.getInitial());
  Serial.println("Alarm initialized.");
  Serial.println("Clock started.");
  display.setDisplay(true);
}

enum ClockState {
  CLOCK,
  ONOFF,
  ALARM,
  INITIAL,
};

ClockState clockState = CLOCK;
uint32_t oldPosition;
uint32_t alarmUpdateTime;

void loop() {
  const auto now_millis = millis();

  // Handle rotary knob
  const auto newRawPosition = knob.read();
  const auto newPosition = newRawPosition >> 2;
  const int8_t knobDiff = newPosition - oldPosition;
  if (knobDiff) {
    oldPosition = newPosition;
  }

  switch(clockState) {
  case CLOCK:
    if (clock.tick()) {
      display.printTime(clock.getHours(), clock.getMinutes(), clock.getColon());
      display.addDot(alarmSignal.hasAlarmSoon(clock.getEpochTime()) ? 4 : 0);
    }
    break;
  case ONOFF:
    if (knobDiff) {
      alarmUpdateTime = now_millis;
      if (knobDiff%2) {
        alarmState.update(knobDiff);
        display.printText(alarmState.toString());
      }
    }
    break;
  case ALARM:
    if (knobDiff) {
      alarmUpdateTime = now_millis;
      alarm.update(knobDiff);
      display.printTime(alarm.getHours(), alarm.getMinutes(), true);
    }
    break;
  case INITIAL:
    if (knobDiff) {
      alarmUpdateTime = now_millis;
      initialAlarm.update(knobDiff);
      display.printTime(-1, initialAlarm.getMinutes(), false);
    }
    break;
  default:
    break;
  }

  const auto now_epoch = clock.getEpochTime();

  if (clockState != CLOCK) {
    const auto sinceChange = now_millis - alarmUpdateTime;
    if (sinceChange > 5000) {
      Serial.println("Switching back.");
      alarmSignal.setAlarm(alarm.getHours(), alarm.getMinutes(), initialAlarm.getMinutes(), alarmState.getState(), now_epoch);
      clockState = CLOCK;
      display.setBlink(false);
    }
  }

  if (alarmSignal.update(now_epoch)) {
    alarmSignal.process(now_epoch);
  }

  // Handle the button
  button.update();
  if (button.fell()) {
    alarmUpdateTime = now_millis;
    switch(clockState) {
      case CLOCK:
        Serial.println("ONOFF");
        clockState = ONOFF;
        display.printText(alarmState.toString());
        break;
      case ONOFF:
        Serial.println("ALARM");
        clockState = ALARM;
        display.printTime(alarm.getHours(), alarm.getMinutes(), true);
        break;
      case ALARM:
        Serial.println("INITIAL");
        clockState = INITIAL;
        display.printTime(-1, initialAlarm.getMinutes(), false);
        break;
      case INITIAL:
        Serial.println("ONOFF");
        clockState = ONOFF;
        display.printText(alarmState.toString());
        break;
    }
    display.setBlink(true);
    display.addDot(0);
  }
  display.writeDisplay();

  delay(50);
}
