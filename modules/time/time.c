// SPDX-License-Identifier: GPL-3.0-or-later

#include <time.h>

#include "virreled/time.h"

int g_timeMillis = -1;

int time_getIntervalMilliseconds()
{
  struct timespec ts;

  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
  {
    return -1;
  }

  int timeMillis = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);

  if(g_timeMillis == -1)
  {
    g_timeMillis = timeMillis;
    return 0;
  }

  int intervalMillis = timeMillis - g_timeMillis;
  g_timeMillis       = timeMillis;

  return intervalMillis;
}
