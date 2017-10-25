
class AlarmClock
{
public:
  AlarmClock(int hours = 0, int minutes = 0, int interval = 5)
  : _hours(hours)
  , _minutes(minutes)
  , _interval(interval)
  {}

  int getHours() const {
    return _hours;
  }

  int getMinutes() const {
    return _minutes;
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

