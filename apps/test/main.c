//gcc -o window.so --shared ../../modules/x11win/x11win.c
//gcc -o input.so --shared ../../modules/x11input/x11input.c
//gcc -o testapp -lX11 ./window.so ./input.so main.c
//
// SPDX-License-Identifier: GPL-3.0-or-later

int g_keepRunning = 1;

void window_initialize();
void window_processFrame();
void window_deinitialize();

void input_initialize();
void input_processFrame();
void input_deinitialize();

int main(int argc, char** argv)
{
  window_initialize();
  input_initialize();

  while (g_keepRunning) {
    input_processFrame();
    window_processFrame();
  }

  input_deinitialize();
  window_deinitialize();

  return 0;
}
