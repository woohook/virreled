HDC pDC;
HBITMAP frameBitmap;
int* framebuf;

HWND g_hwnd;
int g_width = 0;
int g_height = 0;
HINSTANCE g_hInstance = 0;

void (*g_handle_key_event)(unsigned int key, int press) = 0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  HDC hDC;
  PAINTSTRUCT ps;

  switch (message) {
    case WM_CREATE:
    {
      // bitmap framebuffer inspired by Sol: https://sol.gfxile.net/wintut/ch3.html
      BITMAPINFO bitmapinfo;
      hDC=CreateCompatibleDC(NULL);
      bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
      bitmapinfo.bmiHeader.biWidth = g_width;
      bitmapinfo.bmiHeader.biHeight = -g_height;
      bitmapinfo.bmiHeader.biPlanes = 1;
      bitmapinfo.bmiHeader.biBitCount = 32;
      bitmapinfo.bmiHeader.biCompression = BI_RGB;
      bitmapinfo.bmiHeader.biSizeImage = 0;
      bitmapinfo.bmiHeader.biClrUsed = 256;
      bitmapinfo.bmiHeader.biClrImportant = 256;
      frameBitmap = CreateDIBSection(hDC, &bitmapinfo, DIB_RGB_COLORS, &framebuf, 0, 0);
      pDC=CreateCompatibleDC(NULL);
      SelectObject(pDC,frameBitmap);
      DeleteDC(hDC);
      break;
    }

    case WM_PAINT:
    {
      hDC=BeginPaint(hwnd,&ps);
      BitBlt(hDC,0,0,g_width,g_height,pDC,0,0,SRCCOPY);
      EndPaint(hwnd,&ps);
      break;
    }

    case WM_KEYUP:
    case WM_KEYDOWN:
      if(g_handle_key_event != 0)
      {
        g_handle_key_event((unsigned int)wParam, (message==WM_KEYDOWN) ? 1 : 0);
      }
      break;

    case WM_DESTROY:
      SelectObject(pDC,NULL);
      DeleteDC(pDC);
      DeleteObject(frameBitmap);
      PostQuitMessage(0);
      quit = 1;
      break;

    default:
      return DefWindowProc(hwnd, message, wParam, lParam);
  }
  return 0;
}

void window_destroy()
{
  DestroyWindow(g_hwnd);
}

void window_set_pixel(int x, int y, int color)
{
  *(framebuf+(y*g_width)+x) = RGB(color&0xFF, (color>>8)&0xFF, (color>>16)&0xFF);
}

void window_flush()
{
  InvalidateRgn(g_hwnd,0,0);
  UpdateWindow (g_hwnd);
}

void window_create(int x, int y, int width, int height)
{
  WNDCLASS wc;

  ZeroMemory(&wc, sizeof wc);
  wc.hInstance     = g_hInstance;
  wc.lpszClassName = "virreled";
  wc.lpfnWndProc   = (WNDPROC)WndProc;
  wc.style         = CS_VREDRAW|CS_HREDRAW;
  wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
  wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor       = LoadCursor(NULL, IDC_CROSS);

  if (FALSE == RegisterClass(&wc))
  {
    printf("RegisterClass failed\n");
    exit(1);
  }

  g_width  = width;
  g_height = height;

  g_hwnd = CreateWindow(
    "virreled",
    "virreled",
    WS_OVERLAPPEDWINDOW|WS_VISIBLE,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    width,
    height,
    0,
    0,
    g_hInstance,
    0);
}

void window_process_events()
{
  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

void window_set_key_handler(void (*handle_key_event)(unsigned int key, int press))
{
  g_handle_key_event = handle_key_event;
}
