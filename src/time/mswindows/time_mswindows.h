#include <windows.h>

int _time_get_milliseconds_fraction()
{
  SYSTEMTIME time;
  GetSystemTime(&time);

  return time.wMilliseconds;
}
