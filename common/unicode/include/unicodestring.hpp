#ifndef UNICODE_UNICODESTRING_HPP
#define UNICODE_UNICODESTRING_HPP

#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "private/api.hpp"

namespace unicode {
  namespace detail {
    // Allocator adaptor that interposes construct() calls to
    // convert value initialization into default initialization.
    // From: https://stackoverflow.com/questions/21028299/is-this-behavior-of-vectorresizesize-type-n-under-c11-and-boost-container/21028912#21028912
    template<typename T, typename A=std::allocator<T>>
    class default_init_allocator : public A {
        typedef std::allocator_traits<A> a_t;
      public:
        template<typename U>
        struct rebind {
          using other = default_init_allocator<U, typename a_t::template rebind_alloc<U>>;
        };

        using A::A;

        template<typename U>
        void construct(U* ptr)
        noexcept(std::is_nothrow_default_constructible_v<U>) {
          ::new(static_cast<void*>(ptr)) U;
        }

        template<typename U, typename...Args>
        void construct(U* ptr, Args&& ... args) {
          a_t::construct(static_cast<A&>(*this), ptr, std::forward<Args>(args)...);
        }
    };
  }

  using string = std::basic_string<char16_t, std::char_traits<char16_t>, detail::default_init_allocator<char16_t>>;
  using string_view = std::basic_string_view<char16_t>;

  namespace literals {
    inline string operator ""_u(const char16_t* str, size_t len) {
      return string{str, len};
    }

    inline constexpr string_view operator ""_uv(const char16_t* str, size_t len) noexcept {
      return string_view{str, len};
    }
  }


  UNICODE_PUBLIC int caseInsensitiveCompare(string_view s1, string_view s2);

  UNICODE_PUBLIC std::optional<string> fromUTF8(std::string_view utf8);

  UNICODE_PUBLIC std::optional<string> toLowerCase(string_view str);

  UNICODE_PUBLIC std::optional<string> toUpperCase(string_view str);

  UNICODE_PUBLIC std::optional<std::string> toUTF8(string_view utf16);
}

inline auto utf8(unicode::string_view sv) {
  return unicode::toUTF8(sv).value_or("");
}

inline auto utf16(std::string_view svutf8) {
  return unicode::fromUTF8(svutf8).value_or(u"");
}

#endif //UNICODE_UNICODESTRING_HPP
