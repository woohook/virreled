//gcc -o window.so --shared -fpic ../../modules/x11win/x11win.c -lX11
//gcc -o input.so  --shared -fpic ../../modules/x11input/x11input.c -lX11
//gcc -o render.so --shared -fpic ../../modules/glxrender/glxrender.c -lGL
//gcc -o glscene.so  --shared -fpic ../../modules/glscene/glscene.c ../../modules/glscene/glmodel.c -lGL
//gcc -o scene.so  --shared -fpic ../../modules/scene/sceneloader.c
//gcc -o entity.so  --shared -fpic ../../modules/entity/entity.c
//gcc -o time.so  --shared -fpic -I../../interfaces ../../modules/time/time.c
//gcc -o physics.so --shared -fpic -I/usr/include -I../../interfaces ../../modules/physics-ode/physics-ode.c -lode
//gcc -o testapp main.c ./window.so ./input.so ./render.so ./glscene.so ./scene.so ./entity.so ./physics.so ./time.so
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

void physics_initialize();
void physics_processFrame();
void physics_deinitialize();

void scene_initialize();
void scene_processFrame();
void scene_deinitialize();

void scene_load(const char* filename);

int main(int argc, char** argv)
{
  window_initialize();
  input_initialize();
  render_initialize();
  physics_initialize();
  scene_initialize();

  scene_load("test.scene");

  while (g_keepRunning) {
    window_processFrame();
    input_processFrame();
    render_processFrame();
    physics_processFrame();
    scene_processFrame();
    render_postProcessFrame();
  }

  scene_deinitialize();
  physics_deinitialize();
  render_deinitialize();
  input_deinitialize();
  window_deinitialize();

  return 0;
}
