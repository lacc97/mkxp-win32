#include "gdiplus.h"

WIN32_API Status gdiplus_GdiplusStartup(ULONG_PTR* token, const GdiplusStartupInput* input,
                                        GdiplusStartupOutput* output) {
  return Ok;
}
WIN32_API void gdiplus_GdiplusShutdown(ULONG_PTR token) {}
WIN32_API GpStatus gdiplus_GdipDisposeImage(GpImage* image) {
  return Ok;
}
WIN32_API GpStatus gdiplus_GdipSaveImageToFile(GpImage* image, const WCHAR* filename, const CLSID* clsidEncoder,
                                               const EncoderParameters* encoderParams) {
  return Ok;
}
WIN32_API GpStatus gdiplus_GdipCreateBitmapFromGdiDib(const BITMAPINFO* gdiBitmapInfo, void* gdiBitmapData,
                                                      GpBitmap** bitmap) {
  return Ok;
}
WIN32_API GpStatus gdiplus_GdipCreateBitmapFromHBITMAP(HBITMAP hbm, HPALETTE hpal, GpBitmap** bitmap) {
  return Ok;
}
WIN32_API GpStatus gdiplus_GdipCreateBitmapFromScan0(INT width, INT height, INT stride, PixelFormat format, BYTE* scan0,
                                                     GpBitmap** bitmap) {
  return Ok;
}
