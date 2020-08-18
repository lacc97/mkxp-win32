#ifndef BUFFER_STATIC_BUFFER_HPP
#define BUFFER_STATIC_BUFFER_HPP

#include <array>

#include "private/buffer_base.hpp"
#include "private/conversion_helpers.hpp"

namespace buffer {
  template <typename CharT, size_t N>
  class static_buffer : public detail::buffer_base<CharT, static_buffer<CharT, N>> {
      using base_type = detail::buffer_base<CharT, static_buffer<CharT, N>>;
      friend class detail::buffer_base<CharT, static_buffer<CharT, N>>;

      using array_type = std::array<CharT, N>;

    public:
      inline constexpr static_buffer() noexcept = default;

      inline constexpr static_buffer(const static_buffer&) noexcept = default;

      inline constexpr static_buffer(static_buffer&&) noexcept = default;

      inline ~static_buffer() = default;


      inline constexpr static_buffer& operator=(const static_buffer&) noexcept = default;

      inline constexpr static_buffer& operator=(static_buffer&&) noexcept = default;

    protected:
      [[nodiscard]] inline typename base_type::pointer storage_pointer() noexcept {
        return m_Storage.data();
      }

      [[nodiscard]] inline typename base_type::maybe_const_pointer storage_pointer() const noexcept {
        return m_Storage.data();
      }

      [[nodiscard]] inline typename base_type::size_type storage_size() const noexcept {
        return m_Storage.size();
      }

    private:
      array_type m_Storage;
  };

  template <typename CharT, size_t N>
  struct conversion_traits<static_buffer<CharT, N>> {
  inline static constexpr bool is_specialised = true;


  using src_char_type = CharT;


  inline static constexpr buffer_span<src_char_type> to_span(static_buffer<CharT, N>& sb) noexcept {
    return buffer_span<src_char_type>{sb.data(), sb.size()};
  }

  inline static constexpr buffer_view<src_char_type> to_view(const static_buffer<CharT, N>& sb) noexcept {
    return buffer_view<src_char_type>{sb.data(), sb.size()};
  }
};

  template <size_t N>
  using static_byte_buffer = static_buffer<unsigned char, N>;
}

#endif //BUFFER_STATIC_BUFFER_HPP
