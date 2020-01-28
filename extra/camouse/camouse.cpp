#include "camouse.hpp"

#include "user32.h"
#include "mkxpGlue.h"

HWND camouse_GetWindowHandle() {
  return user32_FindWindowA("RGSS Player", nullptr);
}

LONG camouse_GetWheelDelta() {
  return 0;
}

LONG camouse_DisposeHook() {
  return 0;
}

LONG camouse_RegenerationHook() {
  return 0;
}
