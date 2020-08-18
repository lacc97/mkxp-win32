#ifndef BUFFER_BUFFER_SPAN_HPP
#define BUFFER_BUFFER_SPAN_HPP

#include "conversion/c_array_conversion.hpp"

#include "private/conversion_helpers.hpp"
#include "private/span_def.hpp"

namespace buffer {
  using byte_buffer_span = buffer_span<unsigned char>;


  template <typename T>
  inline constexpr auto to_byte_span(T&& t) noexcept {
    return buffer::to_span<unsigned char, T>(std::forward<T>(t));
  }
}

#endif //BUFFER_BUFFER_SPAN_HPP
