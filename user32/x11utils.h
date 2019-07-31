#ifndef X11UTILS_H
#define X11UTILS_H

#include <functional>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#ifdef __cplusplus
}
#endif

namespace X11 {
    bool ConfigureWindow(Display* d, Window w, unsigned int vMask, XWindowChanges& xWinChangeStruct);

    template<class Pred>
    std::vector<Window> FindWindows(Display* d, Window w, const Pred& pred);

    inline bool Free(void* ptr) {
        return (ptr ? (XFree(ptr) == Success) : false);
    }

    Atom GetAtom(Display* d, const std::string& propName);

    Screen* GetDefaultScreen(Display* d);

    Display* GetDisplay(char* displayName = NULL);

    std::string GetErrorText(Display* d, int code);

    std::string GetDetailedErrorText(Display* d, int code);

    bool GetPosRelativeToParent(Display* d, Window w, int& x, int& y);

    bool GetPosRelativeToScreen(Display* d, Window w, int& x, int& y);

    Window GetRootWindow(Display* d);

    Window GetRootWindowFromWindow(Display* d, Window w);

    bool GetScreenSize(Screen* s, unsigned int& width, unsigned int& height);

    bool GetSize(Display* d, Window w, unsigned int& width, unsigned int& height);

    std::vector<Window> GetWindowChildren(Display* d, Window w);

    std::string GetWindowName(Display* d, Window w);

    Window GetWindowParent(Display* d, Window w);

    __pid_t GetWindowPID(Display* d, Window w);
}

template<typename Pred>
std::vector<Window> X11::FindWindows(Display* d, Window w, const Pred& pred) {
    std::vector<Window> results;

    auto fn_search = [&](Window win, auto& fn_this) -> void {
        if(pred(d, win))
            results.push_back(win);

        // Recurse into child windows.
        for(Window child : X11::GetWindowChildren(d, win))
            fn_this(child, fn_this);
    };

    fn_search(w, fn_search);

    return results;
}

#endif


