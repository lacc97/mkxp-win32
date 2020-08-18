#ifndef BUFFER_ALIASING_HELPERS_HPP
#define BUFFER_ALIASING_HELPERS_HPP

#include <cstddef>

#include <type_traits>

#include "char_helpers.hpp"

namespace buffer::detail {
  template <typename Src, typename Dst>
  class span_aliasing {
      static_assert(is_char_allowed<Src> && is_char_allowed<Dst>, "types must be allowed in buffer template");

      using src_type = std::remove_const_t<Src>;
      using dst_type = std::remove_const_t<Dst>;

    public:
      inline static constexpr bool is_allowed = std::is_same_v<src_type, dst_type> ||
                                                std::is_same_v<dst_type, char> ||
                                                std::is_same_v<dst_type, signed char> ||
                                                std::is_same_v<dst_type, unsigned char> ||
                                                std::is_same_v<dst_type, std::byte>;

      inline static constexpr size_t size_factor = sizeof(src_type)/sizeof(dst_type);
  };
}

#endif    //BUFFER_ALIASING_HELPERS_HPP
