#include <ESP8266WiFi.h>
#include <Encoder.h>
#include <Bounce2.h>
#include <PubSubClient.h>
#include <EEPROM.h>

#include "clock_display.h"
#include "wifi_clock.h"
#include "alarm_clock.h"
#include "time_func.h"
#include "alarm_signal.h"


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
AlarmClock alarm(5);
AlarmClock initialAlarm(1);
AlarmSignal alarmSignal;
bool alarmOn;
Encoder knob(ROTARY_A, ROTARY_B);
Bounce button;
WiFiClient wifiClient;
PubSubClient ps_client(wifiClient);


void reconnect() {
  while (!ps_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (ps_client.connect("ESP8266Client")) {
      Serial.println("connected");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(ps_client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void writeAlarm(byte hour, byte minute, byte initMinutes, byte alarmOn) {
  EEPROM.begin(4);
  EEPROM.write(0, hour);
  EEPROM.write(1, minute);
  EEPROM.write(2, initMinutes);
  EEPROM.write(3, alarmOn);
  EEPROM.end();
}

void readAlarm(byte& hour, byte& minute, byte& initMinutes, byte& alarmOn) {
  EEPROM.begin(4);
  hour = EEPROM.read(0);
  minute = EEPROM.read(1);
  initMinutes = EEPROM.read(2);
  alarmOn = EEPROM.read(3);
  EEPROM.end();
}

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
  Serial.println("done");
  Serial.print("Initializing LEDs and Buttons...");
  pinMode(LED_PORT, OUTPUT);
  pinMode(ROTARY_BUTT, INPUT_PULLUP);
  button.attach(ROTARY_BUTT);
  button.interval(5);
  Serial.println("done");
  Serial.print("Initializing MQTT...");
  ps_client.setServer("ubuntuserver", 1883);
  Serial.println("done");
  Serial.print("Initialize from EEPROM...");
  byte hours, minutes;
  byte initMinutes = 0;
  byte alarmOnRead = true;
  readAlarm(hours, minutes, initMinutes, alarmOnRead);
  alarm.setHours(hours);
  alarm.setMinutes(minutes);
  initialAlarm.setMinutes(initMinutes);
  alarmOn = alarmOnRead;
  alarmSignal.setAlarm(hours, minutes, initMinutes, alarmOn);
  
  Serial.println("done");
  Serial.print("Getting time...");
  clock.tick();
  Serial.println("done");
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

void sendMQTTMessage(String message) {
  reconnect();
  ps_client.publish("home-assistant/bedroom/wake_up_time", message.c_str());
}

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
      display.addDot(alarmOn ? 4 : 0);
    }
    break;
  case ONOFF:
    if (knobDiff) {
      alarmUpdateTime = now_millis;
      if (knobDiff%2) {
        alarmOn = !alarmOn;
        display.printText(alarmOn ? "on" : "off");
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
    auto sinceChange = now_millis - alarmUpdateTime;
    if (sinceChange > 5000) {
      Serial.println("Switching back.");
      writeAlarm(alarm.getHours(), alarm.getMinutes(), initialAlarm.getMinutes(), alarmOn);
      alarmSignal.setAlarm(alarm.getHours(), alarm.getMinutes(), initialAlarm.getMinutes(), alarmOn, now_epoch);
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
        display.printText(alarmOn ? "on" : "off");
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
        display.printText(alarmOn ? "on" : "off");
        break;
    }
    display.setBlink(true);
    display.addDot(0);
  }
  display.writeDisplay();
}
