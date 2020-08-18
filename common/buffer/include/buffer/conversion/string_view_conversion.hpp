#ifndef BUFFER_STRING_VIEW_CONVERSION_HPP
#define BUFFER_STRING_VIEW_CONVERSION_HPP

#include <string_view>

#include "../private/conversion_helpers.hpp"
#include "../private/span_def.hpp"

namespace buffer {
  template <typename T, typename Traits>
  struct conversion_traits<std::basic_string_view<T, Traits>> {
    inline static constexpr bool is_specialised = true;


    using src_char_type = T;


    inline static constexpr buffer_view<src_char_type> to_view(std::basic_string_view<T, Traits> a) noexcept {
      return buffer_view<src_char_type>{a.data(), a.size()};
    }
  };
}

#endif    //BUFFER_STRING_VIEW_CONVERSION_HPP
