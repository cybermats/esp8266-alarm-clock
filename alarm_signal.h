#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include "time_func.h"

class AlarmSignal
{
public:
  AlarmSignal()
  : _wifiClient()
  , _ps_client(_wifiClient)
  , _alarmTriggerTime(0)
  , _initialTriggerTime()
  {}

  void begin(const char* mqtt_server, uint16_t mqtt_port = 1883) 
  {
    EEPROM.begin(4);
    _hours = EEPROM.read(0);
    _minutes = EEPROM.read(1);
    _initial = EEPROM.read(2);
    _active = EEPROM.read(3);
    EEPROM.end();
    Serial.print("Connecting to MQTT server: ");
    Serial.print(mqtt_server);
    Serial.print(":");
    Serial.println(mqtt_port);
    _ps_client.setServer(mqtt_server, mqtt_port);
    connectMQTT();
  }

  void setAlarm(uint8_t hours, uint8_t minutes, uint8_t initial, bool active, uint32_t now)
  {
    _hours = hours;
    _minutes = minutes;
    _initial = initial;
    _active = active;

    EEPROM.begin(4);
    EEPROM.write(0, _hours);
    EEPROM.write(1, _minutes);
    EEPROM.write(2, _initial);
    EEPROM.write(3, _active);
    EEPROM.end();
    
    _initialTriggerTime = getNextAlarmTime(now, _hours, _minutes - (int8_t)_initial);
    _alarmTriggerTime = getNextAlarmTime(now, _hours, _minutes);
    Serial.print("Initial initialized. Next alarm at: ");
    Serial.println(formatTime(_initialTriggerTime));
    Serial.print("Alarm initialized. Next alarm at: ");
    Serial.println(formatTime(_alarmTriggerTime));
  }

  bool update(uint32_t now) const
  {
    if (_active) {
      if (_alarmTriggerTime < now || _initialTriggerTime < now) {
        return true;
      }
    }
    return false;
  }

  void process(uint32_t now) {
    if (_alarmTriggerTime == 0 || _initialTriggerTime == 0) {
      _initialTriggerTime = getNextAlarmTime(now, _hours, _minutes - (int8_t)_initial);
      _alarmTriggerTime = getNextAlarmTime(now, _hours, _minutes);
      Serial.print("Initial initialized. Next alarm at: ");
      Serial.println(formatTime(_initialTriggerTime));
      Serial.print("Alarm initialized. Next alarm at: ");
      Serial.println(formatTime(_alarmTriggerTime));
    }
    
    if (_initialTriggerTime < now && _initialTriggerTime != _alarmTriggerTime) {
      _initialTriggerTime = getNextAlarmTime(now, _hours, _minutes - (int8_t)_initial);
      Serial.print("Initial triggered. Next alarm at: ");
      Serial.println(formatTime(_initialTriggerTime));
    }
    
    if (_alarmTriggerTime < now) {
      _alarmTriggerTime = getNextAlarmTime(now, _hours, _minutes);
      Serial.print("Alarm triggered. Next alarm at: ");
      Serial.println(formatTime(_alarmTriggerTime));
    }
  }
  uint8_t getHours() const {
    return _hours;
  }

  uint8_t getMinutes() const {
    return _minutes;
  }

  uint8_t getInitial() const {
    return _initial;
  }

  bool getAlarmState() const {
    return _active;
  }
  
private:
  void connectMQTT() {
    while (!_ps_client.connected()) {
      Serial.print("Attempting MQTT connection...");
      if (_ps_client.connect("ESP8266Client")) {
        Serial.println("connected");
      }
      else {
        Serial.print("failed, rc=");
        Serial.print(_ps_client.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  }

  void sendMQTTMessage(String message) {
    connectMQTT();
    _ps_client.publish("home-assistant/bedroom/wake_up_time", message.c_str());
  }




  WiFiClient _wifiClient;
  PubSubClient _ps_client;
  uint32_t _alarmTriggerTime;
  uint32_t _initialTriggerTime;
  bool _active;
  uint8_t _hours;
  uint8_t _minutes;
  uint8_t _initial;
};

