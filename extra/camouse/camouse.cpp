#include "camouse.hpp"

#include "log.h"

#include "user32.h"
#include "mkxpGlue.h"

HWND camouse_GetWindowHandle() {
  SPDLOG_TRACE("camouse::GetWindowHandle()");

  return user32_FindWindowA("RGSS Player", nullptr);
}

LONG camouse_GetWheelDelta() {
  SPDLOG_TRACE("camouse::GetWheelDelta()");

  return 0;
}

LONG camouse_DisposeHook() {
  SPDLOG_TRACE("camouse::DisposeHook()");

  return 0;
}

LONG camouse_RegenerationHook() {
  SPDLOG_TRACE("camouse::RegenerationHook()");

  return 0;
}
