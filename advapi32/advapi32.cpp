#include "advapi32.h"

#include <unistd.h>

#include <cstring>

#include "log.h"

BOOL advapi32_GetUserName(LPTSTR lpBuffer, LPDWORD pcbBuffer) {
    return advapi32_GetUserNameA(lpBuffer, pcbBuffer);
}

BOOL advapi32_GetUserNameA(LPSTR lpBuffer, LPDWORD pcbBuffer) {
    const size_t BUFSIZE = 128;
    char unBuf[BUFSIZE];

    debug() << "Requested username.";

    if(getlogin_r(unBuf, BUFSIZE) != 0) {
        error() << "Failed to acquire username.";
        return FALSE;
    }

    auto unlen = std::strlen(unBuf);

    if(unlen >= (*pcbBuffer)) {
        warn() << "Could not acquire username. Buffer size << " << (*pcbBuffer) << " is too small (required " << unlen
               << ").";
        return FALSE;
    }

    *pcbBuffer = unlen + 1;
    std::strcpy(lpBuffer, unBuf);

    return TRUE;
}
