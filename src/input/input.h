#ifdef _WIN32
#include "mswindows/keycodes.h"
#else
#include "linux/keycodes.h"
#endif

int g_command_throttle = 0;
int g_command_brake = 0;
int g_command_handbrake = 0;
int g_command_turn_left = 0;
int g_command_turn_right = 0;

int g_command_quit = 0;

int g_command_reverse = 0;

int g_command_camera_switch_mode = 0;
int g_command_camera_rotate_left = 0;
int g_command_camera_rotate_right = 0;
int g_command_camera_zoom_in = 0;
int g_command_camera_zoom_out = 0;

void handle_key_event(unsigned int key, int press)
{
  switch(key)
  {
    case KEY_ESCAPE: g_command_quit = press; break;
    case KEY_ARROW_UP: g_command_throttle = press; break;
    case KEY_ARROW_DOWN: g_command_brake = press; break;
    case KEY_SPACE: g_command_handbrake = press; break;
    case KEY_ARROW_LEFT: g_command_turn_left = press; break;
    case KEY_ARROW_RIGHT: g_command_turn_right = press; break;
    case KEY_R: g_command_reverse = press; break;
    case KEY_C: g_command_camera_switch_mode = press; break;
    case KEY_N: g_command_camera_rotate_left = press; break;
    case KEY_M: g_command_camera_rotate_right = press; break;
    case KEY_PLUS: g_command_camera_zoom_in = press; break;
    case KEY_MINUS: g_command_camera_zoom_out = press; break;
    default: break;
  }
}
