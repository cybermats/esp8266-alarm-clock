#include <NTPClient.h>
#include <WiFiUdp.h>

class WifiClock
{
public:

  WifiClock()
  : _ntpUDP()
  , _timeClient(_ntpUDP, 2 * 60 * 60)
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

  uint8_t getHours() {
    return _timeClient.getHours();
  }

  uint8_t getMinutes() {
    return _timeClient.getMinutes();
  }

  uint8_t getSeconds() {
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

