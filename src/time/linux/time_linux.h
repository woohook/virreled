#include <time.h>

int _time_get_milliseconds_fraction()
{
  struct timespec now = {0,0};

  clock_gettime(CLOCK_MONOTONIC, &now);

  int millies = now.tv_nsec / 1000000;

  return millies;
}
