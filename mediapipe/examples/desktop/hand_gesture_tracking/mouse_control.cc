#include "mouse_control.h"

#include <X11/extensions/XInput2.h>

#include <utility>

inline std::pair<int, int> GetScreenResolution() {
  Display* d = XOpenDisplay(NULL);
  Screen* s = DefaultScreenOfDisplay(d);
  return {s->width, s->height};
}

void MouseControl::SetCursorPosition(int x, int y) {
  static Display* dpy = XOpenDisplay(0);
  static Window root_window = XRootWindow(dpy, 0);

  XSelectInput(dpy, root_window, KeyReleaseMask);
  XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, x, y);
  XFlush(dpy);
}
void MouseControl::SetCursorRelativePos(float x, float y) {
  static std::pair<int, int> ScreenResolution = GetScreenResolution();
  x = (x - 0.05) / 9 * 10;
  if (x < 0) x = 0;
  if (x > 1) x = 1;
  y = (y - 0.05) / 9 * 10;
  if (y < 0) y = 0;
  if (y > 1) y = 1;

  SetCursorPosition(x * ScreenResolution.first, y * ScreenResolution.second);
}