#ifndef GDIPLUS_H
#define GDIPLUS_H

#include "wintypes.h"

#include "gdi32.h"
#include "rpcrt4.h"

#ifdef __cplusplus
extern "C" {
#endif

enum DebugEventLevel { DebugEventLevelFatal, DebugEventLevelWarning };

enum Status {
  Ok,
  GenericError,
  InvalidParameter,
  OutOfMemory,
  ObjectBusy,
  InsufficientBuffer,
  NotImplemented,
  Win32Error,
  WrongState,
  Aborted,
  FileNotFound,
  ValueOverflow,
  AccessDenied,
  UnknownImageFormat,
  FontFamilyNotFound,
  FontStyleNotFound,
  NotTrueTypeFont,
  UnsupportedGdiplusVersion,
  GdiplusNotInitialized,
  PropertyNotFound,
  PropertyNotSupported,
  ProfileNotFound
};
typedef Status GpStatus;

enum PixelFormat {};

struct EncoderParameters;

typedef void (*DebugEventProc)(DebugEventLevel level, CHAR* message);
typedef Status (*NotificationHookProc)(ULONG_PTR* token);
typedef void (*NotificationUnhookProc)(ULONG_PTR token);

struct GdiplusStartupInput {
  UINT32 GdiplusVersion;
  DebugEventProc DebugEventCallback;
  BOOL SuppressBackgroundThread;
  BOOL SuppressExternalCodecs;
};

struct GdiplusStartupOutput {
  NotificationHookProc NotificationHook;
  NotificationUnhookProc NotificationUnhook;
};

struct GpImage;
struct GpBitmap;


WIN32_API Status gdiplus_GdiplusStartup(ULONG_PTR* token, const GdiplusStartupInput* input,
                                        GdiplusStartupOutput* output);
WIN32_API void gdiplus_GdiplusShutdown(ULONG_PTR token);

WIN32_API GpStatus gdiplus_GdipDisposeImage(GpImage* image);
WIN32_API GpStatus gdiplus_GdipSaveImageToFile(GpImage* image, const WCHAR* filename, const CLSID* clsidEncoder,
                                               const EncoderParameters* encoderParams);

WIN32_API GpStatus gdiplus_GdipCreateBitmapFromGdiDib(const BITMAPINFO* gdiBitmapInfo, void* gdiBitmapData,
                                                      GpBitmap** bitmap);
WIN32_API GpStatus gdiplus_GdipCreateBitmapFromHBITMAP(HBITMAP hbm, HPALETTE hpal, GpBitmap** bitmap);
WIN32_API GpStatus gdiplus_GdipCreateBitmapFromScan0(INT width, INT height, INT stride, PixelFormat format, BYTE* scan0,
                                                     GpBitmap** bitmap);

#ifdef __cplusplus
}
#endif


#endif    //GDIPLUS_H
