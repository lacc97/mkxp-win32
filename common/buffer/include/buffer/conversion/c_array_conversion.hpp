#ifndef BUFFER_C_ARRAY_CONVERSION_HPP
#define BUFFER_C_ARRAY_CONVERSION_HPP

#include "../private/conversion_helpers.hpp"
#include "../private/span_def.hpp"

namespace buffer {
  template <typename T, size_t N>
  struct conversion_traits<T[N]> {
    inline static constexpr bool is_specialised = true;


    using src_char_type = T;


    inline static constexpr buffer_span<src_char_type> to_span(src_char_type(&arr)[N]) noexcept {
      return buffer_span<src_char_type>{arr, N};
    }

    inline static constexpr buffer_view<src_char_type> to_view(const src_char_type(&arr)[N]) noexcept {
      return buffer_view<src_char_type>{arr, N};
    }
  };
}

#endif    //BUFFER_C_ARRAY_CONVERSION_HPP
