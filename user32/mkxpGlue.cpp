#include "mkxpGlue.h"

#include "visibility.h"

namespace mkxp {
  namespace {
    SDL_Window* s_Window = nullptr;
  }

  SDL_Window* getWindow() noexcept {
    return s_Window;
  }

  WIN32_API void setWindow(SDL_Window* win) noexcept {
    s_Window = win;
  }
}    // namespace mkxp
