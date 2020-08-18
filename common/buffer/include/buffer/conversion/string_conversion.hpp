#ifndef BUFFER_STRING_CONVERSION_HPP
#define BUFFER_STRING_CONVERSION_HPP

#include <string>

#include "../private/conversion_helpers.hpp"
#include "../private/span_def.hpp"

namespace buffer {
  template <typename T, typename Traits, typename Allocator>
  struct conversion_traits<std::basic_string<T, Traits, Allocator>> {
    inline static constexpr bool is_specialised = true;


    using src_char_type = T;


    inline static constexpr buffer_span<src_char_type> to_span(std::basic_string<T, Traits, Allocator>& a) noexcept {
      return buffer_span<src_char_type>{a.data(), a.size()};
    }

    inline static constexpr buffer_view<src_char_type> to_view(const std::basic_string<T, Traits, Allocator>& a) noexcept {
      return buffer_view<src_char_type>{a.data(), a.size()};
    }
  };
}    // namespace buffer

#endif    //BUFFER_STRING_CONVERSION_HPP
