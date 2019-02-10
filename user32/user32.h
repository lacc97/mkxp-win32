#ifndef USER32_H
#define USER32_H

#include "wintypes.h"

#ifdef __cplusplus
extern "C" {
#endif

HWND user32_CreateWindowEx(
    DWORD     dwExStyle,
    LPCTSTR   lpClassName,
    LPCTSTR   lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam
);

HWND user32_CreateWindowExA(
    DWORD     dwExStyle,
    LPCSTR    lpClassName,
    LPCSTR    lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam
);

SHORT user32_GetAsyncKeyState(
  WORD vKey
);

BOOL user32_GetClientRect(
    HWND  hWnd,
    PRECT lpRect
);

BOOL user32_GetCursorPos(
  LPPOINT lpPoint
);

HDC user32_GetDC(
    HWND hWnd
);

HWND user32_GetDesktopWindow();

int user32_GetSystemMetrics(
    int nIndex
);

BOOL user32_GetWindowRect(
    HWND  hWnd,
    PRECT lpRect
);

LONG user32_GetWindowLong(
    HWND hWnd,
    int  nIndex
);
LONG user32_GetWindowLongA(
    HWND hWnd,
    int  nIndex
);

int user32_FillRect(
    HDC        hDC,
    const RECT* lprc,
    HBRUSH     hbr
);

HWND user32_FindWindow(
    LPCTSTR lpClassName,
    LPCTSTR lpWindowName
);
HWND user32_FindWindowA(
    LPCSTR lpClassName,
    LPCSTR lpWindowName
);

int user32_MessageBox(
    HWND    hWnd,
    LPCTSTR lpText,
    LPCTSTR lpCaption,
    UINT    uType
);
int user32_MessageBoxA(
    HWND   hWnd,
    LPCSTR lpText,
    LPCSTR lpCaption,
    UINT   uType
);

int user32_ReleaseDC(
    HWND hWnd,
    HDC  hDC
);

UINT user32_SendInput(
  UINT    cInputs,
  LPINPUT pInputs,
  int     cbSize
);

LONG user32_SetWindowLong(
  HWND hWnd,
  int  nIndex,
  LONG dwNewLong
);
LONG user32_SetWindowLongA(
  HWND hWnd,
  int  nIndex,
  LONG dwNewLong
);

LONG_PTR user32_SetWindowLongPtr(
  HWND     hWnd,
  int      nIndex,
  LONG_PTR dwNewLong
);
LONG_PTR user32_SetWindowLongPtrA(
  HWND     hWnd,
  int      nIndex,
  LONG_PTR dwNewLong
);

BOOL user32_SetWindowPos(
    HWND hWnd,
    HWND hWndInsertAfter,
    int  X,
    int  Y,
    int  cx,
    int  cy,
    UINT uFlags
);

BOOL user32_ShowWindow(
  HWND hWnd,
  int  nCmdShow
);

BOOL user32_SystemParametersInfo(
    UINT  uiAction,
    UINT  uiParam,
    PVOID pvParam,
    UINT  fWinIni
);
BOOL user32_SystemParametersInfoA(
    UINT  uiAction,
    UINT  uiParam,
    PVOID pvParam,
    UINT  fWinIni
);

BOOL user32_UpdateWindow(
  HWND hWnd
);

#ifdef __cplusplus
}
#endif

#endif
