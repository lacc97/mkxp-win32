#ifndef BUFFER_VECTOR_CONVERSION_HPP
#define BUFFER_VECTOR_CONVERSION_HPP

#include <vector>

#include "../private/conversion_helpers.hpp"
#include "../private/span_def.hpp"

namespace buffer {
  template <typename T, typename Allocator>
  struct conversion_traits<std::vector<T, Allocator>> {
    inline static constexpr bool is_specialised = true;


    using src_char_type = T;


    inline static constexpr buffer_span<src_char_type> to_span(std::vector<T, Allocator>& a) noexcept {
      return buffer_span<src_char_type>{a.data(), a.size()};
    }

    inline static constexpr buffer_view<src_char_type> to_view(const std::vector<T, Allocator>& a) noexcept {
      return buffer_view<src_char_type>{a.data(), a.size()};
    }
  };
}

#endif    //BUFFER_VECTOR_CONVERSION_HPP
