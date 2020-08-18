#ifndef BUFFER_CONVERSION_HELPERS_HPP
#define BUFFER_CONVERSION_HELPERS_HPP

#include <type_traits>
#include <utility>

#include "char_helpers.hpp"
#include "span_fwd.hpp"

namespace buffer {
  template <typename T>
  struct conversion_traits {
    inline static constexpr bool is_specialised = false;
  };

  namespace detail {
    template <typename Dst, typename Src>
    struct conversion {
      static_assert(is_char_allowed<Src> && is_char_allowed<Dst>, "types must be allowed in buffer template");

      using src_type = Src;
      using src_noconst_type = std::remove_const_t<src_type>;
      using dst_type = Dst;
      using dst_noconst_type = std::remove_const_t<dst_type>;

      inline static constexpr bool is_src_const = std::is_const_v<src_type>;
      inline static constexpr bool is_dst_const = std::is_const_v<dst_type>;

      inline static constexpr bool is_span_convertible =
        (!is_src_const && !is_dst_const) &&
        std::is_convertible_v<buffer::buffer_span<src_noconst_type>, buffer::buffer_view<dst_noconst_type>>;
      inline static constexpr bool is_view_convertible =
        std::is_convertible_v<buffer::buffer_view<src_noconst_type>, buffer::buffer_view<dst_noconst_type>>;
    };

    template <typename DstCharT, typename T>
    inline constexpr bool is_span_convertible =
      are_chars_allowed<DstCharT, T>&& conversion<DstCharT, T>::is_span_convertible;

    template <typename DstCharT, typename T>
    inline constexpr bool is_view_convertible =
      are_chars_allowed<DstCharT, T>&& conversion<DstCharT, T>::is_view_convertible;

    template <typename DstCharT, typename T>
    using converted_span =
      std::enable_if_t<conversion_traits<T>::is_specialised,
                       std::enable_if_t<is_span_convertible<DstCharT, typename conversion_traits<T>::src_char_type>,
                                        buffer::buffer_span<DstCharT>>>;

    template <typename DstCharT, typename T>
    using converted_view =
      std::enable_if_t<conversion_traits<T>::is_specialised,
                       std::enable_if_t<is_view_convertible<DstCharT, typename conversion_traits<T>::src_char_type>,
                                        buffer::buffer_view<std::remove_const_t<DstCharT>>>>;
  }    // namespace detail

  template <typename Dst, typename T>
  inline constexpr detail::converted_span<Dst, std::remove_const_t<std::remove_reference_t<T>>> to_span(T&& t) noexcept {
    return conversion_traits<std::remove_const_t<std::remove_reference_t<T>>>::to_span(std::forward<T>(t));
  }

  template <typename Dst, typename T>
  inline constexpr detail::converted_view<Dst, std::remove_const_t<std::remove_reference_t<T>>>
  to_view(T&& t) noexcept {
    return conversion_traits<std::remove_const_t<std::remove_reference_t<T>>>::to_view(std::forward<T>(t));
  }
}    // namespace buffer

#endif    //BUFFER_CONVERSION_HELPERS_HPP
