#ifndef BUFFER_ARRAY_CONVERSION_HPP
#define BUFFER_ARRAY_CONVERSION_HPP

#include <cstddef>

#include <array>

#include "../private/conversion_helpers.hpp"
#include "../private/span_def.hpp"

namespace buffer {
  template <typename T, size_t N>
  struct conversion_traits<std::array<T, N>> {
    inline static constexpr bool is_specialised = true;


    using src_char_type = T;


    inline static constexpr buffer_span<src_char_type> to_span(std::array<T, N>& a) noexcept {
      return buffer_span<src_char_type>{a.data(), a.size()};
    }

    inline static constexpr buffer_view<src_char_type> to_view(const std::array<T, N>& a) noexcept {
      return buffer_view<src_char_type>{a.data(), a.size()};
    }
  };
}    // namespace buffer

#endif    //BUFFER_ARRAY_CONVERSION_HPP
