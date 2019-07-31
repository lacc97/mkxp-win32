#include "kernel32.h"

#include <cstdlib>

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>

#include "iniconfig.h"
#include "log.h"

inline std::string toUnixPath(const std::string& winpath) {
    std::string unixpath = boost::replace_all_copy(winpath, "\\", "/");

    if(unixpath[1] == ':')
        return unixpath.substr(2);
    else
        return unixpath;
}


DWORD kernel32_GetLastError(void) {
    return 0;
}

UINT kernel32_GetPrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName) {
    return kernel32_GetPrivateProfileIntA(lpAppName, lpKeyName, nDefault, lpFileName);
}

UINT kernel32_GetPrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName) {
    debug() << "Requesting '" << lpAppName << "/" << lpKeyName << "' from file '" << lpFileName << "'.";

    INIConfiguration privateProfile;
    privateProfile.load(toUnixPath(lpFileName));

    int ret = privateProfile.getIntProperty(lpAppName, lpKeyName, nDefault);

    debug() << "Entry '" << lpAppName << "." << lpKeyName << "' has value " << ret;

    return ret;
}

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

DWORD kernel32_GetPrivateProfileSection(LPCTSTR lpAppName, LPTSTR lpReturnedString, DWORD nSize, LPCTSTR lpFileName) {
    return kernel32_GetPrivateProfileSectionA(lpAppName, lpReturnedString, nSize, lpFileName);
}

DWORD kernel32_GetPrivateProfileSectionA(LPCSTR lpAppName, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName) {
    debug() << "Requesting section '" << lpAppName << "' from file '" << lpFileName << "'.";

    INIConfiguration privateProfile;

    if(!privateProfile.load(toUnixPath(lpFileName)))
        return 0;

    return writeStringsToBuffer(privateProfile.getEntries(lpAppName), lpReturnedString, nSize);
}

DWORD kernel32_GetPrivateProfileSectionNames(LPTSTR lpszReturnBuffer, DWORD nSize, LPCTSTR lpFileName) {
    return kernel32_GetPrivateProfileSectionNamesA(lpszReturnBuffer, nSize, lpFileName);
}

DWORD kernel32_GetPrivateProfileSectionNamesA(LPSTR lpszReturnBuffer, DWORD nSize, LPCSTR lpFileName) {
    debug() << "Requesting section names from file '" << lpFileName << "'.";

    INIConfiguration privateProfile;

    if(!privateProfile.load(toUnixPath(lpFileName)))
        return 0;

    return writeStringsToBuffer(privateProfile.getSectionNames(), lpszReturnBuffer, nSize);
}

DWORD kernel32_GetPrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedString,
                                       DWORD nSize, LPCTSTR lpFileName) {
    return kernel32_GetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
}

DWORD kernel32_GetPrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString,
                                        DWORD nSize, LPCSTR lpFileName) {
    debug() << "Requesting '" << lpAppName << "/" << lpKeyName << "' from file '" << lpFileName << "'.";

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

typedef std::wstring wcstring;

std::string fromCodePageToCharSet(UINT CodePage) {
    switch((CodeP) CodePage) {
        case CP_ACP:
            return "Latin1";
            break;

        case CP_UTF7:
            return "UTF-7";
            break;

        case CP_UTF8:
            return "UTF-8";
            break;
    }

    return "";
}

int
kernel32_MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr,
                             int cchWideChar) {
    std::string charset = fromCodePageToCharSet(CodePage);

    std::string mbstr;

    if(cbMultiByte == -1)
        mbstr = std::string(lpMultiByteStr);
    else if(cbMultiByte == 0)
        return FALSE;
    else
        mbstr = std::string(lpMultiByteStr, cbMultiByte);

    debug() << "Converting " << charset << "string to UTF-16.";

    int extraNullCharacter = (cbMultiByte == -1 ? 1 : 0);

    wcstring utf16_string = boost::locale::conv::to_utf<wcstring::value_type>(mbstr, charset);

    if(cchWideChar == 0)
        return utf16_string.length() + extraNullCharacter;
    else if(cchWideChar < utf16_string.length() + extraNullCharacter) {
        error() << "Failed to convert " << charset << " string. Buffer is too small.";
        return FALSE;
    }

    std::memcpy(lpWideCharStr, utf16_string.c_str(), sizeof(wcstring::value_type) * utf16_string.length());

    if(cbMultiByte == -1)
        lpWideCharStr[utf16_string.length()] = 0;

    return utf16_string.length() + extraNullCharacter;
}

void kernel32_RtlZeroMemory(PVOID ptr, SIZE_T cnt) {
    trace() << "Zeroing memory of size " << cnt << " at address " << ptr << ".";

    std::memset(ptr, 0, cnt);
}

int
kernel32_WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr,
                             int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar) {
    std::string charset = fromCodePageToCharSet(CodePage);

    wcstring wcstr;

    if(cchWideChar == -1)
        wcstr = wcstring(lpWideCharStr);
    else if(cchWideChar == 0)
        return FALSE;
    else
        wcstr = wcstring(lpWideCharStr, cchWideChar);

    debug() << "Converting UTF-16 string to " << charset << ".";

    int extraNullCharacter = (cchWideChar == -1 ? 1 : 0);

    std::string mb_string = boost::locale::conv::from_utf<wcstring::value_type>(wcstr, charset);

    if(cbMultiByte == 0)
        return mb_string.length() + extraNullCharacter;
    else if(cchWideChar < mb_string.length() + extraNullCharacter) {
        error() << "Failed to convert string to " << charset << ". Buffer is too small.";
        return FALSE;
    }

    std::memcpy(lpMultiByteStr, mb_string.c_str(), mb_string.length());

    if(cchWideChar == -1)
        lpMultiByteStr[mb_string.length()] = 0;

    if(lpUsedDefaultChar != NULL)
        *lpUsedDefaultChar = FALSE;

    return mb_string.length() + extraNullCharacter;
}

BOOL kernel32_WritePrivateProfileSection(LPCTSTR lpAppName, LPCTSTR lpString, LPCTSTR lpFileName) {
    return kernel32_WritePrivateProfileSectionA(lpAppName, lpString, lpFileName);
}

BOOL kernel32_WritePrivateProfileSectionA(LPCSTR lpAppName, LPCSTR lpString, LPCSTR lpFileName) {
    debug() << "Writing section '" << lpAppName << "' in file '" << lpFileName << "'.";

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

BOOL kernel32_WritePrivateProfileString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, LPCTSTR lpFileName) {
    return kernel32_WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}

BOOL kernel32_WritePrivateProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString, LPCSTR lpFileName) {
    debug() << "Writing string '" << lpString << "' to '" << lpAppName << "/" << lpKeyName << "' in file '"
            << lpFileName << "'.";

    std::string fp = toUnixPath(lpFileName);

    INIConfiguration privateProfile;

    if(!privateProfile.load(fp))
        return false;

    privateProfile.setStringProperty(lpAppName, lpKeyName, lpString);

    return privateProfile.save(fp);
}
