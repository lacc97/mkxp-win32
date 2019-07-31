#include "x11utils.h"

#include <functional>
#include <map>

#include "log.h"

// class X11ErrorHandler {
// public:
//     typedef int(*handler_type)(Display*, XErrorEvent*);
//     
//     static int handler(Display* d, XErrorEvent* ev) {
//         error() << "X error on display '" << XDisplayString(d) <<  "': (serial: " << ev->serial << ", error code: "<< int(ev->error_code) << ", request code: " << int(ev->request_code) << ", minor code: " << int(ev->minor_code) << "resource id: " << ev->resourceid << ")";
//         
//         s_PrevHandler(d, ev);
//         
//         return True;
//     }
//     
//     X11ErrorHandler() {
//         debug() << "Set new X11 error handler";
//         s_PrevHandler = XSetErrorHandler(handler);
//     }
//     
// private:
//     static handler_type s_PrevHandler;
// };
// 
// X11ErrorHandler::handler_type X11ErrorHandler::s_PrevHandler = nullptr;
// X11ErrorHandler x11ErrorHandler;

void printXlibError(Display* display, int errorcode) {
    std::string errorString = X11::GetErrorText(display, errorcode);

    if(!errorString.empty())
        error() << "Error " << errorcode << " on display '" << XDisplayString(display) << "': " << errorString;
}

template<typename ReturnType = bool>
inline ReturnType
checkX11Error(Display* display, int returnCode, ReturnType successReturnValue = true, ReturnType returnValue = false,
              const std::function<void(void)>& fExtra = []() {}) {
#ifdef NDEBUG

    if(false) {
#endif

        if(returnCode != Success) {
            printXlibError(display, returnCode);
            fExtra();
            return returnValue;
        }

#ifdef NDEBUG
    }

#endif

    return successReturnValue;
}

bool X11::ConfigureWindow(Display* d, Window w, unsigned int vMask, XWindowChanges& xWinChangeStruct) {
    int retcode = XConfigureWindow(d, w, vMask, &xWinChangeStruct);

    return checkX11Error(d, retcode);
}

std::map<Display*, std::map<std::string, Atom>> s_AtomMap;

Atom X11::GetAtom(Display* d, const std::string& propName) {
    try {
        auto& atomMap = s_AtomMap.at(d);

        try {
            return atomMap.at(propName);
        } catch(std::out_of_range& ex) {
            auto atom = XInternAtom(d, propName.c_str(), True);

            if(atom == None) {
                error() << "Atom '" << propName << "' not found.";
                return None;
            }

            atomMap[propName] = atom;
            return atom;
        }
    } catch(std::out_of_range& ex) {
        s_AtomMap[d] = std::map<std::string, Atom>();
        return GetAtom(d, propName);
    }
}

Screen* X11::GetDefaultScreen(Display* d) {
    return XDefaultScreenOfDisplay(d);
}

Display* X11::GetDisplay(char* displayName) {
    return XOpenDisplay(displayName);
}

std::string X11::GetErrorText(Display* d, int code) {
    std::string errorBuf(4096, '\0');

    // Guaranteed by C++ 11.
    if(XGetErrorText(d, code, const_cast<char*>(errorBuf.data()), errorBuf.capacity()) != Success) {
        error() << "Failed to retrieve Xlib error.";
        errorBuf.resize(0);
    } else {
        errorBuf.resize(std::strlen(errorBuf.c_str()));
    }

    return errorBuf;
}

bool X11::GetPosRelativeToParent(Display* d, Window w, int& x, int& y) {
    Window wRoot;
    unsigned int wWidth, wHeight;
    unsigned int wBorderWidth;
    unsigned int wDepthReturn;

    int retcode = XGetGeometry(d, w, &wRoot, &x, &y, &wWidth, &wHeight, &wBorderWidth, &wDepthReturn);

    return checkX11Error(d, retcode);
}

bool X11::GetPosRelativeToScreen(Display* d, Window w, int& x, int& y) {
    Window child;
    Window root = GetRootWindowFromWindow(d, w);

    if(root == 0)
        return false;

    int retcode = XTranslateCoordinates(d, w, root, 0, 0, &x, &y, &child);

    return checkX11Error(d, retcode);
}

Window X11::GetRootWindow(Display* d) {
    return XDefaultRootWindow(d);
}

Window X11::GetRootWindowFromWindow(Display* d, Window w) {
    Window wRoot, wParent, * wChildren = NULL;
    unsigned int nChildren;

    int retcode = XQueryTree(d, w, &wRoot, &wParent, &wChildren, &nChildren);
    X11::Free(wChildren);

    return checkX11Error<Window>(d, retcode, wRoot, 0);
}

bool X11::GetScreenSize(Screen* s, unsigned int& width, unsigned int& height) {
    width = XWidthOfScreen(s);
    height = XHeightOfScreen(s);

    return true;
}

bool X11::GetSize(Display* d, Window w, unsigned int& width, unsigned int& height) {
    XWindowAttributes winAttrib;

    int retcode = XGetWindowAttributes(d, w, &winAttrib);

    width = winAttrib.width;
    height = winAttrib.height;

    return checkX11Error(d, retcode);
}

std::vector<Window> X11::GetWindowChildren(Display* d, Window w) {
    Window wRoot;
    Window wParent;
    Window* wChild = NULL;
    unsigned int nChildren;

    int retcode = XQueryTree(d, w, &wRoot, &wParent, &wChild, &nChildren);

    std::vector<Window> children;

    if(wChild) {
        children.reserve(nChildren);

        for(size_t ii = 0; ii < nChildren; ii++)
            children.push_back(wChild[ii]);

        XFree(wChild);
    }

    return checkX11Error(d, retcode, children, children);
}

std::string X11::GetWindowName(Display* d, Window w) {
    auto atomName = GetAtom(d, "_NET_WM_NAME");

    if(atomName == None)
        return "";

    Atom type;
    int format;
    unsigned long nItems;
    unsigned long bytesAfter;
    char* name = NULL;

    int retcode = XGetWindowProperty(d, w, atomName, 0, 1, False, XA_STRING, &type, &format, &nItems, &bytesAfter,
                                     reinterpret_cast<unsigned char**>(&name));

    std::string nameStr;

    if(name) {
        nameStr = name;
        XFree(name);
    }

    return checkX11Error<std::string>(d, retcode, nameStr, "");
}

__pid_t X11::GetWindowPID(Display* d, Window w) {
    auto atomPid = GetAtom(d, "_NET_WM_PID");

    if(atomPid == None)
        return -1;

    Atom type;
    int format;
    unsigned long nItems;
    unsigned long bytesAfter;
    unsigned char* propPid = NULL;

    int retcode = XGetWindowProperty(d, w, atomPid, 0, 1, False, XA_CARDINAL, &type, &format, &nItems, &bytesAfter,
                                     &propPid);

    __pid_t pid = -1;

    if(propPid) {
        pid = *reinterpret_cast<__pid_t*>(propPid);
        XFree(propPid);
    }

    return checkX11Error<__pid_t>(d, retcode, pid, -1);
}
