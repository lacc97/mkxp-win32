#ifndef BUFFER_SMALL_BUFFER_HPP
#define BUFFER_SMALL_BUFFER_HPP

#include <variant>

#include "buffer_base.hpp"
#include "buffer.hpp"

namespace buffer {
  template<typename CharT, size_t StaticN>
  class small_buffer : public detail::buffer_base<CharT, small_buffer<CharT, StaticN>> {
      using base_type = detail::buffer_base<CharT, small_buffer<CharT, StaticN>>;
      friend class detail::buffer_base<CharT, small_buffer<CharT, StaticN>>;

      struct array {
        std::array<CharT, StaticN> a;
        size_t s{0};


        inline CharT* data() {
          return a.data();
        }

        inline const CharT* data() const {
          return a.data();
        }

        inline size_t size() const {
          return s;
        }
      };

      using array_type = array;
      using vector_type = std::vector<CharT, detail::default_init_allocator < CharT>>;

    public:
      inline constexpr small_buffer() noexcept = default;

      inline explicit small_buffer(typename base_type::size_type n)
        : small_buffer{} {
        resize(n);
      }

      template<typename OtherCharT, std::enable_if_t<base_type::template is_compatible_v<OtherCharT>, bool> = true>
      inline explicit small_buffer(OtherCharT* p, typename base_type::size_type n)
        : small_buffer{n} {
        std::memcpy(storage_pointer(), p, n * sizeof(CharT));
      }

      inline small_buffer(const small_buffer&) = default;

      inline small_buffer(small_buffer&&) noexcept = default;

      inline ~small_buffer() = default;


      inline small_buffer& operator=(const small_buffer&) = default;

      inline small_buffer& operator=(small_buffer&&) noexcept = default;


      inline void resize(typename base_type::size_type n) {
        std::visit([this, n](auto&& s) -> void {
          using T = std::decay_t<decltype(s)>;

          if constexpr(std::is_same_v<T, array_type>) {
            if(n <= StaticN) {
              s.s = n;
            } else {
              auto v = vector_type(n);
              std::copy(s.a.begin(), s.a.begin() + s.s, v.begin());
              m_Storage.template emplace<vector_type>(std::move(v));
            }
          } else {
            s.resize(n);
          }
        }, m_Storage);
      }

      inline void resize(typename base_type::size_type n, const typename base_type::value_type& value) {
        std::visit([this, n, &value](auto&& s) -> void {
          using T = std::decay_t<decltype(s)>;

          if constexpr(std::is_same_v<T, array_type>) {
            if(n <= StaticN) {
              if(n > s)
                std::fill(s.a.begin() + s.s, s.a.begin() + n, value);

              s.s = n;
            } else {
              auto v = vector_type(n);
              std::fill(std::copy(s.a.begin(), s.a.begin() + s.s, v.begin()), v.end(), value);
              m_Storage.template emplace<vector_type>(std::move(v));
            }
          } else {
            s.resize(n, value);
          }
        }, m_Storage);
      }

    protected:
      [[nodiscard]] inline typename base_type::pointer storage_pointer() noexcept {
        return std::visit([](auto&& s) -> typename base_type::pointer {
          return s.data();
        }, m_Storage);
      }

      [[nodiscard]] inline typename base_type::maybe_const_pointer storage_pointer() const noexcept {
        return std::visit([](auto&& s) -> typename base_type::maybe_const_pointer {
          return s.data();
        }, m_Storage);
      }

      [[nodiscard]] inline typename base_type::size_type storage_size() const noexcept {
        return std::visit([](auto&& s) -> typename base_type::size_type {
          return s.size();
        }, m_Storage);
      }

    private:
      std::variant<array_type, vector_type> m_Storage;
  };

  template<size_t StaticN>
  using small_byte_buffer = small_buffer<std::byte, StaticN>;
}

#endif //BUFFER_SMALL_BUFFER_HPP
