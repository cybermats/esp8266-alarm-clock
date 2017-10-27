#pragma once

#include "time_func.h"

class AlarmSignal
{
public:
  AlarmSignal()
  : _alarmTriggerTime(0)
  , _initialTriggerTime(0)
  {}

  void setAlarm(uint8_t hours, uint8_t minutes, uint8_t initial, bool active, uint32_t now = 0)
  {
    _hours = hours;
    _minutes = minutes;
    _initial = initial;
    _active = active;
    if (now) {
      _initialTriggerTime = getNextAlarmTime(now, _hours, _minutes - (int8_t)_initial);
      _alarmTriggerTime = getNextAlarmTime(now, _hours, _minutes);
      Serial.print("Initial initialized. Next alarm at: ");
      Serial.println(formatTime(_initialTriggerTime));
      Serial.print("Alarm initialized. Next alarm at: ");
      Serial.println(formatTime(_alarmTriggerTime));
    }
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
  
private:
uint32_t _alarmTriggerTime;
uint32_t _initialTriggerTime;
bool _active;
uint8_t _hours;
uint8_t _minutes;
uint8_t _initial;
};

