//i686-w64-mingw32-gcc ../../modules/glscene/*.c simplewindow.c -mwindows -lopengl32
//*/

#include <windows.h>
#include <GL/gl.h>

int       g_keepRunning = 1;

extern float* g_rx;
extern float* g_ry;
extern float* g_rz;

HINSTANCE g_hInstance     = 0;
int       g_nCmdShow      = 0;
HDC       g_renderContext = 0;
HGLRC     g_glContext     = 0;
int       g_width         = 300;
int       g_height        = 200;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void window_initialize()
{
    MSG  msg;
    HWND hwnd;
    WNDCLASSW wc;

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.lpszClassName = L"Window";
    wc.hInstance     = g_hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpszMenuName  = NULL;
    wc.lpfnWndProc   = WndProc;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassW(&wc);
    hwnd = CreateWindowW(wc.lpszClassName, L"Window",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                100, 100, g_width, g_height, NULL, NULL, g_hInstance, NULL);

    ShowWindow(hwnd, g_nCmdShow);
    UpdateWindow(hwnd);

    g_renderContext = GetDC(hwnd);

  PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW |
    PFD_SUPPORT_OPENGL |
    PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA,
    24,
    0, 0, 0, 0, 0, 0,
    0,
    0,
    0,
    0, 0, 0, 0,
    32,
    0,
    0,
    PFD_MAIN_PLANE,
    0,
    0, 0, 0
  };

  int pixelFormat = ChoosePixelFormat (g_renderContext,&pfd);
  SetPixelFormat(g_renderContext, pixelFormat, &pfd);

  g_glContext = wglCreateContext(g_renderContext);
  wglMakeCurrent(g_renderContext, g_glContext);
}

void window_processFrame()
{
}

void window_deinitialize()
{
  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(g_glContext);

  g_glContext     = 0;
  g_renderContext = 0;
  g_hInstance     = 0;
}

void input_initialize()
{
}

void input_processFrame()
{
  MSG  msg;
      while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE)
      {
        if (GetMessage(&msg, NULL, 0, 0) )
        {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        } else {
          g_keepRunning = 0;
          return;
        }
      }
}

void input_deinitialize()
{
}

void render_initialize()
{
}

void render_processFrame()
{
}

void render_postProcessFrame()
{
  SwapBuffers(g_renderContext);
}

void render_deinitialize()
{
}

LRESULT CALLBACK
WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
      case WM_DESTROY:
          PostQuitMessage(0);
          break;
    case WM_CHAR:
        switch ((int)wParam) {
        case 'q':
          g_keepRunning = 0;
          break;
        case 'a':
          *g_ry = *g_ry + 10.0f;
          break;
        case 'd':
          *g_ry = *g_ry - 10.0f;
          break;
        case 'w':
          *g_rx = *g_rx + 10.0f;
          break;
        case 's':
          *g_rx = *g_rx - 10.0f;
          break;
        case 'x':
          *g_rz = *g_rz + 10.0f;
          break;
        case 'y':
          *g_rz = *g_rz - 10.0f;
          break;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
  g_hInstance = hInstance;
  g_nCmdShow  = nCmdShow;

  window_initialize();
  input_initialize();
  render_initialize();
  scene_initialize();

  scene_load("test.scene");

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
