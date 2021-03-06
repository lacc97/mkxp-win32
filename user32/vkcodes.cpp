#include <SDL.h>
#include "vkcodes.h"

#define VK_CASE_X(vkname, sdlname) case VK_##vkname: return SDLK_##sdlname
#define VK_CASE(name) VK_CASE_X(name, name)

SDL_Keycode toSDLKeycode(WORD vk) {
    switch(vk) {
        VK_CASE(CANCEL);
        VK_CASE_X(BACK, BACKSPACE);
        VK_CASE(TAB);
        VK_CASE(CLEAR);
        VK_CASE(RETURN);
        VK_CASE_X(SHIFT, LSHIFT);
        VK_CASE_X(CONTROL, LCTRL);
        VK_CASE(MENU);
        VK_CASE(PAUSE);
        VK_CASE_X(CAPITAL, CAPSLOCK);
        VK_CASE(ESCAPE);
        VK_CASE_X(MODECHANGE, MODE);
        VK_CASE(SPACE);
        VK_CASE(PRIOR);
        VK_CASE(END);
        VK_CASE(HOME);
        VK_CASE(LEFT);
        VK_CASE(UP);
        VK_CASE(RIGHT);
        VK_CASE(DOWN);
        VK_CASE(SELECT);
        VK_CASE(EXECUTE);
        VK_CASE_X(SNAPSHOT, PRINTSCREEN);
        VK_CASE(INSERT);
        VK_CASE(DELETE);
        VK_CASE(HELP);
        VK_CASE(0);
        VK_CASE(1);
        VK_CASE(2);
        VK_CASE(3);
        VK_CASE(4);
        VK_CASE(5);
        VK_CASE(6);
        VK_CASE(7);
        VK_CASE(8);
        VK_CASE(9);
        VK_CASE(a);
        VK_CASE(b);
        VK_CASE(c);
        VK_CASE(d);
        VK_CASE(e);
        VK_CASE(f);
        VK_CASE(g);
        VK_CASE(h);
        VK_CASE(i);
        VK_CASE(j);
        VK_CASE(k);
        VK_CASE(l);
        VK_CASE(m);
        VK_CASE(n);
        VK_CASE(o);
        VK_CASE(p);
        VK_CASE(q);
        VK_CASE(r);
        VK_CASE(s);
        VK_CASE(t);
        VK_CASE(u);
        VK_CASE(v);
        VK_CASE(w);
        VK_CASE(x);
        VK_CASE(y);
        VK_CASE(z);
        VK_CASE_X(APPS, APPLICATION);
        VK_CASE(SLEEP);
        VK_CASE_X(NUMPAD0, KP_0);
        VK_CASE_X(NUMPAD1, KP_1);
        VK_CASE_X(NUMPAD2, KP_2);
        VK_CASE_X(NUMPAD3, KP_3);
        VK_CASE_X(NUMPAD4, KP_4);
        VK_CASE_X(NUMPAD5, KP_5);
        VK_CASE_X(NUMPAD6, KP_6);
        VK_CASE_X(NUMPAD7, KP_7);
        VK_CASE_X(NUMPAD8, KP_8);
        VK_CASE_X(NUMPAD9, KP_9);
        VK_CASE_X(MULTIPLY, KP_MULTIPLY);
        VK_CASE_X(ADD, KP_PLUS);
        VK_CASE_X(SEPARATOR, KP_ENTER);
        VK_CASE_X(SUBTRACT, KP_MINUS);
        VK_CASE_X(DECIMAL, KP_PERIOD);
        VK_CASE_X(DIVIDE, KP_DIVIDE);
        VK_CASE(F1);
        VK_CASE(F2);
        VK_CASE(F3);
        VK_CASE(F4);
        VK_CASE(F5);
        VK_CASE(F6);
        VK_CASE(F7);
        VK_CASE(F8);
        VK_CASE(F9);
        VK_CASE(F10);
        VK_CASE(F11);
        VK_CASE(F12);
        VK_CASE(F13);
        VK_CASE(F14);
        VK_CASE(F15);
        VK_CASE(F16);
        VK_CASE(F17);
        VK_CASE(F18);
        VK_CASE(F19);
        VK_CASE(F20);
        VK_CASE(F21);
        VK_CASE(F22);
        VK_CASE(F23);
        VK_CASE(F24);
        VK_CASE_X(NUMLOCK, NUMLOCKCLEAR);
        VK_CASE_X(SCROLL, SCROLLLOCK);
        VK_CASE(LSHIFT);
        VK_CASE(RSHIFT);
        VK_CASE_X(LCONTROL, LCTRL);
        VK_CASE_X(RCONTROL, RCTRL);
        VK_CASE_X(BROWSER_BACK, AC_BACK);
        VK_CASE_X(BROWSER_FORWARD, AC_FORWARD);
        VK_CASE_X(BROWSER_REFRESH, AC_REFRESH);
        VK_CASE_X(BROWSER_STOP, AC_STOP);
        VK_CASE_X(BROWSER_SEARCH, AC_SEARCH);
        VK_CASE_X(BROWSER_FAVORITES, AC_BOOKMARKS);
        VK_CASE_X(BROWSER_HOME, AC_HOME);
        VK_CASE_X(VOLUME_MUTE, AUDIOMUTE);
        VK_CASE_X(VOLUME_DOWN, VOLUMEDOWN);
        VK_CASE_X(VOLUME_UP, VOLUMEUP);
        VK_CASE_X(MEDIA_NEXT_TRACK, AUDIONEXT);
        VK_CASE_X(MEDIA_PREV_TRACK, AUDIOPREV);
        VK_CASE_X(MEDIA_STOP, AUDIOSTOP);
        VK_CASE_X(MEDIA_PLAY_PAUSE, AUDIOPLAY);
        VK_CASE_X(LAUNCH_MAIL, MAIL);
        VK_CASE_X(LAUNCH_MEDIA_SELECT, MEDIASELECT);
        VK_CASE(CRSEL);
        VK_CASE(EXSEL);
        VK_CASE_X(PLAY, AUDIOPLAY);
    }

    return SDLK_UNKNOWN;
}
