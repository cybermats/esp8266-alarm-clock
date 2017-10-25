#include <Wire.h>

#define KT16K33_CMD_BRIGHTNESS 0xE0
#define KT16K33_CMD_OSCILLATOR 0x20
#define KT16K33_CMD_OFF 0x00
#define KT16K33_CMD_ON 0x01
#define KT16K33_CMD_DISPLAY 0x80
#define KT16K33_CMD_DISPLAY_BLINK_OFF 0x01
#define KT16K33_CMD_DISPLAY_BLINK_2HZ 0x03
#define KT16K33_CMD_DISPLAY_BLINK_1HZ 0x05
#define KT16K33_CMD_DISPLAY_BLINK_HHZ 0x07

constexpr uint8_t numbertable[] = {
  0x3F, /* 0 */
  0x06, /* 1 */
  0x5B, /* 2 */
  0x4F, /* 3 */
  0x66, /* 4 */
  0x6D, /* 5 */
  0x7D, /* 6 */
  0x07, /* 7 */
  0x7F, /* 8 */
  0x6F, /* 9 */
  0x77, /* a */
  0x7C, /* b */
  0x39, /* C */
  0x5E, /* d */
  0x79, /* E */
  0x71, /* F */
};

class ClockDisplay
{
public:
  ClockDisplay()
  : _displaybuffer({0, 0, 0, 0, 0, 0, 0, 0})
  , _colon(false)
  {}
  byte begin(uint8_t sda, uint8_t scl, uint8_t addr)
  {
    _i2c_addr = addr;
    Wire.begin(sda, scl);

    writeCommand(KT16K33_CMD_OSCILLATOR | KT16K33_CMD_ON); // Turn on oscillator
    writeCommand(KT16K33_CMD_DISPLAY | KT16K33_CMD_ON); // Turn on display
    setBrightness(1);
  }

  byte setBrightness(uint8_t b)
  {
    if (b > 15) b = 15;
    return writeCommand(KT16K33_CMD_BRIGHTNESS | b);
  }

  void setBlink(bool state) {
    writeCommand(KT16K33_CMD_DISPLAY | (state ? KT16K33_CMD_DISPLAY_BLINK_1HZ : KT16K33_CMD_DISPLAY_BLINK_OFF));
  }

  void setHour(uint8_t hour) 
  {
    _hour = hour;
    render();
  }

  void setMinute(uint8_t minute) 
  {
    _minute = minute;
    render();
  }

  void setTime(uint8_t hour, uint8_t minute, bool state)
  {
    _hour = hour;
    _minute = minute;
    _colon = state;
    render();
  }

  void setColon(bool state)
  {
    _colon = state;
    render();
  }

  void render()
  {
    auto hour = _hour;

    _displaybuffer[0] = 0;
    if (hour >= 10) {
      _displaybuffer[0] = numbertable[hour/10];
    }
    _displaybuffer[1] = numbertable[hour % 10];
    _displaybuffer[2] = _colon ? 0x2 : 0;
    _displaybuffer[3] = numbertable[_minute / 10];
    _displaybuffer[4] = numbertable[_minute % 10];
  }

  byte writeDisplay() 
  {
    Wire.beginTransmission(_i2c_addr);
    Wire.write((uint8_t)0x00); // Start at address 00

    for (uint8_t i = 0; i < 8; ++i) 
    {
      Wire.write(_displaybuffer[i] & 0xFF);
      Wire.write(_displaybuffer[i] >> 8);
    }
    return Wire.endTransmission();
  }

private:

  byte writeCommand(uint8_t command) const
  {
    Wire.beginTransmission(_i2c_addr);
    Wire.write(command);
    return Wire.endTransmission();
  }

  uint8_t _i2c_addr;
  uint16_t _displaybuffer[8];
  uint8_t _hour;
  uint8_t _minute;
  bool _colon;
};

