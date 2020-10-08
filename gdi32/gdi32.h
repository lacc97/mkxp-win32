#ifndef GDI32_H
#define GDI32_H

#include "wintypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagRGBQUAD {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
  BITMAPINFOHEADER bmiHeader;
  RGBQUAD          bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;

typedef HANDLE HBITMAP;
typedef HANDLE HBRUSH;
typedef HANDLE HGDIOBJ;
typedef HANDLE HPALETTE;

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
