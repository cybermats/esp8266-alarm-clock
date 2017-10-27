
class AlarmClock
{
public:
  AlarmClock(int interval = 5)
  : _hours(0)
  , _minutes(0)
  , _interval(interval)
  {}

  void begin(uint8_t hours, uint8_t minutes) {
    _hours = hours;
    _minutes = minutes;
  }

  uint8_t getHours() const {
    return _hours;
  }

  uint8_t getMinutes() const {
    return _minutes;
  }

  String toString() const {
    String output = "";
    output += _hours;
    output += ":";
    if (_minutes < 10) output += "0";
    output += _minutes;
    return output;
  }

  void update(int8_t delta) {
    const int8_t timespan = delta * _interval;
    int8_t minutes = _minutes + timespan;
    int8_t hours = _hours;
    while(minutes < 0) {
      --hours;
      minutes += 60;
    }
    while(minutes >= 60) {
      ++hours;
      minutes -= 60;
    }
    while(hours < 0) {
      hours += 24;
    }
    while(hours >= 24) {
      hours -= 24;
    }
    _hours = hours;
    _minutes = minutes;
  }

  
private:
  uint8_t _hours;
  uint8_t _minutes;
  uint8_t _interval;
};

