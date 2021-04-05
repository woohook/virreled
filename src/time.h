#include <time.h>

int g_time_millies_last  = -1;
int g_time_frame_millies = 0;

void time_process_frame()
{
  struct timespec now = {0,0};

  clock_gettime(CLOCK_MONOTONIC, &now);

  int millies = now.tv_nsec / 1000000;

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
