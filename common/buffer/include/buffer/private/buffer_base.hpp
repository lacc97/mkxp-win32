#ifndef BUFFER_BUFFER_BASE_HPP
#define BUFFER_BUFFER_BASE_HPP

#include <cassert>
#include <cstring>

#include <iterator>
#include <limits>
#include <type_traits>

#include "char_helpers.hpp"
#include "span_fwd.hpp"

#if defined(NDEBUG)
# if defined(_MSC_VER)
#  define BUFFER_ASSUME(x) __assume(x)
# elif defined(__GNUC__)
#  define BUFFER_ASSUME(x) do { if (!(x)) __builtin_unreachable(); } while (0)
# else
#  define BUFFER_ASSUME(x)
# endif
#else
# define BUFFER_ASSUME(x) assert(x)
#endif

namespace buffer {
  template <typename T>
  inline constexpr size_t sever_size = sizeof(T);

  namespace detail {
    template <typename ImplT>
    struct impl_traits {
      inline static constexpr bool owns_buffer = true;
    };
  }

  template<typename T, typename CharT>
  inline static constexpr bool is_severable = std::is_pod_v<T> && sever_size<T> >= sizeof(CharT) && (sever_size<T> % sizeof(CharT) == 0);

  template<typename T, typename CharT>
  inline std::enable_if_t<is_severable<T, CharT> && !std::is_const_v<CharT>, T> sever(detail::buffer_span<const CharT>) noexcept;
}

namespace buffer::detail {
  template<typename CharT, typename ImplT>
  class buffer_base {
    static_assert(std::is_same_v<std::remove_volatile_t<CharT>, CharT>,
                  "buffer must have non-volatile type");
    static_assert(is_char_allowed<CharT>,
                  "buffer must have integral, non-bool type (or be std::byte)");

    using impl_type = ImplT;

    inline static constexpr bool owns_buffer = impl_traits<impl_type>::owns_buffer;

   public:
    using value_type = CharT;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

   protected:
    using maybe_const_value_type = std::conditional_t<owns_buffer, std::add_const_t<value_type>, value_type>;
    using maybe_const_reference = maybe_const_value_type&;
    using maybe_const_pointer = maybe_const_value_type*;
    using maybe_const_iterator = maybe_const_pointer;
    using maybe_const_reverse_iterator = std::reverse_iterator<maybe_const_iterator>;

    template <typename T>
    using aliased_access_t = std::conditional<std::is_same_v<T, value_type>, maybe_const_reference, T>;


    template <typename OtherCharT>
    inline static constexpr bool is_compatible_v = is_char_allowed<OtherCharT> &&
                                                   (sizeof(OtherCharT) == sizeof(CharT)) &&
                                                   (std::alignment_of_v<OtherCharT> == std::alignment_of_v<CharT>);

    template <typename OtherCharT>
    inline static constexpr bool is_copyable_from_v = is_compatible_v<OtherCharT> && !std::is_const_v<CharT>;

    template <typename OtherCharT>
    inline static constexpr bool is_copyable_to_v = is_compatible_v<OtherCharT> && !std::is_const_v<OtherCharT>;

    template <typename T>
    inline static constexpr bool is_severable_v = is_severable<T, value_type>;

    template <typename T>
    inline static constexpr bool is_settable_v = !std::is_const_v<value_type> && is_severable_v<T>;

    template <typename T>
    inline static constexpr bool is_const_settable_v = !owns_buffer && is_settable_v<T>;

   public:
    template <typename T = CharT>
    [[nodiscard]] inline constexpr std::enable_if_t<is_severable_v<T>, T> at(size_type pos) const noexcept {
      if constexpr(std::is_same_v<std::remove_const_t<T>, std::remove_const_t<value_type>>) {
        BUFFER_ASSUME(size() > pos);
        return operator[](pos);
      } else {
        return sever<T, std::remove_const_t<value_type>>(subspan(pos, sever_size<T>/sizeof(value_type)));
      }
    }

    [[nodiscard]] inline constexpr iterator begin() noexcept {
      return data();
    }

    [[nodiscard]] inline constexpr maybe_const_iterator begin() const noexcept {
      return data();
    }

    template<typename OutputIterator>
    inline size_type copy(OutputIterator out, size_type count = std::numeric_limits<size_type>::max(), size_type pos = 0) const noexcept {
      auto src = subspan(pos, count);
      return std::distance(out, std::copy(src.begin(), src.end(), out));
    }

    template <typename T>
    inline std::enable_if_t<is_settable_v<T>, size_type> copy_from(T t, size_type pos = 0) noexcept {
      return set<T>(pos, t);
    }

    template <typename T>
    inline std::enable_if_t<is_const_settable_v<T>, size_type> copy_from(T t, size_type pos = 0) const noexcept {
      return set<T>(pos, t);
    }

    template <typename OtherCharT>
    inline std::enable_if_t<is_copyable_from_v<OtherCharT>, size_type> copy_from(buffer_span<OtherCharT> src, size_type count = std::numeric_limits<size_type>::max(), size_type pos = 0) const noexcept {
      return src.copy_to(subspan(pos), count);
    }

    template<typename OtherCharT>
    inline std::enable_if_t<is_copyable_to_v<OtherCharT>, size_type> copy_to(buffer_span<OtherCharT> dst, size_type count = std::numeric_limits<size_type>::max(), size_type pos = 0) const noexcept {
//        if constexpr(is_compatible_v<OtherCharT>) {
      auto src = subspan(pos, std::min(count, dst.size()));
      if(!src.empty())
        std::memmove(dst.data(), src.data(), sizeof(value_type)*src.size());
      return src.size();
//        } else {
//          BUFFER_ASSUME(((count*sizeof(OtherCharT))%sizeof(CharT)) == 0);
//          BUFFER_ASSUME(((dst.size()*sizeof(OtherCharT))%sizeof(CharT)) == 0);
//          auto src = subspan(pos, std::min(sizeof(OtherCharT)*count/sizeof(CharT), sizeof(OtherCharT)*dst.size()/sizeof(CharT)));
//          std::memcpy(dst.data(), src.data(), sizeof(CharT)*src.size());
//          return src.size();
//        }
    }

    [[nodiscard]] inline constexpr const_iterator cbegin() const noexcept {
      return data();
    }

    [[nodiscard]] inline constexpr const_iterator cend() const noexcept {
      return data() + size();
    }

    [[nodiscard]] inline constexpr pointer data() noexcept {
      return static_cast<impl_type*>(this)->storage_pointer();
    }

    [[nodiscard]] inline constexpr maybe_const_pointer data() const noexcept {
      return static_cast<const impl_type*>(this)->storage_pointer();
    }

    [[nodiscard]] inline constexpr bool empty() const noexcept {
      return size() == 0;
    }

    [[nodiscard]] inline constexpr iterator end() noexcept {
      return data() + size();
    }

    [[nodiscard]] inline constexpr maybe_const_iterator end() const noexcept {
      return data() + size();
    }

    inline constexpr void fill(CharT v) noexcept {
      std::fill(begin(), end(), v);
    }

    inline constexpr void fill(CharT v) const noexcept {
      std::fill(begin(), end(), v);
    }

    [[nodiscard]] inline constexpr reference first() noexcept {
      BUFFER_ASSUME(!empty());
      return operator[](0);
    }

    template <typename T = CharT>
    [[nodiscard]] inline constexpr std::enable_if_t<is_severable_v<T>, aliased_access_t<T>> first() const noexcept {
      if constexpr(std::is_same_v<std::remove_const_t<T>, std::remove_const_t<value_type>>) {
        BUFFER_ASSUME(!empty());
        return operator[](0);
      } else {
        return sever<T, std::remove_const_t<value_type>>(first(sever_size<T>/sizeof(value_type)));
      }
    }

    [[nodiscard]] inline constexpr buffer_span<value_type> first(size_type n) noexcept {
      return subspan(0, n);
    }

    [[nodiscard]] inline constexpr buffer_span<maybe_const_value_type> first(size_type n) const noexcept {
      return subspan(0, n);
    }

    [[nodiscard]] inline constexpr reference last() noexcept {
      BUFFER_ASSUME(!empty());
      return operator[](size() - 1);
    }

    template <typename T = CharT>
    [[nodiscard]] inline constexpr std::enable_if_t<is_severable_v<T, CharT>, aliased_access_t<T>> last() const noexcept {
      if constexpr(std::is_same_v<std::remove_const_t<T>, std::remove_const_t<value_type>>) {
        BUFFER_ASSUME(!empty());
        return operator[](size() - 1);
      } else {
        return sever<T, std::remove_const_t<value_type>>(last(sever_size<T> / sizeof(value_type)));
      }
    }

    [[nodiscard]] inline constexpr buffer_span<value_type> last(size_type count) noexcept {
      return subspan(size() - std::min(count, size()));
    }

    [[nodiscard]] inline constexpr buffer_span<maybe_const_value_type> last(size_type count) const noexcept {
      return subspan(size() - std::min(count, size()));
    }

    template <typename T>
    inline std::enable_if_t<is_settable_v<T>, size_type> set(size_type pos, T t) noexcept {
      auto dst = subspan(pos);
      if(dst.size() < sizeof(t))
        return 0;
      std::memcpy(dst.data(), &t, sizeof(t));
      return sizeof(t);
    }

    template <typename T>
    inline std::enable_if_t<is_const_settable_v<T>, size_type> set(size_type pos, T t) const noexcept {
      auto dst = subspan(pos);
      if(dst.size() < sizeof(t))
        return 0;
      std::memcpy(dst.data(), &t, sizeof(t));
      return sizeof(t);
    }

    [[nodiscard]] inline constexpr size_type size() const noexcept {
      return static_cast<const impl_type*>(this)->storage_size();
    }

    [[nodiscard]] inline constexpr buffer_span<value_type> span() noexcept {
      return subspan(0);
    }

    [[nodiscard]] inline constexpr buffer_span<maybe_const_value_type> span() const noexcept {
      return subspan(0);
    }

    [[nodiscard]] inline constexpr buffer_span<value_type> subspan(size_type off) noexcept {
      BUFFER_ASSUME(off <= size());
      return buffer_span<value_type>{data() + off, size() - off};
    }

    [[nodiscard]] inline constexpr buffer_span<maybe_const_value_type> subspan(size_type off) const noexcept {
      BUFFER_ASSUME(off <= size());
      return buffer_span<maybe_const_value_type>{data() + off, size() - off};
    }

    [[nodiscard]] inline constexpr buffer_span<value_type> subspan(size_type off, size_type count) noexcept {
      BUFFER_ASSUME(off <= size());
      return buffer_span<value_type>{data() + off, std::min(count, size() - off)};
    }

    [[nodiscard]] inline constexpr buffer_span<maybe_const_value_type> subspan(size_type off, size_type count) const noexcept {
      BUFFER_ASSUME(off <= size());
      return buffer_span<maybe_const_value_type>{data() + off, std::min(count, size() - off)};
    }

    [[nodiscard]] inline constexpr buffer_span<const value_type> subview(size_type offset) const noexcept {
      return subspan(offset);
    }

    [[nodiscard]] inline constexpr buffer_span<const value_type> subview(size_type offset, size_type count) const noexcept {
      return subspan(offset, count);
    }

    [[nodiscard]] inline constexpr buffer_span<const value_type> view() const noexcept {
      return subview(0);
    }

    [[nodiscard]] inline constexpr reference operator[](size_type pos) {
      BUFFER_ASSUME(pos < size());
      return data()[pos];
    }

    [[nodiscard]] inline constexpr maybe_const_reference operator[](size_type pos) const {
      BUFFER_ASSUME(pos < size());
      return data()[pos];
    }
  };
}

namespace buffer {
  template<typename T, typename CharT>
  inline std::enable_if_t<is_severable<T, CharT> && !std::is_const_v<CharT>, T> sever(detail::buffer_span<const CharT> buf) noexcept {
    if constexpr(!std::is_const_v<CharT>) {
      static_assert(sever_size<T> == sizeof(T),
                    "if you specialise buffer::sever_size<T>, you must also specialise buffer::sever<T>()");
      BUFFER_ASSUME(sever_size<T> <= buf.size() * sizeof(CharT));

      T t;
      std::memcpy(&t, buf.data(), sizeof(T));
      return t;
    } else {
      return sever<std::remove_const_t<T>, CharT>(buf);
    }
  }
}

#undef BUFFER_ASSUME

#endif //BUFFER_BUFFER_BASE_HPP
