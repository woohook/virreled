//gcc -o window.so --shared -lX11 ../../modules/x11win/x11win.c
//gcc -o input.so --shared -lX11 ../../modules/x11input/x11input.c
//gcc -o render.so --shared -lGL ../../modules/glxrender/glxrender.c
//gcc -o scene.so --shared -lGL ../../modules/glscene/glscene.c
//gcc -o testapp ./window.so ./input.so ./render.so ./scene.so main.c
//
// SPDX-License-Identifier: GPL-3.0-or-later

int g_keepRunning = 1;

void window_initialize();
void window_processFrame();
void window_deinitialize();

void render_initialize();
void render_processFrame();
void render_postProcessFrame();
void render_deinitialize();

void input_initialize();
void input_processFrame();
void input_deinitialize();

void scene_initialize();
void scene_processFrame();
void scene_deinitialize();

int main(int argc, char** argv)
{
  window_initialize();
  input_initialize();
  render_initialize();
  render_initialize();

  while (g_keepRunning) {
    window_processFrame();
    input_processFrame();
    render_processFrame();
    scene_processFrame();
    render_postProcessFrame();
  }

  scene_deinitialize();
  render_deinitialize();
  input_deinitialize();
  window_deinitialize();

  return 0;
}
