#include "gdi32.h"

HBRUSH gdi32_CreateSolidBrush(COLORREF color) {
    return toHANDLE<HBRUSH>(NULL);
}

BOOL gdi32_DeleteObject(HGDIOBJ ho) {
    return FALSE;
}
