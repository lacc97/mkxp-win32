#include "user32.h"

HDC user32_GetDC(HWND hWnd) {
    if(!hWnd)
        return NULL;

    // TODO
    return NULL;
}

int user32_FillRect(HDC hDC, const RECT* lprc, HBRUSH hbr) {
    return TRUE;
}

int user32_ReleaseDC(HWND hWnd, HDC hDC) {
    return TRUE;
}
