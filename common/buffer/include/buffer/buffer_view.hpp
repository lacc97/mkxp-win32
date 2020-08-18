#ifndef BUFFER_BUFFER_VIEW_HPP
#define BUFFER_BUFFER_VIEW_HPP

#include "conversion/c_array_conversion.hpp"

#include "private/conversion_helpers.hpp"
#include "private/span_def.hpp"

namespace buffer {
  using byte_buffer_view = buffer_view<unsigned char>;


  template <typename T>
  inline constexpr auto to_byte_view(T&& t) noexcept {
    return to_view<unsigned char, T>(std::forward<T>(t));
  }
}

#endif //BUFFER_BUFFER_VIEW_HPP
