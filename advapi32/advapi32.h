#ifndef ADVAPI32_H
#define ADVAPI32_H

#include "wintypes.h"

#ifdef __cplusplus
extern "C" {
#endif

WIN32_API BOOL advapi32_GetUserName(
    LPTSTR   lpBuffer,
    LPDWORD pcbBuffer
);

WIN32_API BOOL advapi32_GetUserNameA(
    LPSTR   lpBuffer,
    LPDWORD pcbBuffer
);

#ifdef __cplusplus
}
#endif

#endif
