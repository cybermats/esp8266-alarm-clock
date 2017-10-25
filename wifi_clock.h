#include <NTPClient.h>
#include <WiFiUdp.h>

class WifiClock
{
public:

  WifiClock()
  : _ntpUDP()
  , _timeClient(_ntpUDP)
  {
    _timeClient.begin();
    _timeClient.update();
    _lastSeconds = _timeClient.getSeconds();
  }

  bool tick()
  {
    _timeClient.update();
    auto secs = _timeClient.getSeconds();
    if (secs != _lastSeconds) {
      _lastSeconds = secs;
      return true;
    }
    return false;
  }

  byte getHours() {
    return _timeClient.getHours();
  }

  byte getMinutes() {
    return _timeClient.getMinutes();
  }

  byte getSeconds() {
    return _timeClient.getSeconds();
  }

  bool getColon() {
    return _timeClient.getSeconds() % 2 == 0;
  }

private:
  WiFiUDP _ntpUDP;
  NTPClient _timeClient;

  int _lastSeconds;
};

