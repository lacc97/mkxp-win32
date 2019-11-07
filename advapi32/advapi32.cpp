#include "advapi32.h"

#include <unistd.h>

#include <cstring>

#include "log.h"

BOOL advapi32_GetUserName(LPTSTR lpBuffer, LPDWORD pcbBuffer) {
    return advapi32_GetUserNameA(lpBuffer, pcbBuffer);
}

BOOL advapi32_GetUserNameA(LPSTR lpBuffer, LPDWORD pcbBuffer) {
    constexpr size_t BUFSIZE = 128;
    char unBuf[BUFSIZE];

    SPDLOG_TRACE("advapi32::GetUserNameA(lpBuffer={}, pcbBuffer={})", (void*)(lpBuffer), (void*)(pcbBuffer));

    if(getlogin_r(unBuf, BUFSIZE) != 0) {
        spdlog::error("Failed to acquire username");
        return FALSE;
    }

    auto unlen = std::strlen(unBuf);

    if(unlen >= (*pcbBuffer)) {
        spdlog::warn("Could not acquire username because buffer ({} bytes) is too small (requires {} bytes)", *pcbBuffer, unlen);
        return FALSE;
    }

    *pcbBuffer = unlen + 1;
    std::strcpy(lpBuffer, unBuf);

    return TRUE;
}
