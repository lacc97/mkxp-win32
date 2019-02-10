#include "user32.h"

#include <unistd.h>

#include <cstring>

#include <iostream>
#include <map>

#include <SDL.h>

#include "log.h"
#include "x11utils.h"
#include "vkcodes.h"

HWND user32_CreateWindowEx(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    return user32_CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

HWND user32_CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    return NULL;
}

SHORT user32_GetAsyncKeyState(WORD vKey) {
    const auto* state = SDL_GetKeyboardState(NULL);
    
    return (state[toSDLScancode(vKey)] ? ~(SHORT{-1} >> 1) : 0);
}

BOOL user32_GetClientRect(HWND hWnd, PRECT lpRect) {
    debug() << "Requested size of window " << hWnd << ".";

    auto display = X11::GetDisplay();
    auto window = reinterpret_cast<Window>(hWnd);

    Window wRoot;
    int wX, wY;
    unsigned int wWidth, wHeight;
    unsigned int wBorderWidth;
    unsigned int wDepthReturn;

    if(XGetGeometry(display, window, &wRoot, &wX, &wY, &wWidth, &wHeight, &wBorderWidth, &wDepthReturn) == 0) {
        error() << "Failed to get geometry of window " << hWnd << ".";
        return FALSE;
    }

    lpRect->left    = 0;
    lpRect->top     = 0;
    lpRect->right   = wWidth;
    lpRect->bottom  = wHeight;

    debug() << "Window " << hWnd << " has size " << wWidth << "x" << wHeight << ".";

    return TRUE;
}

BOOL user32_GetCursorPos(LPPOINT lpPoint) {
    SDL_GetGlobalMouseState(&lpPoint->x, &lpPoint->y);
    
    return TRUE;
}

HWND user32_GetDesktopWindow() {
    trace() << "Requested desktop window.";
    return reinterpret_cast<HWND>(X11::GetRootWindow(X11::GetDisplay()));
}

int user32_GetSystemMetrics(int nIndex) {
    debug() << "Requested system metric " << nIndex << ".";

    switch(nIndex) {
        case 0: {     // CXSCREEN
            unsigned int height, width;

            if(X11::GetScreenSize(X11::GetDefaultScreen(X11::GetDisplay()), width, height))
                return width;
        }

        case 1: {    // CYSCREEN
            unsigned int height, width;

            if(X11::GetScreenSize(X11::GetDefaultScreen(X11::GetDisplay()), width, height))
                return height;
        }

    }

    return 0;
}

BOOL user32_GetWindowRect(HWND hWnd, PRECT lpRect) {
    debug() << "Requested position of window " << hWnd << " relative to screen.";

    auto display = X11::GetDisplay();
    auto window = reinterpret_cast<Window>(hWnd);

    unsigned int wWidth, wHeight;

    if(!X11::GetPosRelativeToScreen(display, window, lpRect->left, lpRect->top)) {
        error() << "Failed to get position of window " << hWnd << " relative to screen.";
        return false;
    }

    if(!X11::GetSize(display, window, wWidth, wHeight)) {
        error() << "Failed to get size of window " << hWnd << ".";
        return false;
    }

    lpRect->right   = lpRect->left + wWidth;
    lpRect->bottom  = lpRect->top + wHeight;

    debug() << "Window " << hWnd << " has position (" << lpRect->left << ", " << lpRect->top << ") -- (" << lpRect->right << ", " << lpRect->bottom << ").";

    return TRUE;
}

LONG user32_GetWindowLong(HWND hWnd, int nIndex) {
    return user32_GetWindowLongA(hWnd, nIndex);
}

LONG user32_GetWindowLongA(HWND hWnd, int nIndex) {
    debug() << "Requested parameter " << nIndex << " of window " << hWnd << ".";

    switch(nIndex) {
        default:
            warn() << "Ignored window parameter " << nIndex << " request of window " << hWnd << ".";
            return FALSE;
    }
}

HWND user32_FindWindow(LPCSTR lpClassName, LPCSTR lpWindowName) {
    return user32_FindWindowA(lpClassName, lpWindowName);
}

HWND user32_FindWindowA(LPCSTR lpClassName, LPCSTR lpWindowName) {
    if(lpWindowName) {
        debug() << "Requested window of class '" << lpClassName << "' and name '" << lpWindowName << "'.";
    } else {
        debug() << "Requested window of class '" << lpClassName << "'.";
    }

    if(std::strcmp(lpClassName, "RGSS Player") != 0) {
        warn() << "Window class is not 'RGSS Player'. Skipping.";
        return NULL;
    }

    auto thisPid = getpid();
    auto display = X11::GetDisplay();

    std::vector<Window> results = X11::FindWindows(display, X11::GetRootWindow(display), [thisPid](Display * d, Window win) -> bool {
        return (thisPid == X11::GetWindowPID(d, win));
    });

    if(results.size() > 0) {
        debug() << "Found windows: ";

        for(Window w : results)
            debug() << " - " << X11::GetWindowName(display, w) << " (" << reinterpret_cast<HWND>(w) << ")";

        debug() << "out of which we pick " << reinterpret_cast<HWND>(results[0]) << ".";
        return reinterpret_cast<HWND>(results[0]);
    } else {
        if(lpWindowName)
            info() << "Window of class '" << lpClassName << "' and name '" << lpWindowName << "' not found.";
        else
            info() << "Window of class '" << lpClassName << "' not found.";

        return NULL;
    }
}

int user32_MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType) {
    return user32_MessageBoxA(hWnd, lpText, lpCaption, uType);
}

int user32_MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    constexpr UINT MB_OK                = 0x00000000;
    constexpr UINT MB_OKCANCEL          = 0x00000001;
    constexpr UINT MB_ABORTRETRYIGNORE  = 0x00000002;
    constexpr UINT MB_YESNOCANCEL       = 0x00000003;
    constexpr UINT MB_YESNO             = 0x00000004;
    constexpr UINT MB_RETRYCANCEL       = 0x00000005;
    constexpr UINT MB_CANCELTRYCONTINUE = 0x00000006;

    constexpr int IDOK          = 1;
    constexpr int IDCANCEL      = 2;
    constexpr int IDABORT       = 3;
    constexpr int IDRETRY       = 4;
    constexpr int IDIGNORE      = 5;
    constexpr int IDYES         = 6;
    constexpr int IDNO          = 7;
    constexpr int IDTRYAGAIN    = 10;
    constexpr int IDCONTINUE    = 11;

    debug() << "Showing message box with title '" << lpCaption << "' and text '" << lpText << "'";

    UINT dialogButtonsFlag = (uType & 0x0000000f);
    UINT dialogTypeFlag    = (uType & 0x000000f0);

    int numbuttons;
    SDL_MessageBoxButtonData buttons[3];

    switch(dialogButtonsFlag) {
        case MB_OK:
            numbuttons   = 1;
            buttons[0].flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT | SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[0].buttonid = IDOK;
            buttons[0].text     = "Ok";
            break;

        case MB_OKCANCEL:
            numbuttons   = 2;
            buttons[0].flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].buttonid = IDOK;
            buttons[0].text     = "Ok";
            buttons[1].flags    = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[1].buttonid = IDCANCEL;
            buttons[1].text     = "Cancel";
            break;

        case MB_ABORTRETRYIGNORE:
            numbuttons   = 3;
            buttons[0].flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].buttonid = IDABORT;
            buttons[0].text     = "Abort";
            buttons[1].flags    = 0;
            buttons[1].buttonid = IDRETRY;
            buttons[1].text     = "Retry";
            buttons[2].flags    = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[2].buttonid = IDIGNORE;
            buttons[2].text     = "Ignore";
            break;

        case MB_YESNOCANCEL:
            numbuttons   = 3;
            buttons[0].flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].buttonid = IDYES;
            buttons[0].text     = "Yes";
            buttons[1].flags    = 0;
            buttons[1].buttonid = IDNO;
            buttons[1].text     = "No";
            buttons[2].flags    = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[2].buttonid = IDCANCEL;
            buttons[2].text     = "Cancel";
            break;

        case MB_YESNO:
            numbuttons   = 2;
            buttons[0].flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].buttonid = IDYES;
            buttons[0].text     = "Yes";
            buttons[1].flags    = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[1].buttonid = IDNO;
            buttons[1].text     = "No";
            break;

        case MB_RETRYCANCEL:
            numbuttons   = 2;
            buttons[0].flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].buttonid = IDRETRY;
            buttons[0].text     = "Retry";
            buttons[1].flags    = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[1].buttonid = IDCANCEL;
            buttons[1].text     = "Cancel";
            break;

        case MB_CANCELTRYCONTINUE:
            numbuttons   = 3;
            buttons[0].flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].buttonid = IDCANCEL;
            buttons[0].text     = "Cancel";
            buttons[1].flags    = 0;
            buttons[1].buttonid = IDTRYAGAIN;
            buttons[1].text     = "Try Again";
            buttons[2].flags    = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[2].buttonid = IDCONTINUE;
            buttons[2].text     = "Continue";
            break;

    }

    Uint32 flags;

    switch(dialogTypeFlag) {
        case 0x00000010: // STOP
            flags = SDL_MESSAGEBOX_ERROR;
            break;

        case 0x00000020: // QUESTION
        case 0x00000040: // INFORMATION
            flags = SDL_MESSAGEBOX_INFORMATION;
            break;

        case 0x00000030: // WARN
            flags = SDL_MESSAGEBOX_WARNING;
            break;
    }

    SDL_MessageBoxData mbData;
    mbData.flags        = flags;
    mbData.window       = NULL;
    mbData.title        = lpCaption;
    mbData.message      = lpText;
    mbData.numbuttons   = numbuttons;
    mbData.buttons      = buttons;
    mbData.colorScheme  = NULL;

    int retVal;

    if(SDL_ShowMessageBox(&mbData, &retVal) != 0)
        error() << "SDL_ShowMessageBox failed: " << SDL_GetError();

    return retVal;
}

UINT user32_SendInput(UINT cInputs, LPINPUT pInputs, int cbSize) {
    constexpr DWORD INPUT_MOUSE     = 0;
    constexpr DWORD INPUT_KEYBOARD  = 1;
//     constexpr DWORD INPUT_HARDWARE  = 2;

    debug() << "Sending " << cInputs << "synthesized input events.";

    if(cbSize != sizeof(INPUT)) {
        error() << "Size of INPUT type does not match (" << sizeof(INPUT) << " != " << cbSize << ").";
        return 0;
    }

    UINT count = 0;

    for(size_t ii = 0; ii < cInputs; ii++) {
        INPUT winEv = pInputs[ii];
        SDL_Event sdlEv;

        switch(winEv.type) {
            case INPUT_MOUSE: {
//                 constexpr DWORD XBUTTON1 = 0x0001;
//                 constexpr DWORD XBUTTON2 = 0x0002;
//
//                 if(winEv.DUMMYUNIONNAME.mi.time == 0)
//                     winEv.DUMMYUNIONNAME.mi.time = time(NULL);
//
//
//
                warn() << "Skipping MOUSEINPUT event.";

                break;
            }

            case INPUT_KEYBOARD: {
                constexpr DWORD KEYEVENTF_EXTENDEDKEY = 0x0001;
                constexpr DWORD KEYEVENTF_KEYUP       = 0x0002;
                constexpr DWORD KEYEVENTF_SCANCODE    = 0x0008;
                constexpr DWORD KEYEVENTF_UNICODE     = 0x0004;

                if((winEv.DUMMYUNIONNAME.ki.dwFlags & KEYEVENTF_UNICODE) != 0) {
                    warn() << "Skipping unicode KEYBDINPUT event.";
                    break;
                }

                if((winEv.DUMMYUNIONNAME.ki.dwFlags & KEYEVENTF_SCANCODE) != 0) {
                    warn() << "Skipping scan code KEYBDINPUT event.";
                    break;
                }

                if(winEv.DUMMYUNIONNAME.ki.time == 0)
                    winEv.DUMMYUNIONNAME.ki.time = time(NULL);

                if((winEv.DUMMYUNIONNAME.ki.dwFlags & KEYEVENTF_KEYUP) == 0) {
                    sdlEv.key.type = SDL_KEYDOWN;
                    sdlEv.key.state = SDL_PRESSED;
                } else {
                    sdlEv.key.type = SDL_KEYUP;
                    sdlEv.key.state = SDL_RELEASED;
                }

                sdlEv.key.timestamp = winEv.DUMMYUNIONNAME.ki.time;
                sdlEv.key.windowID = 0;
                sdlEv.key.state = 0;
                sdlEv.key.keysym.sym = toSDLKeycode(winEv.DUMMYUNIONNAME.ki.wVk);
                sdlEv.key.keysym.scancode = SDL_GetScancodeFromKey(sdlEv.key.keysym.sym);
                sdlEv.key.keysym.mod = 0;

                SDL_PushEvent(&sdlEv);
                count++;
                break;
            }

            default:
                warn() << "Skipping event of type " << winEv.type;
                continue;
        }
    }

    return count;
}

LONG user32_SetWindowLong(HWND hWnd, int nIndex, LONG dwNewLong) {
    return user32_SetWindowLongA(hWnd, nIndex, dwNewLong);
}

LONG user32_SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong) {
    return user32_SetWindowLongPtrA(hWnd, nIndex, dwNewLong);
}

LONG_PTR user32_SetWindowLongPtr(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    return user32_SetWindowLongPtrA(hWnd, nIndex, dwNewLong);
}

LONG_PTR user32_SetWindowLongPtrA(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    debug() << "Requested to change parameter " << nIndex << " of window " << hWnd << " to value " << dwNewLong << ".";

    switch(nIndex) {
        default:
            warn() << "Ignored window parameter " << nIndex << " change request of window " << hWnd << ".";
            return FALSE;
    }
}

BOOL user32_SetWindowPos(HWND hWnd,  HWND hWndInsertAfter,  int  X,  int  Y,  int  cx,  int  cy,  UINT uFlags) {
    constexpr UINT SWP_NOSIZE           = 0x0001;
    constexpr UINT SWP_NOMOVE           = 0x0002;
    constexpr UINT SWP_NOZORDER         = 0x0004;
    constexpr UINT SWP_DRAWFRAME        = 0x0020;
    constexpr UINT SWP_SHOWWINDOW       = 0x0040;
    constexpr UINT SWP_HIDEWINDOW       = 0x0080;

    if(!hWnd)
        return FALSE;

    debug() << "Modifying position of window " << hWnd << ".";

    auto display = X11::GetDisplay();
    auto window = reinterpret_cast<Window>(hWnd);
    auto sibling = reinterpret_cast<Window>(hWndInsertAfter);

    unsigned int valueMask = 0;
    XWindowChanges xWinChangeStruct;

    if((uFlags & SWP_NOSIZE) == 0) {
        valueMask |= (CWWidth | CWHeight);

        xWinChangeStruct.width  = cx;
        xWinChangeStruct.height = cy;

        trace() << "New size of window " << hWnd << " will be " << cx << "x" << cy << ".";
    }

    if((uFlags & SWP_NOMOVE) == 0) {
        valueMask |= (CWX | CWY);

        X11::GetPosRelativeToParent(display, window, xWinChangeStruct.x, xWinChangeStruct.y);

        xWinChangeStruct.x += X;
        xWinChangeStruct.y += Y;

        trace() << "New position of window " << hWnd << " will be (" << cx << ", " << cy << ").";
    }

    if((uFlags & SWP_NOZORDER) == 0) {
        valueMask |= (CWStackMode);

        switch(sibling) {
            case(Window)-2:  // HWND_NOTOPMOST
                trace() << "Window " << hWnd << " will be located Below.";
                xWinChangeStruct.stack_mode = Below;
                break;

            case(Window)-1:  //HWND_TOPMOST
                trace() << "Window " << hWnd << " will be located Above.";
                xWinChangeStruct.stack_mode = Above;
                break;

            case 0: // HWND_TOP
                trace() << "Window " << hWnd << " will be located Above.";
                xWinChangeStruct.stack_mode = Above;
                break;

            case 1: // HWND_BOTTOM
                trace() << "Window " << hWnd << " will be located Below.";
                xWinChangeStruct.stack_mode = Below;
                break;

            default:
                valueMask |= (CWSibling);

                xWinChangeStruct.sibling = sibling;
                xWinChangeStruct.stack_mode = Above;

                trace() << "Window " << hWnd << " will be located Above sibling " << hWndInsertAfter << ".";
                break;
        }
    }

    if(!X11::ConfigureWindow(display, window, valueMask, xWinChangeStruct)) {
        error() << "Failed to move window " << hWnd << ".";
        return FALSE;
    } else {
        return TRUE;
    }
}

BOOL user32_ShowWindow(HWND hWnd, int nCmdShow) {
    constexpr int SW_FORCEMINIMIZE   = 11;
    constexpr int SW_HIDE            = 0;
    constexpr int SW_MAXIMIZE        = 3;
    constexpr int SW_MINIMIZE        = 6;
    constexpr int SW_RESTORE         = 9;
    constexpr int SW_SHOW            = 5;
    constexpr int SW_SHOWDEFAULT     = 10;
    constexpr int SW_SHOWMAXIMIZED   = 3;
    constexpr int SW_SHOWMINIMIZED   = 2;
    constexpr int SW_SHOWMINNOACTIVE = 7;
    constexpr int SW_SHOWNA          = 8;
    constexpr int SW_SHOWNOACTIVATE  = 4;
    constexpr int SW_SHOWNORMAL      = 1;

    debug() << "Showing window " << hWnd << " using method " << nCmdShow << ".";

    if(!hWnd) {
        warn() << "Passed a null window. Returning.";
        return FALSE;
    }

    // TODO
    warn() << "Ignoring show request.";
    return FALSE;
}

BOOL user32_SystemParametersInfo(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni) {
    return user32_SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);
}

BOOL user32_SystemParametersInfoA(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni) {
    switch(uiAction) {
        case 0x0030: {  // SPI_GETWORKAREA
            debug() << "Requested parameter SPI_GETWORKAREA";

            auto display = X11::GetDisplay();

            unsigned int w, h;
            PRECT pRect = reinterpret_cast<PRECT>(pvParam);
            pRect->left = 0;
            pRect->top  = 0;

            if(!X11::GetScreenSize(X11::GetDefaultScreen(display), w, h)) {
                error() << "Failed to obtain size of desktop window.";
                return FALSE;
            }

            pRect->right  = w;
            pRect->bottom = h;

            debug() << "Desktop window has size " << w << "x" << h << ".";

            return TRUE;
        }
    }

    warn() << "Ignored system parameter " << uiAction << " request.";
    return FALSE;
}

BOOL user32_UpdateWindow(HWND hWnd) {
    return FALSE;
}

