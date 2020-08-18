#ifndef BUFFER_BUFFER_HPP
#define BUFFER_BUFFER_HPP

#include <memory>
#include <vector>

#include "buffer_base.hpp"

namespace buffer {
  namespace detail {
    // Allocator adaptor that interposes construct() calls to
    // convert value initialization into default initialization.
    // From: https://stackoverflow.com/questions/21028299/is-this-behavior-of-vectorresizesize-type-n-under-c11-and-boost-container/21028912#21028912
    template<typename T, typename A=std::allocator<T>>
    class default_init_allocator : public A {
        typedef std::allocator_traits<A> a_t;
      public:
        template<typename U>
        struct rebind {
          using other = default_init_allocator<U, typename a_t::template rebind_alloc<U>>;
        };

        using A::A;

        template<typename U>
        inline void construct(U* ptr) noexcept(std::is_nothrow_default_constructible_v<U>) {
          ::new(static_cast<void*>(ptr)) U;
        }

        template<typename U, typename...Args>
        inline void construct(U* ptr, Args&& ... args) {
          a_t::construct(static_cast<A&>(*this), ptr, std::forward<Args>(args)...);
        }
    };
  }

  template <typename CharT>
  class buffer : public detail::buffer_base<CharT, buffer<CharT>> {
      using base_type = detail::buffer_base<CharT, buffer<CharT>>;
      friend class detail::buffer_base<CharT, buffer<CharT>>;

      using vector_type = std::vector<CharT, detail::default_init_allocator<CharT>>;

    public:
      inline constexpr buffer() noexcept = default;

      inline explicit buffer(typename base_type::size_type n)
        : m_Storage(n) {}

      inline buffer(const buffer&) = default;

      inline buffer(buffer&&) noexcept = default;

      inline ~buffer() = default;


      inline buffer& operator=(const buffer&) = default;

      inline buffer& operator=(buffer&&) noexcept = default;


      inline void reserve(typename base_type::size_type n) {
        m_Storage.reserve(n);
      }

      inline void resize(typename base_type::size_type n) {
        m_Storage.resize(n);
      }

      inline void resize(typename base_type::size_type n, const typename base_type::value_type& value) {
        m_Storage.resize(n, value);
      }

      inline void shrink_to_fit() {
        m_Storage.shrink_to_fit();
      }

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
      vector_type m_Storage;
  };

  using byte_buffer = buffer<std::byte>;
}

#endif //BUFFER_BUFFER_HPP
