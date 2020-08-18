#ifndef BUFFER_CHAR_HELPERS_HPP
#define BUFFER_CHAR_HELPERS_HPP

#include <cstddef>

#include <type_traits>

namespace buffer::detail {
  template <typename CharT>
  inline static constexpr bool is_char_allowed = (std::is_integral_v<std::remove_const_t<CharT>> || std::is_same_v<std::remove_const_t<CharT>, std::byte>) && !std::is_same_v<std::remove_const_t<CharT>, bool>;

  template <typename CharT1, typename CharT2>
  inline static constexpr bool are_chars_allowed = is_char_allowed<CharT1> && is_char_allowed<CharT2>;
}

#endif    //BUFFER_CHAR_HELPERS_HPP
