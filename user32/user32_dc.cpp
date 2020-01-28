#include "user32.h"

WIN32_API HDC user32_GetDC(HWND hWnd) {
    if(!hWnd)
        return toHANDLE<HWND>(NULL);

    // TODO
    return toHANDLE<HWND>(NULL);
}

WIN32_API int user32_FillRect(HDC hDC, const RECT* lprc, HBRUSH hbr) {
    return TRUE;
}

WIN32_API int user32_ReleaseDC(HWND hWnd, HDC hDC) {
    return TRUE;
}
