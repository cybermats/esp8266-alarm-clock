
class AlarmClock
{
public:
  AlarmClock(int interval = 5)
  : _hours(0)
  , _minutes(0)
  , _interval(interval)
  {}

  void setHours(byte hours) {
    _hours = hours;
  }

  void setMinutes(byte minutes) {
    _minutes = minutes;
  }

  int getHours() const {
    return _hours;
  }

  int getMinutes() const {
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
    int timespan = delta * _interval;
    int minutes = _minutes + timespan;
    int hours = _hours;
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
  int _hours;
  int _minutes;
  int _interval;
};

