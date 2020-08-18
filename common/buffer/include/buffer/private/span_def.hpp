#ifndef BUFFER_SPAN_DEF_HPP
#define BUFFER_SPAN_DEF_HPP

#include <cstring>

#include "aliasing_helpers.hpp"
#include "buffer_base.hpp"
#include "conversion_helpers.hpp"
#include "span_fwd.hpp"

namespace buffer {
  namespace detail {
    template <typename CharT>
    struct impl_traits<buffer_span<CharT>> {
      inline static constexpr bool owns_buffer = false;
    };

    template <typename CharT>
    class buffer_span : public buffer_base<CharT, buffer_span<CharT>> {
      using base_type = buffer_base<CharT, buffer_span<CharT>>;
      friend class buffer_base<CharT, buffer_span<CharT>>;

      template <typename OtherCharT>
      using aliasing_info_t = span_aliasing<OtherCharT, CharT>;

      template <typename OtherCharT>
      using aliasing_allowed_t = std::enable_if_t<
        aliasing_info_t<OtherCharT>::is_allowed && (!std::is_const_v<OtherCharT> || std::is_const_v<CharT>), bool>;

      template <typename T>
      using taken_t = std::enable_if_t<is_severable<T, CharT>, std::conditional_t<std::is_same_v<T, CharT>, CharT&, T>>;

     public:
      inline constexpr buffer_span() noexcept = default;

      template <typename OtherCharT, aliasing_allowed_t<OtherCharT> = true>
      inline constexpr buffer_span(buffer_span<OtherCharT> buf) noexcept
          : m_Ptr{reinterpret_cast<typename base_type::pointer>(buf.data())},
            m_Size{aliasing_info_t<OtherCharT>::size_factor * buf.size()} {}

      template <typename OtherCharT, aliasing_allowed_t<OtherCharT> = true>
      inline constexpr buffer_span(OtherCharT* s, typename base_type::size_type count) noexcept
          : m_Ptr{reinterpret_cast<typename base_type::pointer>(s)},
            m_Size{aliasing_info_t<OtherCharT>::size_factor * count} {}

      template <typename OtherCharT, aliasing_allowed_t<OtherCharT> = true>
      inline constexpr buffer_span(OtherCharT* b, OtherCharT* e) noexcept
          : m_Ptr{b},
            m_Size{aliasing_info_t<OtherCharT>::size_factor * static_cast<typename base_type::size_type>(e - b)} {
        assert(b < e);
      }


      template <typename OtherCharT, aliasing_allowed_t<OtherCharT> = true>
      inline constexpr buffer_span& operator=(buffer_span<OtherCharT> buf) noexcept {
        m_Ptr = reinterpret_cast<typename base_type::pointer>(buf.data());
        m_Size = aliasing_info_t<OtherCharT>::size_factor * buf.size();
        return *this;
      }

      template <typename T = CharT>
      inline constexpr taken_t<T> take_first() noexcept {
        if constexpr(std::is_same_v<T, CharT>) {
          return take_first(1)[0];
        } else {
          return sever<T, std::remove_const_t<CharT>>(take_first(sever_size<T> / sizeof(CharT)).view());
        }
      }

      inline constexpr buffer_span take_first(typename base_type::size_type count) noexcept {
        auto firstHalf = this->first(count);
        m_Ptr += firstHalf.size();
        m_Size -= firstHalf.size();
        return firstHalf;
      }

      template <typename T = CharT>
      inline constexpr taken_t<T> take_last() noexcept {
        if constexpr(std::is_same_v<T, CharT>) {
          return take_last(1)[0];
        } else {
          return sever<T, std::remove_const_t<CharT>>(take_last(sever_size<T> / sizeof(CharT)).view());
        }
      }

      inline constexpr buffer_span take_last(typename base_type::size_type count) noexcept {
        auto secondHalf = this->last(count);
        m_Size -= secondHalf.size();
        return secondHalf;
      }

     protected:
      [[nodiscard]] inline constexpr typename base_type::pointer storage_pointer() noexcept {
        return m_Ptr;
      }

      [[nodiscard]] inline constexpr typename base_type::maybe_const_pointer storage_pointer() const noexcept {
        return m_Ptr;
      }

      [[nodiscard]] inline constexpr typename base_type::size_type storage_size() const noexcept {
        return m_Size;
      }

     private:
      typename base_type::pointer m_Ptr{nullptr};
      typename base_type::size_type m_Size{0};
    };
  }    // namespace detail

  template <typename CharT>
  struct conversion_traits<detail::buffer_span<CharT>> {
    inline static constexpr bool is_specialised = true;


    using src_char_type = CharT;


    inline static constexpr buffer_span<src_char_type> to_span(buffer_span<src_char_type> t) noexcept {
      return t;
    }

    inline static constexpr buffer_view<src_char_type> to_view(buffer_span<src_char_type> t) noexcept {
      return t;
    }
  };
}    // namespace buffer

#endif    //BUFFER_SPAN_DEF_HPP
