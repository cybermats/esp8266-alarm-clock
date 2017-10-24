

class WifiClock
{
public:

  WifiClock()
  : _last(millis())
  {}

  bool tick()
  {
    unsigned long now = millis();
    auto diff = now - _last;

    if (diff > 1000) {
      _seconds += diff / 1000;
      _last = now;
      _colon = true;
    }

    if (_seconds >= 60) {
      _minutes += (_seconds / 60);
      _seconds %= 60;
    }
    
    if (_minutes >= 60) {
      _hours += (_minutes / 60);
      _minutes %= 60;
    }
    
    if (_hours >= 24) {
      _days += (_hours / 24);
      _hours %= 24;
    }

    if (now == _last) {
      return true;
    }
    else if (diff > 500 && _colon) {
      _colon = false;
      return true;
    }
    return false;
  }

  byte getDays() const {
    return _days;
  }

  byte getHours() const {
    return _hours;
  }

  byte getMinutes() const {
    return _minutes;
  }

  byte getSeconds() const {
    return _seconds;
  }

  bool getColon() const {
    return _colon;
  }

private:
  byte _days;
  byte _hours;
  byte _minutes;
  byte _seconds;
  bool _colon;
  unsigned long _last;
};

