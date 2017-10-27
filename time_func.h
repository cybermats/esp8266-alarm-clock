#pragma once

uint8_t getDay(uint32_t t)
{
  return ((t / 86400L) + 4) % 7;
}

uint8_t getHours(uint32_t t)
{
  return (t % 86400L) / 3600;
}

uint8_t getMinutes(uint32_t t)
{
  return (t % 3600) / 60;
}

uint8_t getSeconds(uint32_t t)
{
  return t % 60;
}

uint32_t getMidnight(uint32_t t)
{
  return t - (t % 86400L);
}

uint32_t addDays(uint32_t t, int8_t days)
{
  return t + (days * 86400L);
}

uint32_t addTime(uint32_t t, int8_t hours, int8_t minutes)
{
  return t + (hours * 3600) + (minutes * 60);
}

String formatTime(uint32_t t) {
  auto day = getDay(t);
  String dayStr = "";
  switch(day) {
    case 0: dayStr = "Sunday"; break;
    case 1: dayStr = "Monday"; break;
    case 2: dayStr = "Tuesday"; break;
    case 3: dayStr = "Wedday"; break;
    case 4: dayStr = "Thursday"; break;
    case 5: dayStr = "Friday"; break;
    case 6: dayStr = "Saturday"; break;
  }

  auto hours = getHours(t);
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

  auto minutes = getMinutes(t);
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  auto seconds = getSeconds(t);
  String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

  return dayStr + " " + hoursStr + ":" + minuteStr + ":" + secondStr;
}

uint32_t getNextAlarmTime(uint32_t now, uint8_t hours, uint8_t minutes)
{
  auto alarm = addTime(getMidnight(now), hours, minutes);
  while (alarm < now)
  {
    alarm = addDays(alarm, 1);
  }
  return alarm;
}

