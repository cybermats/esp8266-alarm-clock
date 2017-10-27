#pragma once


enum AlarmType
{
  OFF = 0,
  ONCE = 1,
  WEEKDAY = 2,
  ON = 3,
};

class AlarmState
{
public:
  void setState(AlarmType state) {
    _state = state;
  }

  AlarmType getState() const {
    return _state;
  }

  const char* toString() const {
    switch(_state) {
      case OFF: return "off";
      case ONCE: return "once";
      case WEEKDAY: return "wday";
      default:
      case ON: return "on";
    }
  }

  void update(int8_t delta) {
    auto state = _state + delta;
    while (state < 0) state += 4;
    state = state % 4;
    _state = static_cast<AlarmType>(state);
    
  }


private:
  AlarmType _state;
};

