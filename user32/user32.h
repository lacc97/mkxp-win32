#ifndef USER32_H
#define USER32_H

#include "wintypes.h"

#ifdef __cplusplus
extern "C" {
#endif

int MessageBoxA(
    HWND   hWnd,
    LPCSTR lpText,
    LPCSTR lpCaption,
    UINT   uType
);

#ifdef __cplusplus
}
#endif

#endif
