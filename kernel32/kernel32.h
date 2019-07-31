#ifndef KERNEL32_H
#define KERNEL32_H

#include "wintypes.h"

enum CodeP : UINT {
    CP_ACP = 0,
    CP_UTF7 = 65000,
    CP_UTF8 = 65001
};

#ifdef __cplusplus
extern "C" {
#endif

WIN32_API DWORD kernel32_GetLastError(void);

WIN32_API UINT kernel32_GetPrivateProfileInt(
    LPCTSTR lpAppName,
    LPCTSTR lpKeyName,
    INT nDefault,
    LPCTSTR lpFileName
);
WIN32_API UINT kernel32_GetPrivateProfileIntA(
    LPCSTR lpAppName,
    LPCSTR lpKeyName,
    INT nDefault,
    LPCSTR lpFileName
);

WIN32_API DWORD kernel32_GetPrivateProfileSection(
    LPCTSTR lpAppName,
    LPTSTR lpReturnedString,
    DWORD nSize,
    LPCTSTR lpFileName
);
WIN32_API DWORD kernel32_GetPrivateProfileSectionA(
    LPCSTR lpAppName,
    LPSTR lpReturnedString,
    DWORD nSize,
    LPCSTR lpFileName
);

WIN32_API DWORD kernel32_GetPrivateProfileSectionNames(
    LPTSTR lpszReturnBuffer,
    DWORD nSize,
    LPCTSTR lpFileName
);
WIN32_API DWORD kernel32_GetPrivateProfileSectionNamesA(
    LPSTR lpszReturnBuffer,
    DWORD nSize,
    LPCSTR lpFileName
);

WIN32_API DWORD kernel32_GetPrivateProfileString(
    LPCTSTR lpAppName,
    LPCTSTR lpKeyName,
    LPCTSTR lpDefault,
    LPTSTR lpReturnedString,
    DWORD nSize,
    LPCTSTR lpFileName
);
WIN32_API DWORD kernel32_GetPrivateProfileStringA(
    LPCSTR lpAppName,
    LPCSTR lpKeyName,
    LPCSTR lpDefault,
    LPSTR lpReturnedString,
    DWORD nSize,
    LPCSTR lpFileName
);

WIN32_API int kernel32_MultiByteToWideChar(
    UINT CodePage,
    DWORD dwFlags,
    LPCCH lpMultiByteStr,
    int cbMultiByte,
    LPWSTR lpWideCharStr,
    int cchWideChar
);

WIN32_API void kernel32_RtlZeroMemory(
    PVOID ptr,
    SIZE_T cnt
);

WIN32_API int kernel32_WideCharToMultiByte(
    UINT CodePage,
    DWORD dwFlags,
    LPCWCH lpWideCharStr,
    int cchWideChar,
    LPSTR lpMultiByteStr,
    int cbMultiByte,
    LPCCH lpDefaultChar,
    LPBOOL lpUsedDefaultChar
);

WIN32_API BOOL kernel32_WritePrivateProfileSection(
    LPCTSTR lpAppName,
    LPCTSTR lpString,
    LPCTSTR lpFileName
);
WIN32_API BOOL kernel32_WritePrivateProfileSectionA(
    LPCSTR lpAppName,
    LPCSTR lpString,
    LPCSTR lpFileName
);


WIN32_API BOOL kernel32_WritePrivateProfileString(
    LPCTSTR lpAppName,
    LPCTSTR lpKeyName,
    LPCTSTR lpString,
    LPCTSTR lpFileName
);
WIN32_API BOOL kernel32_WritePrivateProfileStringA(
    LPCSTR lpAppName,
    LPCSTR lpKeyName,
    LPCSTR lpString,
    LPCSTR lpFileName
);

#ifdef __cplusplus
}
#endif

#endif
