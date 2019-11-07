#ifndef GDI32_H
#define GDI32_H

#include "wintypes.h"

#ifdef __cplusplus
extern "C" {
#endif

WIN32_API HBRUSH gdi32_CreateSolidBrush(
    COLORREF color
);

WIN32_API BOOL gdi32_DeleteObject(
    HGDIOBJ ho
);

#ifdef __cplusplus
}
#endif

#endif
