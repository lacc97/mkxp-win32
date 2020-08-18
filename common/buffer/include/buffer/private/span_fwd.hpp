#ifndef BUFFER_SPAN_FWD_HPP
#define BUFFER_SPAN_FWD_HPP

#include <type_traits>

namespace buffer {
  namespace detail {
    template<typename CharT>
    class buffer_span;
  }

  template<typename CharT>
  using buffer_span = detail::buffer_span<std::enable_if_t<!std::is_const_v<CharT>, CharT>>;

  template<typename CharT>
  using buffer_view = detail::buffer_span<std::enable_if_t<!std::is_const_v<CharT>, const CharT>>;
}

#endif    //BUFFER_SPAN_FWD_HPP
