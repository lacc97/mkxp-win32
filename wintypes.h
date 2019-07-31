#ifndef WINTYPES_H
#define WINTYPES_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif


#include "common/visibility.h"

#ifndef CONST
#define CONST   const
#endif

#ifndef NULL
#define NULL    0
#endif

#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

typedef void*           PVOID;
typedef void*           LPVOID;

typedef int             BOOL;
typedef BOOL*           LPBOOL;

typedef unsigned char   BYTE;

typedef char            CHAR;
typedef wchar_t         WCHAR;

typedef int16_t         SHORT;
typedef uint16_t        USHORT;

typedef int32_t         INT;
typedef uint32_t        UINT;
typedef int32_t         INT32;
typedef uint32_t        UINT32;

typedef int32_t         LONG;
typedef intptr_t        LONG_PTR;
typedef uint32_t        ULONG;
typedef uintptr_t       ULONG_PTR;

typedef ULONG_PTR       SIZE_T;

typedef unsigned short  WORD;
typedef uint32_t        DWORD;

typedef WORD*           LPWORD;
typedef DWORD*          LPDWORD;

typedef UINT32           HANDLE;
typedef HANDLE          HBRUSH;
typedef HANDLE          HDC;
typedef HANDLE          HGDIOBJ;
typedef HANDLE          HINSTANCE;
typedef HANDLE          HMENU;
typedef HANDLE          HWND;

typedef CHAR*           LPSTR;
typedef WCHAR*          LPWSTR;

typedef CONST CHAR*     LPCCH;
typedef CONST CHAR*     LPCSTR;
typedef CONST WCHAR*    LPCWCH;
typedef CONST WCHAR*    LPCWSTR;

#ifdef UNICODE
typedef LPWSTR          LPTSTR;
#else
typedef LPSTR           LPTSTR;
#endif
#ifdef UNICODE
typedef LPCWSTR         LPCTSTR;
#else
typedef LPCSTR          LPCTSTR;
#endif

typedef DWORD           COLORREF;
typedef DWORD*          LPCOLORREF;

typedef struct tagPOINT {
  LONG x;
  LONG y;
} POINT, *PPOINT, *LPPOINT;

typedef struct _RECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT, *PRECT;

typedef struct tagMOUSEINPUT {
    LONG      dx;
    LONG      dy;
    DWORD     mouseData;
    DWORD     dwFlags;
    DWORD     time;
    ULONG_PTR dwExtraInfo;
} MOUSEINPUT, *PMOUSEINPUT, *LPMOUSEINPUT;


typedef struct tagKEYBDINPUT {
    WORD      wVk;
    WORD      wScan;
    DWORD     dwFlags;
    DWORD     time;
    ULONG_PTR dwExtraInfo;
} KEYBDINPUT, *PKEYBDINPUT, *LPKEYBDINPUT;

typedef struct tagHARDWAREINPUT {
    DWORD uMsg;
    WORD  wParamL;
    WORD  wParamH;
} HARDWAREINPUT, *PHARDWAREINPUT, *LPHARDWAREINPUT;

typedef struct tagINPUT {
    DWORD type;
    union {
        MOUSEINPUT    mi;
        KEYBDINPUT    ki;
        HARDWAREINPUT hi;
    } DUMMYUNIONNAME;
} INPUT, *PINPUT, *LPINPUT;

#undef CONST


template <typename HANDLEType>
HANDLEType newHANDLE(void* ptr);

template <typename HANDLEType>
void* fromHANDLE(HANDLEType handle);

// probably unnecessary
HANDLE toHANDLE(void* ptr);


#endif
