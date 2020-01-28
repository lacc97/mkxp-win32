#include "kernel32.h"

#include <cstdlib>

#include <unicode.hpp>

#include "iniconfig.h"
#include "log.h"
#include "utils.h"

namespace {
    inline std::string toUnixPath(std::string_view winpath) {
        if(winpath.size() >= 2 && winpath[1] == ':')
            winpath = winpath.substr(2);

        return String::replace_all(winpath, "\\", "/");
    }
}


WIN32_API DWORD kernel32_GetLastError(void) {
    return 0;
}

WIN32_API UINT kernel32_GetPrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName) {
    return kernel32_GetPrivateProfileIntA(lpAppName, lpKeyName, nDefault, lpFileName);
}

WIN32_API UINT kernel32_GetPrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName) {
    SPDLOG_TRACE("kernel32::GetPrivateProfileIntA(lpAppName=\"{}\", lpKeyName=\"{}\", nDefault={}, lpFileName=\"{}\")",
                 lpAppName, lpKeyName, nDefault, lpFileName);

    INIConfiguration privateProfile;
    privateProfile.load(toUnixPath(lpFileName));

    return privateProfile.getIntProperty(lpAppName, lpKeyName, nDefault);
}

namespace {
    template<typename BufferCharType>
    size_t writeStringsToBuffer(const std::vector<std::string>& strs, BufferCharType* buf, size_t bufSize) {
        size_t numWritten = 0;

        for(const std::string& sName : strs) {
            if(sName.length() + numWritten >= bufSize - 2) {
                size_t numCharsToCopy = bufSize - numWritten - 2;
                std::memcpy(buf + numWritten, sName.c_str(), numCharsToCopy);
                numWritten += numCharsToCopy;
                buf[numWritten++] = 0;
                break;
            }

            std::memcpy(buf + numWritten, sName.c_str(), sName.length());
            numWritten += sName.length();
            buf[numWritten++] = 0;
        }

        buf[numWritten++] = 0;
        return numWritten - 2;
    }
}

WIN32_API DWORD kernel32_GetPrivateProfileSection(LPCTSTR lpAppName, LPTSTR lpReturnedString, DWORD nSize,
                                                  LPCTSTR lpFileName) {
    return kernel32_GetPrivateProfileSectionA(lpAppName, lpReturnedString, nSize, lpFileName);
}

WIN32_API DWORD kernel32_GetPrivateProfileSectionA(LPCSTR lpAppName, LPSTR lpReturnedString, DWORD nSize,
                                                   LPCSTR lpFileName) {
    SPDLOG_TRACE(
        "kernel32::GetPrivateProfileSectionA(lpAppName=\"{}\", lpReturnedString={}, nSize={}, lpFileName=\"{}\")",
        lpAppName, (void*) (lpReturnedString), nSize, lpFileName);

    INIConfiguration privateProfile;

    if(!privateProfile.load(toUnixPath(lpFileName)))
        return 0;

    return writeStringsToBuffer(privateProfile.getEntries(lpAppName), lpReturnedString, nSize);
}

WIN32_API DWORD kernel32_GetPrivateProfileSectionNames(LPTSTR lpszReturnBuffer, DWORD nSize, LPCTSTR lpFileName) {
    return kernel32_GetPrivateProfileSectionNamesA(lpszReturnBuffer, nSize, lpFileName);
}

WIN32_API DWORD kernel32_GetPrivateProfileSectionNamesA(LPSTR lpszReturnBuffer, DWORD nSize, LPCSTR lpFileName) {
    SPDLOG_TRACE("kernel32::GetPrivateProfileSectionNamesA(lpszReturnBuffer={}, nSize={}, lpFileName=\"{}\")",
                 (void*) (lpszReturnBuffer), nSize, lpFileName);

    INIConfiguration privateProfile;

    if(!privateProfile.load(toUnixPath(lpFileName)))
        return 0;

    return writeStringsToBuffer(privateProfile.getSectionNames(), lpszReturnBuffer, nSize);
}

WIN32_API DWORD kernel32_GetPrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault,
                                                 LPTSTR lpReturnedString,
                                                 DWORD nSize, LPCTSTR lpFileName) {
    return kernel32_GetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
}

WIN32_API DWORD kernel32_GetPrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault,
                                                  LPSTR lpReturnedString,
                                                  DWORD nSize, LPCSTR lpFileName) {
    SPDLOG_TRACE(
        "kernel32::GetPrivateProfileStringA(lpAppName=\"{}\", lpKeyName=\"{}\", lpDefault=\"{}\", lpReturnedString={}, nSize={}, lpFileName=\"{}\")",
        lpAppName, lpKeyName, lpDefault, (void*) (lpReturnedString), nSize, lpFileName);

    INIConfiguration privateProfile;

    if(!privateProfile.load(toUnixPath(lpFileName)))
        return 0;

    if(!lpAppName || !lpKeyName) {
        std::vector<std::string> names;

        if(!lpAppName)
            names = privateProfile.getSectionNames();
        else
            names = privateProfile.getKeyNames(lpAppName);

        return writeStringsToBuffer(names, lpReturnedString, nSize);
    }

    std::string str = privateProfile.getStringProperty(lpAppName, lpKeyName, lpDefault);
    size_t numCharsToCopy = std::min(str.size(), str.length());

    std::memcpy(lpReturnedString, str.c_str(), numCharsToCopy);
    lpReturnedString[numCharsToCopy] = 0;

    return numCharsToCopy;
}

WIN32_API int kernel32_MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte,
                                           LPWSTR lpWideCharStr, int cchWideChar) {
    unicode::UTF16Converter conv{CodePage};

    std::string_view mbstr;
    if(cbMultiByte == -1)
        mbstr = std::string_view{lpMultiByteStr};
    else if(cbMultiByte == 0)
        return FALSE;
    else
        mbstr = std::string_view{lpMultiByteStr, static_cast<size_t>(cbMultiByte)};

    SPDLOG_DEBUG("Converting CP{0} string to UTF-16", CodePage);

    int extraNullCharacter = (cbMultiByte == -1 ? 1 : 0);

    std::optional<unicode::string> utf16Opt = conv.fromBytes(mbstr);
    if(!utf16Opt)
        return FALSE;
    unicode::string utf16String = std::move(*utf16Opt);

    if(cchWideChar == 0)
        return utf16String.length() + extraNullCharacter;
    else if(cchWideChar < utf16String.length() + extraNullCharacter) {
        spdlog::error("Failed to convert CP{0} string because buffer is too small", CodePage);
        return FALSE;
    }

    std::memcpy(lpWideCharStr, utf16String.c_str(), sizeof(unicode::string::value_type) * utf16String.length());

    if(cbMultiByte == -1)
        lpWideCharStr[utf16String.length()] = 0;

    return utf16String.length() + extraNullCharacter;
}

WIN32_API void kernel32_RtlZeroMemory(PVOID pDestination, SIZE_T nSize) {
    SPDLOG_TRACE("kernel32::RtlZeroMemory(pDestination={}, nSize={})", pDestination, nSize);

    std::memset(pDestination, 0, nSize);
}

WIN32_API int kernel32_WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar,
                                           LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar,
                                           LPBOOL lpUsedDefaultChar) {
    unicode::UTF16Converter conv{CodePage};

    unicode::string_view wcstr;
    if(cchWideChar == -1)
        wcstr = unicode::string_view{lpWideCharStr};
    else if(cchWideChar == 0)
        return FALSE;
    else
        wcstr = unicode::string_view{lpWideCharStr, static_cast<size_t>(cchWideChar)};

    SPDLOG_DEBUG("Converting UTF-16 string to CP{0}", CodePage);

    int extraNullCharacter = (cchWideChar == -1 ? 1 : 0);

    std::optional<std::string> mbStringOpt = conv.toBytes(wcstr);
    if(!mbStringOpt)
        return FALSE;
    std::string mbString = std::move(*mbStringOpt);

    if(cbMultiByte == 0)
        return mbString.length() + extraNullCharacter;
    else if(cchWideChar < mbString.length() + extraNullCharacter) {
        spdlog::error("Failed to convert string to CP{0} because buffer is too small", CodePage);
        return FALSE;
    }

    std::memcpy(lpMultiByteStr, mbString.c_str(), mbString.length());

    if(cchWideChar == -1)
        lpMultiByteStr[mbString.length()] = 0;

    if(lpUsedDefaultChar != NULL)
        *lpUsedDefaultChar = FALSE;

    return mbString.length() + extraNullCharacter;
}

WIN32_API BOOL kernel32_WritePrivateProfileSection(LPCTSTR lpAppName, LPCTSTR lpString, LPCTSTR lpFileName) {
    return kernel32_WritePrivateProfileSectionA(lpAppName, lpString, lpFileName);
}

WIN32_API BOOL kernel32_WritePrivateProfileSectionA(LPCSTR lpAppName, LPCSTR lpString, LPCSTR lpFileName) {
    SPDLOG_TRACE("kernel32::WritePrivateProfileSectionA(lpAppName=\"{}\", lpString=\"{}\", lpFileName=\"{}\")",
                 lpAppName, lpString, lpFileName);

    std::string fp = toUnixPath(lpFileName);

    INIConfiguration privateProfile;

    if(!privateProfile.load(fp))
        return false;

    privateProfile.clearSection(lpAppName);
    privateProfile.addSection(lpAppName);

    LPCSTR str = lpString;

    while(*str != 0) {
        std::string rawProperty(str);

        privateProfile.addRawProperty(lpAppName, rawProperty);

        str += rawProperty.size() + 1;
    }

    return TRUE;
}

WIN32_API BOOL kernel32_WritePrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString,
                                                  LPCTSTR lpFileName) {
    return kernel32_WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}

WIN32_API BOOL kernel32_WritePrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString,
                                                   LPCSTR lpFileName) {
    SPDLOG_TRACE(
        "kernel32::WritePrivateProfileStringA(lpAppName=\"{}\", lpKeyName=\"{}\", lpString=\"{}\", lpFileName=\"{}\")",
        lpAppName, lpKeyName, lpString, lpFileName);

    std::string fp = toUnixPath(lpFileName);

    INIConfiguration privateProfile;

    if(!privateProfile.load(fp))
        return false;

    privateProfile.setStringProperty(lpAppName, lpKeyName, lpString);

    return privateProfile.save(fp);
}
