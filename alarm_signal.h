#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include "time_func.h"
#include "alarm_state.h"

class AlarmSignal
{
public:
  AlarmSignal(const char* initial_topic, const char* actual_topic)
  : _wifiClient()
  , _ps_client(_wifiClient)
  , _initial_topic(initial_topic)
  , _actual_topic(actual_topic)
  , _alarmTriggerTime(0)
  , _initialTriggerTime()
  {}

  void begin(const char* mqtt_server, uint16_t mqtt_port = 1883) 
  {
    EEPROM.begin(5);
    _hours = EEPROM.read(0);
    _minutes = EEPROM.read(1);
    _initial = EEPROM.read(2);
    _active = static_cast<AlarmType>(EEPROM.read(3));
    _triggered = EEPROM.read(5);
    EEPROM.end();
    Serial.print("Connecting to MQTT server: ");
    Serial.print(mqtt_server);
    Serial.print(":");
    Serial.println(mqtt_port);
    _ps_client.setServer(mqtt_server, mqtt_port);
    connectMQTT();
  }

  void setAlarm(uint8_t hours, uint8_t minutes, uint8_t initial, AlarmType active, uint32_t now)
  {
    _hours = hours;
    _minutes = minutes;
    _initial = initial;
    _active = active;

    EEPROM.begin(5);
    EEPROM.write(0, _hours);
    EEPROM.write(1, _minutes);
    EEPROM.write(2, _initial);
    EEPROM.write(3, _active);
    EEPROM.write(4, false);
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
    // Test if this is the first run, which means we haven't calculated
    // the trigger times. Let's do that.
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
      sendMQTTMessage(_initial_topic, String(_initial));
    }
    
    if (_alarmTriggerTime < now) {
      _alarmTriggerTime = getNextAlarmTime(now, _hours, _minutes);
      Serial.print("Alarm triggered. Next alarm at: ");
      Serial.println(formatTime(_alarmTriggerTime));
      sendMQTTMessage(_actual_topic, "alarm");
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

  AlarmType getAlarmState() const {
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

  void sendMQTTMessage(const char* topic, String message) {
    connectMQTT();
    _ps_client.publish(topic, message.c_str());
  }




  WiFiClient _wifiClient;
  PubSubClient _ps_client;
  const char* _initial_topic;
  const char* _actual_topic;
  uint32_t _alarmTriggerTime;
  uint32_t _initialTriggerTime;
  AlarmType _active;
  uint8_t _hours;
  uint8_t _minutes;
  uint8_t _initial;
  bool _triggered;
};

