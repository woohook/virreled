#ifdef __linux__
#include "linux/time_linux.h"
#endif

#ifdef _WIN32
#include "mswindows/time_mswindows.h"
#endif

int g_time_millies_last  = -1;
int g_time_frame_millies = 0;

void time_process_frame()
{
  int millies = _time_get_milliseconds_fraction();

  if(g_time_millies_last != -1)
  {
    g_time_frame_millies = millies - g_time_millies_last;

    if(g_time_frame_millies < 0)
    {
      g_time_frame_millies = g_time_frame_millies + 1000;
    }
  }

  g_time_millies_last = millies;
}

int time_get_step_duration()
{
  return g_time_frame_millies;
}
