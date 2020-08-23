#include "user32.h"

#include <unistd.h>

#include <cstring>

#include <iostream>
#include <map>

#include <SDL.h>

#include "log.h"
#include "mkxpGlue.h"
#include "vkcodes.h"

namespace {
    inline HWND toHWND(SDL_Window* win) {
        return newHANDLE<HWND>(reinterpret_cast<void*>(win));
    }

    inline SDL_Window* fromHWND(HWND hwnd) {
        return reinterpret_cast<SDL_Window*>(fromHANDLE(hwnd));
    }

    enum WindowStyles : uint32_t {
        WS_BORDER = 0x00800000,
        WS_CAPTION = 0x00C00000,
        WS_ICONIC = 0x20000000,
        WS_MAXIMIZE = 0x01000000,
        WS_MAXIMIZEBOX = 0x00010000,
        WS_MINIMIZE = 0x20000000,
        WS_MINIMIZEBOX = 0x00020000,
        WS_OVERLAPPED = 0x00000000,
        WS_SIZEBOX = 0x00040000,
        WS_SYSMENU = 0x00080000,
        WS_THICKFRAME = 0x00040000,
        WS_TILED = 0x00000000,
        WS_VISIBLE = 0x10000000,
        WS_OVERLAPPEDWINDOW = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        WS_TILEDWINDOW = WS_TILED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
    };

    LONG_PTR getStyle(SDL_Window* win) {
        LONG_PTR style = 0;
        auto sdlFlags = SDL_GetWindowFlags(win);

        if(sdlFlags & SDL_WINDOW_SHOWN)
            style |= WS_VISIBLE;

        if(sdlFlags & SDL_WINDOW_MINIMIZED)
            style |= WS_MINIMIZE;

        if(sdlFlags & SDL_WINDOW_MAXIMIZED)
            style |= WS_MAXIMIZE;

        if(!(sdlFlags & SDL_WINDOW_BORDERLESS))
            style |= WS_OVERLAPPEDWINDOW;

        return style;
    }

    LONG_PTR changeStyle(SDL_Window* win, LONG_PTR newStyle) {
        auto oldStyle = getStyle(win);

//        if(newStyle & WS_OVERLAPPED || newStyle & WS_CAPTION
//            || newStyle & WS_SYSMENU || newStyle & WS_THICKFRAME
//            || newStyle & WS_MINIMIZEBOX || newStyle & WS_MAXIMIZEBOX || newStyle & WS_BORDER)
        if(newStyle & WS_BORDER)
            SDL_SetWindowBordered(win, SDL_TRUE);
        else
            SDL_SetWindowBordered(win, SDL_FALSE);

        if(newStyle & WS_MAXIMIZE)
            SDL_MaximizeWindow(win);

        if(newStyle & WS_MINIMIZE)
            SDL_MinimizeWindow(win);

        if(newStyle & WS_VISIBLE)
            SDL_ShowWindow(win);

        return oldStyle;
    }
}

namespace fmt {
    template<>
    struct formatter<POINT> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext& ctx) {
            return ctx.begin();
        }

        template<typename FormatContext>
        auto format(const POINT& p, FormatContext& ctx) {
            return format_to(ctx.out(), "({}, {})", p.x, p.y);
        }
    };

    template<>
    struct formatter<RECT> {
        template<typename ParseContext>
        constexpr auto parse(ParseContext& ctx) {
            return ctx.begin();
        }

        template<typename FormatContext>
        auto format(const RECT& p, FormatContext& ctx) {
            return format_to(ctx.out(), "[({}, {}) -> ({}, {})]", p.left, p.top, p.right, p.bottom);
        }
    };
}

WIN32_API BOOL user32_ClientToScreen(HWND hWnd, LPPOINT lpPoint) {
  SPDLOG_TRACE("user32::ClientToScreen(hWnd={}, *lpPoint={})", hWnd, *lpPoint);

  if(!hWnd) {
    spdlog::warn("Ignoring null window");
    return FALSE;
  }

  auto win = fromHWND(hWnd);

  int x{0}, y{0};

  SDL_GetWindowPosition(win, &x, &y);

  lpPoint->x += x;
  lpPoint->y += y;

  SPDLOG_TRACE(" <- user32::ClientToScreen(hWnd={}, *lpPoint={})", hWnd, *lpPoint);
  return TRUE;
}

WIN32_API HWND user32_CreateWindowEx(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int X, int Y,
                           int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    return user32_CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent,
                                  hMenu, hInstance, lpParam);
}

WIN32_API HWND user32_CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y,
                            int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                            LPVOID lpParam) {
    return toHWND(NULL);
}

WIN32_API SHORT user32_GetAsyncKeyState(WORD vKey) {
    return user32_GetKeyState(vKey);
}

WIN32_API BOOL user32_GetClientRect(HWND hWnd, PRECT lpRect) {
    SPDLOG_TRACE("user32::GetClientRect(hWnd={}, lpRect={})", (void*) (hWnd), (void*) (lpRect));

    if(!hWnd) {
        spdlog::warn("Ignoring null window");
        return FALSE;
    }

    auto win = fromHWND(hWnd);

    lpRect->left = 0;
    lpRect->top = 0;

    SDL_GL_GetDrawableSize(win, &lpRect->right, &lpRect->bottom);

    SPDLOG_TRACE(" <- user32::GetClientRect(hWnd={}, *lpRect={})", (void*) (hWnd), *lpRect);
    return TRUE;
}

WIN32_API BOOL user32_GetCursorPos(LPPOINT lpPoint) {
    SPDLOG_TRACE("user32::GetCursorPos(lpPoint={})", (void*) (lpPoint));

    SDL_GetMouseState(&lpPoint->x, &lpPoint->y);

    SPDLOG_TRACE(" <- user32::GetCursorPos(*lpPoint={})", *lpPoint);
    return TRUE;
}

WIN32_API HWND user32_GetDesktopWindow() {
    SPDLOG_TRACE("user32::GetDesktopWindow()");
    return toHWND(NULL);
}

WIN32_API SHORT user32_GetKeyState(WORD vKey) {
    switch(vKey) {
        case VK_LBUTTON:
        case VK_RBUTTON:
        case VK_MBUTTON:
        case VK_XBUTTON1:
        case VK_XBUTTON2: {
            int mouseState = SDL_GetGlobalMouseState(NULL, NULL);

            bool isButtonPressed = false;

            switch(vKey) {
                case VK_LBUTTON:
                    isButtonPressed = ((mouseState & SDL_BUTTON_LMASK) != 0);
                    break;

                case VK_RBUTTON:
                    isButtonPressed = ((mouseState & SDL_BUTTON_RMASK) != 0);
                    break;

                case VK_MBUTTON:
                    isButtonPressed = ((mouseState & SDL_BUTTON_MMASK) != 0);
                    break;

                case VK_XBUTTON1:
                    isButtonPressed = ((mouseState & SDL_BUTTON_X1MASK) != 0);
                    break;

                case VK_XBUTTON2:
                    isButtonPressed = ((mouseState & SDL_BUTTON_X2MASK) != 0);
                    break;
            }

//             return (isButtonPressed ? ~(SHORT{-1} >> 1) : 0);
            return SHORT(isButtonPressed ? -1 : 0);
        }

        default: {
            const auto* state = SDL_GetKeyboardState(NULL);

//             return (state[toSDLScancode(vKey)] ? ~(SHORT{-1} >> 1) : 0);
            return SHORT(state[toSDLScancode(vKey)] ? -1 : 0);
        }
    }
}

WIN32_API int user32_GetSystemMetrics(int nIndex) {
    SPDLOG_TRACE("user32::GetSystemMetrics(nIndex={})", nIndex);

    switch(nIndex) {
        case 0: {     // CXSCREEN
            SDL_Rect rect;

            if(SDL_GetDisplayBounds(SDL_GetWindowDisplayIndex(getMkxpWindow()), &rect) == 0)
                return rect.w;

            break;
        }

        case 1: {    // CYSCREEN
            SDL_Rect rect;

            if(SDL_GetDisplayBounds(SDL_GetWindowDisplayIndex(getMkxpWindow()), &rect) == 0)
                return rect.h;

            break;
        }

    }

    return 0;
}

WIN32_API BOOL user32_GetWindowRect(HWND hWnd, PRECT lpRect) {
    SPDLOG_TRACE("user32::GetWindowRect(hWnd={}, lpRect={})", (void*) (hWnd), (void*) (lpRect));

    if(!hWnd) {
        spdlog::warn("Ignoring null window");
        return FALSE;
    }

    auto* win = fromHWND(hWnd);

    SDL_GetWindowPosition(win, &lpRect->left, &lpRect->top);
    SDL_GetWindowSize(win, &lpRect->right, &lpRect->bottom);

    lpRect->right += lpRect->left;
    lpRect->bottom += lpRect->top;

    SPDLOG_TRACE(" <- user32::GetWindowRect(hWnd={}, *lpRect={})", (void*) (hWnd), *lpRect);
    return TRUE;
}

WIN32_API LONG user32_GetWindowLong(HWND hWnd, int nIndex) {
    return user32_GetWindowLongA(hWnd, nIndex);
}

WIN32_API LONG user32_GetWindowLongA(HWND hWnd, int nIndex) {
    constexpr int GWL_STYLE = -16;
    constexpr int GWL_USERDATA = -21;

    SPDLOG_TRACE("user32::GetWindowLongA(hWnd={}, nIndex={})", (void*)(hWnd), nIndex);

    if(!hWnd) {
        spdlog::warn("Ignoring null window");
        return FALSE;
    }

    auto win = fromHWND(hWnd);

    switch(nIndex) {
        case GWL_STYLE: {
            auto style = getStyle(win);
            return style;
        }

        case GWL_USERDATA: {
            auto userData = reinterpret_cast<LONG_PTR>(SDL_GetWindowData(win, "WIN32API_USER_WINDOW_DATA"));
            return userData;
        }

        default:
            spdlog::warn("Ignored request for parameter {} of window {}", nIndex, (void*)(fromHANDLE(hWnd)));
            return FALSE;
    }
}

WIN32_API HWND user32_FindWindow(LPCSTR lpClassName, LPCSTR lpWindowName) {
    return user32_FindWindowA(lpClassName, lpWindowName);
}

WIN32_API HWND user32_FindWindowA(LPCSTR lpClassName, LPCSTR lpWindowName) {
    if(lpWindowName) {
        SPDLOG_TRACE("user32::FindWindowA(lpClassName=\"{}\", lpWindowName=\"{}\")", lpClassName, lpWindowName);
    } else {
        SPDLOG_TRACE("user32::FindWindowA(lpClassName=\"{}\", lpWindowName=0x0", lpClassName);
    }

    if(std::strcmp(lpClassName, "RGSS Player") != 0) {
        spdlog::warn("Ignoring query because window class is not 'RGSS Player'");
        return toHWND(NULL);
    }

    return toHWND(getMkxpWindow());
}

WIN32_API int user32_MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType) {
    return user32_MessageBoxA(hWnd, lpText, lpCaption, uType);
}

WIN32_API int user32_MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    constexpr UINT MB_OK = 0x00000000;
    constexpr UINT MB_OKCANCEL = 0x00000001;
    constexpr UINT MB_ABORTRETRYIGNORE = 0x00000002;
    constexpr UINT MB_YESNOCANCEL = 0x00000003;
    constexpr UINT MB_YESNO = 0x00000004;
    constexpr UINT MB_RETRYCANCEL = 0x00000005;
    constexpr UINT MB_CANCELTRYCONTINUE = 0x00000006;

    constexpr int IDOK = 1;
    constexpr int IDCANCEL = 2;
    constexpr int IDABORT = 3;
    constexpr int IDRETRY = 4;
    constexpr int IDIGNORE = 5;
    constexpr int IDYES = 6;
    constexpr int IDNO = 7;
    constexpr int IDTRYAGAIN = 10;
    constexpr int IDCONTINUE = 11;

    SPDLOG_TRACE("user32::MessageBoxA(hWnd={}, lpText=\"{}\", lpCaption=\"{}\", uType={:x})", (void*) (hWnd), lpText,
                 lpCaption, uType);

    UINT dialogButtonsFlag = (uType & 0x0000000f);
    UINT dialogTypeFlag = (uType & 0x000000f0);

    int numbuttons = 0;
    SDL_MessageBoxButtonData buttons[3];

    switch(dialogButtonsFlag) {
        case MB_OK:
            numbuttons = 1;
            buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT | SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[0].buttonid = IDOK;
            buttons[0].text = "Ok";
            break;

        case MB_OKCANCEL:
            numbuttons = 2;
            buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].buttonid = IDOK;
            buttons[0].text = "Ok";
            buttons[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[1].buttonid = IDCANCEL;
            buttons[1].text = "Cancel";
            break;

        case MB_ABORTRETRYIGNORE:
            numbuttons = 3;
            buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].buttonid = IDABORT;
            buttons[0].text = "Abort";
            buttons[1].flags = 0;
            buttons[1].buttonid = IDRETRY;
            buttons[1].text = "Retry";
            buttons[2].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[2].buttonid = IDIGNORE;
            buttons[2].text = "Ignore";
            break;

        case MB_YESNOCANCEL:
            numbuttons = 3;
            buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].buttonid = IDYES;
            buttons[0].text = "Yes";
            buttons[1].flags = 0;
            buttons[1].buttonid = IDNO;
            buttons[1].text = "No";
            buttons[2].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[2].buttonid = IDCANCEL;
            buttons[2].text = "Cancel";
            break;

        case MB_YESNO:
            numbuttons = 2;
            buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].buttonid = IDYES;
            buttons[0].text = "Yes";
            buttons[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[1].buttonid = IDNO;
            buttons[1].text = "No";
            break;

        case MB_RETRYCANCEL:
            numbuttons = 2;
            buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].buttonid = IDRETRY;
            buttons[0].text = "Retry";
            buttons[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[1].buttonid = IDCANCEL;
            buttons[1].text = "Cancel";
            break;

        case MB_CANCELTRYCONTINUE:
            numbuttons = 3;
            buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].buttonid = IDCANCEL;
            buttons[0].text = "Cancel";
            buttons[1].flags = 0;
            buttons[1].buttonid = IDTRYAGAIN;
            buttons[1].text = "Try Again";
            buttons[2].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[2].buttonid = IDCONTINUE;
            buttons[2].text = "Continue";
            break;

    }

    Uint32 flags = 0;

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
    mbData.flags = flags;
    mbData.window = NULL;
    mbData.title = lpCaption;
    mbData.message = lpText;
    mbData.numbuttons = numbuttons;
    mbData.buttons = buttons;
    mbData.colorScheme = NULL;

    int retVal;

    if(SDL_ShowMessageBox(&mbData, &retVal) != 0)
        spdlog::error("SDL_ShowMessageBox failed: {}", SDL_GetError());

    return retVal;
}

WIN32_API BOOL user32_MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint) {
    SPDLOG_TRACE("user32::MoveWindow(hWnd={}, X={}, Y={}, nWidth={}, nHeight={}, bRepaint={})", (void*) (hWnd), X, Y,
                 nWidth, nHeight, bool(bRepaint));

    if(!hWnd) {
        spdlog::warn("Ignoring null window");
        return FALSE;
    }

    auto win = fromHWND(hWnd);

    SDL_SetWindowPosition(win, X, Y);
    SDL_SetWindowSize(win, nWidth, nHeight);

    if(bRepaint)
        SDL_UpdateWindowSurface(win);

    return TRUE;
}

WIN32_API BOOL user32_ScreenToClient(HWND hWnd, LPPOINT lpPoint) {
    SPDLOG_TRACE("user32::ScreenToClient(hWnd={}, *lpPoint={})", hWnd, *lpPoint);

    if(!hWnd) {
        spdlog::warn("Ignoring null window");
        return FALSE;
    }

    auto win = fromHWND(hWnd);

    int x{0}, y{0};

    SDL_GetWindowPosition(win, &x, &y);

    lpPoint->x -= x;
    lpPoint->y -= y;

    SPDLOG_TRACE(" <- user32::ScreenToClient(hWnd={}, *lpPoint={})", hWnd, *lpPoint);
    return TRUE;
}

WIN32_API UINT user32_SendInput(UINT cInputs, LPINPUT pInputs, int cbSize) {
    constexpr DWORD INPUT_MOUSE = 0;
    constexpr DWORD INPUT_KEYBOARD = 1;
//     constexpr DWORD INPUT_HARDWARE  = 2;

    SPDLOG_TRACE("user32::SendInput(cInputs={}, pInputs={}, cbSize={})", cInputs, (void*) (pInputs), cbSize);

    if(cbSize != sizeof(INPUT)) {
        spdlog::error("Size of INPUT type does not match ({} != {})", sizeof(INPUT), cbSize);
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
                spdlog::warn("Skipping MOUSEINPUT event");
                break;
            }

            case INPUT_KEYBOARD: {
                constexpr DWORD KEYEVENTF_EXTENDEDKEY = 0x0001;
                constexpr DWORD KEYEVENTF_KEYUP = 0x0002;
                constexpr DWORD KEYEVENTF_SCANCODE = 0x0008;
                constexpr DWORD KEYEVENTF_UNICODE = 0x0004;

                if((winEv.DUMMYUNIONNAME.ki.dwFlags & KEYEVENTF_UNICODE) != 0) {
                    spdlog::warn("Skipping unicode KEYBDINPUT event");
                    break;
                }

                if((winEv.DUMMYUNIONNAME.ki.dwFlags & KEYEVENTF_SCANCODE) != 0) {
                    spdlog::warn("Skipping scan code KEYBDINPUT event");
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
                spdlog::warn("Skipping event of type {}", winEv.type);
                continue;
        }
    }

    return count;
}

BOOL user32_SetCursorPos(int X, int Y) {
  SDL_WarpMouseInWindow(getMkxpWindow(), X, Y);
  return TRUE;
}

WIN32_API LONG user32_SetWindowLong(HWND hWnd, int nIndex, LONG dwNewLong) {
    return user32_SetWindowLongA(hWnd, nIndex, dwNewLong);
}

WIN32_API LONG user32_SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong) {
    return user32_SetWindowLongPtrA(hWnd, nIndex, dwNewLong);
}

WIN32_API LONG_PTR user32_SetWindowLongPtr(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    return user32_SetWindowLongPtrA(hWnd, nIndex, dwNewLong);
}

WIN32_API LONG_PTR user32_SetWindowLongPtrA(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    constexpr int GWL_STYLE = -16;
    constexpr int GWL_USERDATA = -21;

    SPDLOG_TRACE("user32::SetWindowLongPtrA(hWnd={}, nIndex={}, dwNewLong={})", (void*) (hWnd), nIndex, dwNewLong);

    if(!hWnd) {
        spdlog::warn("Ignoring null window");
        return FALSE;
    }

    auto win = fromHWND(hWnd);

    switch(nIndex) {
        case GWL_STYLE:
            return changeStyle(win, dwNewLong);

        case GWL_USERDATA:
            return reinterpret_cast<LONG_PTR>(SDL_SetWindowData(win, "WIN32API_USER_WINDOW_DATA",
                                                                reinterpret_cast<void*>(dwNewLong)));

        default:
            spdlog::warn("Ignoring window parameter {} change request for window {}", nIndex, (void*) fromHANDLE(hWnd));
            return FALSE;
    }
}

WIN32_API BOOL user32_SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
    constexpr UINT SWP_NOSIZE = 0x0001;
    constexpr UINT SWP_NOMOVE = 0x0002;
    constexpr UINT SWP_NOZORDER = 0x0004;
    constexpr UINT SWP_DRAWFRAME = 0x0020;
    constexpr UINT SWP_SHOWWINDOW = 0x0040;
    constexpr UINT SWP_HIDEWINDOW = 0x0080;

    SPDLOG_TRACE("user32::SetWindowPos(hWnd={}, hWndInsertAfter={}, X={}, Y={}, cx={}, cy={}, uFlags={:x})",
                 (void*) (hWnd), (void*) (hWndInsertAfter), X, Y, cx, cy, uFlags);

    if(!hWnd) {
        spdlog::warn("Ignoring null window");
        return FALSE;
    }

    auto win = fromHWND(hWnd);

    if((uFlags & SWP_NOSIZE) == 0) {
        SDL_SetWindowSize(win, cx, cy);

        SPDLOG_DEBUG("Changed size of window {} to {}x{}", (void*) (hWnd), cx, cy);
    }

    if((uFlags & SWP_NOMOVE) == 0) {
        SDL_SetWindowPosition(win, X, Y);

        SPDLOG_DEBUG("Changed position of window {} to {}x{}", (void*) (hWnd), X, Y);
    }

    if((uFlags & SWP_NOZORDER) == 0) {
//         valueMask |= (CWStackMode);
//
//         switch(sibling) {
//             case(Window)-2:  // HWND_NOTOPMOST
//                 trace() << "Window " << hWnd << " will be located Below.";
//                 xWinChangeStruct.stack_mode = Below;
//                 break;
//
//             case(Window)-1:  //HWND_TOPMOST
//                 trace() << "Window " << hWnd << " will be located Above.";
//                 xWinChangeStruct.stack_mode = Above;
//                 break;
//
//             case 0: // HWND_TOP
//                 trace() << "Window " << hWnd << " will be located Above.";
//                 xWinChangeStruct.stack_mode = Above;
//                 break;
//
//             case 1: // HWND_BOTTOM
//                 trace() << "Window " << hWnd << " will be located Below.";
//                 xWinChangeStruct.stack_mode = Below;
//                 break;
//
//             default:
//                 valueMask |= (CWSibling);
//
//                 xWinChangeStruct.sibling = sibling;
//                 xWinChangeStruct.stack_mode = Above;
//
//                 trace() << "Window " << hWnd << " will be located Above sibling " << hWndInsertAfter << ".";
//                 break;
//         }
    }

    return TRUE;
}

WIN32_API int user32_ShowCursor(BOOL bShow) {
    SPDLOG_TRACE("user32::ShowCursor(bShow={})", bool(bShow));

    int cState = SDL_ShowCursor(bShow == TRUE ? SDL_ENABLE : SDL_DISABLE);

    return (cState == SDL_ENABLE ? 1 : -1);
}

WIN32_API BOOL user32_ShowWindow(HWND hWnd, int nCmdShow) {
    constexpr int SW_FORCEMINIMIZE = 11;
    constexpr int SW_HIDE = 0;
    constexpr int SW_MAXIMIZE = 3;
    constexpr int SW_MINIMIZE = 6;
    constexpr int SW_RESTORE = 9;
    constexpr int SW_SHOW = 5;
    constexpr int SW_SHOWDEFAULT = 10;
    constexpr int SW_SHOWMINIMIZED = 2;
    constexpr int SW_SHOWMINNOACTIVE = 7;
    constexpr int SW_SHOWNA = 8;
    constexpr int SW_SHOWNOACTIVATE = 4;
    constexpr int SW_SHOWNORMAL = 1;

    SPDLOG_TRACE("user32::ShowWindow(hWnd={}, nCmdShow={:x})", (void*)(hWnd), nCmdShow);

    if(!hWnd) {
        spdlog::warn("Ignoring null window");
        return FALSE;
    }

    auto win = fromHWND(hWnd);
    bool wasVisible = (SDL_GetWindowFlags(win) & SDL_WINDOW_SHOWN);

    switch(nCmdShow) {
        case SW_HIDE:
            SDL_HideWindow(win);
            break;

        case SW_FORCEMINIMIZE:
        case SW_MINIMIZE:
        case SW_SHOWMINIMIZED:
            SDL_MinimizeWindow(win);
            break;

        case SW_MAXIMIZE:
            SDL_MaximizeWindow(win);
            break;

        case SW_RESTORE:
            SDL_RestoreWindow(win);
            break;

        case SW_SHOW:
        case SW_SHOWNA:
        case SW_SHOWNOACTIVATE:
        case SW_SHOWNORMAL:
            SDL_ShowWindow(win);
            break;
    }

    spdlog::warn("Ignoring show request");
    return wasVisible;
}

WIN32_API BOOL user32_SystemParametersInfo(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni) {
    return user32_SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);
}

WIN32_API BOOL user32_SystemParametersInfoA(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni) {
    SPDLOG_TRACE("user32::SystemParametersInfoA(uiAction={}, uiParam={}, pvParam={}, fWinIni={})", uiAction, uiParam,
                 pvParam, fWinIni);

    switch(uiAction) {
        case 0x0030: {  // SPI_GETWORKAREA
            SDL_Rect rect;

            if(SDL_GetDisplayBounds(SDL_GetWindowDisplayIndex(getMkxpWindow()), &rect) != 0) {
                spdlog::error("Failed to obtain desktop size");
                return FALSE;
            }

            auto pRect = reinterpret_cast<PRECT>(pvParam);
            pRect->left = 0;
            pRect->top = 0;
            pRect->right = rect.w;
            pRect->bottom = rect.h;

            SPDLOG_TRACE(" <- user32::SystemParametersInfoA(uiAction={}, uiParam={}, *pvParam:RECT={}, fWinIni={})",
                         uiAction, uiParam, *pRect, fWinIni);
            return TRUE;
        }
    }

    spdlog::warn("Ignored system parameter {} request", uiAction);
    return FALSE;
}

WIN32_API BOOL user32_UpdateWindow(HWND hWnd) {
    SPDLOG_TRACE("user32::UpdateWindow(hWnd={})", (void*) (hWnd));

    if(!hWnd) {
        spdlog::warn("Ignoring null window");
        return FALSE;
    }

    return (SDL_UpdateWindowSurface(fromHWND(hWnd)) == 0);
}


