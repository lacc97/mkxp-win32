#ifndef CAMOUSE_HPP
#define CAMOUSE_HPP

#include "wintypes.h"

#ifdef __cplusplus
extern "C" {
#endif

WIN32_API HWND camouse_GetWindowHandle();
WIN32_API LONG camouse_GetWheelDelta();
WIN32_API LONG camouse_DisposeHook();
WIN32_API LONG camouse_RegenerationHook();

#ifdef __cplusplus
}
#endif

#endif //CAMOUSE_HPP
