#ifndef USER32_H
#define USER32_H

#include "wintypes.h"

#ifdef __cplusplus
extern "C" {
#endif

WIN32_API HWND user32_CreateWindowEx(
        DWORD dwExStyle,
        LPCTSTR lpClassName,
        LPCTSTR lpWindowName,
        DWORD dwStyle,
        int X,
        int Y,
        int nWidth,
        int nHeight,
        HWND hWndParent,
        HMENU hMenu,
        HINSTANCE hInstance,
        LPVOID lpParam
);

WIN32_API HWND user32_CreateWindowExA(
        DWORD dwExStyle,
        LPCSTR lpClassName,
        LPCSTR lpWindowName,
        DWORD dwStyle,
        int X,
        int Y,
        int nWidth,
        int nHeight,
        HWND hWndParent,
        HMENU hMenu,
        HINSTANCE hInstance,
        LPVOID lpParam
);

WIN32_API SHORT user32_GetAsyncKeyState(
        WORD vKey
);

WIN32_API BOOL user32_GetClientRect(
        HWND hWnd,
        PRECT lpRect
);

WIN32_API BOOL user32_GetCursorPos(
        LPPOINT lpPoint
);

WIN32_API HDC user32_GetDC(
        HWND hWnd
);

WIN32_API HWND user32_GetDesktopWindow();

WIN32_API SHORT user32_GetKeyState(
        WORD vKey
);

WIN32_API int user32_GetSystemMetrics(
        int nIndex
);

WIN32_API BOOL user32_GetWindowRect(
        HWND hWnd,
        PRECT lpRect
);

WIN32_API LONG user32_GetWindowLong(
        HWND hWnd,
        int nIndex
);
WIN32_API LONG user32_GetWindowLongA(
        HWND hWnd,
        int nIndex
);

WIN32_API int user32_FillRect(
        HDC hDC,
        const RECT* lprc,
        HBRUSH hbr
);

WIN32_API HWND user32_FindWindow(
        LPCTSTR lpClassName,
        LPCTSTR lpWindowName
);
WIN32_API HWND user32_FindWindowA(
        LPCSTR lpClassName,
        LPCSTR lpWindowName
);

WIN32_API int user32_MessageBox(
        HWND hWnd,
        LPCTSTR lpText,
        LPCTSTR lpCaption,
        UINT uType
);
WIN32_API int user32_MessageBoxA(
        HWND hWnd,
        LPCSTR lpText,
        LPCSTR lpCaption,
        UINT uType
);

WIN32_API BOOL user32_MoveWindow(
        HWND hWnd,
        int X,
        int Y,
        int nWidth,
        int nHeight,
        BOOL bRepaint
);

WIN32_API int user32_ReleaseDC(
        HWND hWnd,
        HDC hDC
);

WIN32_API BOOL user32_ScreenToClient(
        HWND hWnd,
        LPPOINT lpPoint
);

WIN32_API UINT user32_SendInput(
        UINT cInputs,
        LPINPUT pInputs,
        int cbSize
);

WIN32_API BOOL user32_SetCursorPos(
        int X,
        int Y
);

WIN32_API LONG user32_SetWindowLong(
        HWND hWnd,
        int nIndex,
        LONG dwNewLong
);
WIN32_API LONG user32_SetWindowLongA(
        HWND hWnd,
        int nIndex,
        LONG dwNewLong
);

WIN32_API LONG_PTR user32_SetWindowLongPtr(
        HWND hWnd,
        int nIndex,
        LONG_PTR dwNewLong
);
WIN32_API LONG_PTR user32_SetWindowLongPtrA(
        HWND hWnd,
        int nIndex,
        LONG_PTR dwNewLong
);

WIN32_API BOOL user32_SetWindowPos(
        HWND hWnd,
        HWND hWndInsertAfter,
        int X,
        int Y,
        int cx,
        int cy,
        UINT uFlags
);

WIN32_API int user32_ShowCursor(
        BOOL bShow
);

WIN32_API BOOL user32_ShowWindow(
        HWND hWnd,
        int nCmdShow
);

WIN32_API BOOL user32_SystemParametersInfo(
        UINT uiAction,
        UINT uiParam,
        PVOID pvParam,
        UINT fWinIni
);
WIN32_API BOOL user32_SystemParametersInfoA(
        UINT uiAction,
        UINT uiParam,
        PVOID pvParam,
        UINT fWinIni
);

WIN32_API BOOL user32_UpdateWindow(
        HWND hWnd
);

#ifdef __cplusplus
}
#endif

#endif
