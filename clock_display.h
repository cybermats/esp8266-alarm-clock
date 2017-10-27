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

/*

   --      a
  |  |    f b
   --      g
  |  |    e c
   -- .    d .

  abcd efg.
A 1110 1100 0xec
B 1111 1110 0xfe
C 1001 1100 0x9c
D 0111 1010 0x7a
E 1001 1110 0x9e
F 0111 0001 0x71 *
G 1011 1100 0xbc
H 0110 1110 0x6e
I 0110 0000 0x60
J 0111 1000 0x78
K 0000 0000 0x00
L 0001 1100 0x1c
M 0000 0000 0x00
N 0101 0100 0x54 *
O 0101 1100 0x5c *
P 1100 1110 0xce
Q 0000 0000 0x00
R 0000 1010 0x0a
S 1011 0010 0xb2
T 0001 1110 0x1e
U 0111 1100 0x7c
V 0000 0000 0x00
W 0000 0000 0x00
X 0000 0000 0x00
Y 0110 0110 0x66
Z 0000 0000 0x00

 */

constexpr uint8_t chartable[] = {
  0x00, // a
  0x00, // b
  0x00, // c
  0x00, // d
  0x00, // e
  0x71, // f
  0x00, // g
  0x00, // h
  0x00, // i
  0x00, // j
  0x00, // k
  0x00, // l
  0x00, // m
  0x54, // n
  0x5c, // o
  0x00, // p
  0x00, // q
  0x00, // r
  0x00, // s
  0x00, // t
  0x00, // u
  0x00, // v
  0x00, // w
  0x00, // x
  0x00, // y
  0x00  // z
};



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
  {}
  byte begin(uint8_t sda, uint8_t scl, uint8_t addr)
  {
    _i2c_addr = addr;
    Wire.begin(sda, scl);

    writeCommand(KT16K33_CMD_OSCILLATOR | KT16K33_CMD_ON); // Turn on oscillator
    writeCommand(KT16K33_CMD_DISPLAY | KT16K33_CMD_OFF); // Turn on display
//    setBrightness(1);
  }

  byte setBrightness(uint8_t b)
  {
    if (b > 15) b = 15;
    return writeCommand(KT16K33_CMD_BRIGHTNESS | b);
  }

  void setBlink(bool state) {
    writeCommand(KT16K33_CMD_DISPLAY | (state ? KT16K33_CMD_DISPLAY_BLINK_1HZ : KT16K33_CMD_DISPLAY_BLINK_OFF));
  }

  void setDisplay(bool state) {
    writeCommand(KT16K33_CMD_DISPLAY | (state ? KT16K33_CMD_ON : KT16K33_CMD_OFF)); // Turn on display
   
  }

  void printTime(int8_t hour, int8_t minute, bool colon)
  {
    _displaybuffer[0] = 0;
    _displaybuffer[1] = 0;
    _displaybuffer[2] = 0;
    _displaybuffer[3] = 0;
    if (hour >= 0) {
      if (hour >= 10) {
        _displaybuffer[0] = numbertable[hour/10];
      }
      _displaybuffer[1] = numbertable[hour % 10];
    }
    _displaybuffer[2] = colon ? 0x2 : 0;
    if (minute >= 0) {
      _displaybuffer[3] = numbertable[minute / 10];
      _displaybuffer[4] = numbertable[minute % 10];
    }
  }

  void addDot(int8_t pos) {
    _displaybuffer[0] &= ~(1 << 7);
    _displaybuffer[1] &= ~(1 << 7);
    _displaybuffer[2] &= ~(1 << 7);
    _displaybuffer[3] &= ~(1 << 7);

    
    if (pos) {
      switch(pos) {
        case 1:
          _displaybuffer[0] |= 1 << 7;
          break;
        case 2:
          _displaybuffer[1] |= 1 << 7;
          break;
        case 3:
          _displaybuffer[3] |= 1 << 7;
          break;
        case 4:
          _displaybuffer[4] |= 1 << 7;
          break;
      }
    }
  }

  void printText(String msg) {
    auto len = msg.length();
    if (len > 4) len = 4;
    uint8_t buffer[] = {0, 0, 0, 0};
    for(int i = 0; i < len; ++i) {
      char c = msg.charAt(i);
      uint8_t idx = c - 'a';
      buffer[i] = chartable[idx];
    }
    Serial.print("Buffer: [");
    Serial.print(buffer[0], HEX);
    Serial.print(", ");
    Serial.print(buffer[1], HEX);
    Serial.print(", ");
    Serial.print(buffer[2], HEX);
    Serial.print(", ");
    Serial.print(buffer[3], HEX);
    Serial.println("]");
    _displaybuffer[0] = buffer[0];
    _displaybuffer[1] = buffer[1];
    _displaybuffer[2] = 0;
    _displaybuffer[3] = buffer[2];
    _displaybuffer[4] = buffer[3];
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
};

