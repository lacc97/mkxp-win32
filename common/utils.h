#ifndef UTILS_H
#define UTILS_H

#include <cstring>

#include <limits>
#include <string>
#include <vector>

namespace str {
  namespace view {
    inline bool starts_with(std::string_view s, std::string_view pat) noexcept {
#if __cplusplus >= 202002L
      return s.starts_with(pat);
#else
      return (pat.size() <= s.size()) && (s.substr(0, pat.size()) == pat);
#endif
    }

    std::vector<std::string_view> tokenize(std::string_view s, std::string_view delims);

    std::string_view ltrim(std::string_view s, std::string_view chars = "\t\n\v\f\r ") noexcept;
    std::string_view rtrim(std::string_view s, std::string_view chars = "\t\n\v\f\r ") noexcept;
    inline std::string_view trim(std::string_view s, std::string_view chars = "\t\n\v\f\r ") noexcept {
      return rtrim(ltrim(s, chars), chars);
    }
  }    // namespace view

  std::string replace_all(std::string_view str, std::string_view pattern, std::string_view replacement);

  inline std::vector<std::string> tokenize(std::string_view s, std::string_view delims) {
    auto token_views = view::tokenize(s, delims);
    return std::vector<std::string>(token_views.begin(), token_views.end());
  }

  std::string to_lower(std::string_view s);

  inline std::string ltrim(std::string_view s, std::string_view chars = "\t\b\v\f\r ") {
    return std::string(view::ltrim(s, chars));
  }
  inline std::string rtrim(std::string_view s, std::string_view chars = "\t\b\v\f\r ") {
    return std::string(view::rtrim(s, chars));
  }
  inline std::string trim(std::string_view s, std::string_view chars = "\t\b\v\f\r ") {
    return std::string(view::trim(s, chars));
  }
}    // namespace str

namespace bits {
  template <typename T>
  inline constexpr T rotl(T v, unsigned int b) {
    static_assert(std::is_integral<T>::value, "rotate of non-integral type");
    if constexpr(std::is_signed_v<T>) {
      using signed_type = T;
      using unsigned_type = std::make_unsigned_t<signed_type>;

      unsigned_type uv;
      std::memcpy(&uv, &v, sizeof(uv));

      uv = rotl(uv, b);

      std::memcpy(&v, &uv, sizeof(v));
      return v;
    } else {
      constexpr unsigned int num_bits{std::numeric_limits<T>::digits};
      static_assert(0 == (num_bits & (num_bits - 1)), "rotate value bit length not power of two");

      using promoted_type = typename std::common_type<unsigned int, T>::type;
      using unsigned_promoted_type = typename std::make_unsigned<promoted_type>::type;

      constexpr unsigned int count_mask{num_bits - 1};

      const unsigned int mb{b & count_mask};
      return ((unsigned_promoted_type{v} << mb) | (unsigned_promoted_type{v} >> (-mb & count_mask)));
    }
  }

  template <typename T>
  inline constexpr T rotr(T v, unsigned int b) {
    static_assert(std::is_integral<T>::value, "rotate of non-integral type");

    if constexpr(std::is_signed_v<T>) {
      using signed_type = T;
      using unsigned_type = std::make_unsigned_t<signed_type>;

      unsigned_type uv;
      std::memcpy(&uv, &v, sizeof(uv));

      uv = rotr(uv, b);

      std::memcpy(&v, &uv, sizeof(v));
      return v;
    } else {
      constexpr unsigned int num_bits{std::numeric_limits<T>::digits};
      static_assert(0 == (num_bits & (num_bits - 1)), "rotate value bit length not power of two");

      using promoted_type = typename std::common_type<unsigned int, T>::type;
      using unsigned_promoted_type = typename std::make_unsigned<promoted_type>::type;

      constexpr unsigned int count_mask{num_bits - 1};

      const unsigned int mb{b & count_mask};
      return ((unsigned_promoted_type{v} >> mb) | (unsigned_promoted_type{v} << (-mb & count_mask)));
    }
  }
}    // namespace bits

namespace bytes {
  inline constexpr uint16_t concat2(uint8_t ls, uint8_t ms) noexcept {
    return ((uint16_t(ms) & 0xffu) << 8u) | (ls);
  }

  inline constexpr uint32_t concat4(uint16_t ls, uint16_t ms) noexcept {
    return ((uint32_t(ms) & 0xffffu) << 16u) | (ls);
  }

  inline constexpr uint32_t concat4(uint16_t ls, uint8_t ms, uint8_t mms) noexcept {
    return concat4(ls, concat2(ms, mms));
  }

  inline constexpr uint32_t concat4(uint8_t lls, uint8_t ls, uint16_t ms) noexcept {
    return concat4(concat2(lls, ls), ms);
  }

  inline constexpr uint32_t concat4(uint8_t lls, uint8_t ls, uint8_t ms, uint8_t mms) noexcept {
    return concat4(concat2(lls, ls), concat2(ms, mms));
  }

  namespace int32 {
    inline constexpr uint8_t lls(uint32_t i) noexcept {
      return (i) &0xffu;
    }

    inline constexpr uint8_t ls(uint32_t i) noexcept {
      return (i >> 8u) & 0xffu;
    }

    inline constexpr uint8_t ms(uint32_t i) noexcept {
      return (i >> 16u) & 0xffu;
    }

    inline constexpr uint8_t mms(uint32_t i) noexcept {
      return (i >> 24u) & 0xffu;
    }
  }    // namespace int32

  template <typename F>
  inline constexpr std::enable_if_t<std::is_invocable_r_v<uint8_t, F, uint8_t>, uint32_t> transform(uint32_t i, F&& f) noexcept {
    uint8_t lls = f(int32::lls(i)), ls = f(int32::ls(i)), ms = f(int32::ms(i)), mms = f(int32::mms(i));
    return concat4(lls, ls, ms, mms);
  }

  template <typename F>
  inline constexpr std::enable_if_t<std::is_invocable_r_v<uint8_t, F, uint8_t, uint8_t>, uint32_t> transform(uint32_t i1, uint32_t i2, F&& f) noexcept {
    uint8_t lls = f(int32::lls(i1), int32::lls(i2)), ls = f(int32::ls(i1), int32::ls(i2)), ms = f(int32::ms(i1), int32::ms(i2)),
            mms = f(int32::mms(i1), int32::mms(i2));
    return concat4(lls, ls, ms, mms);
  }

  template <typename T>
  inline constexpr T rotl(T v, unsigned int b) noexcept {
    static_assert(std::is_integral<T>::value, "rotate of non-integral type");
    static_assert(sizeof(T) > 1, "rotate value byte length must be larger than 1");

    return bits::rotl(v, std::numeric_limits<unsigned char>::digits * b);
  }

  template <typename T>
  inline constexpr T rotr(T v, unsigned int b) noexcept {
    static_assert(std::is_integral<T>::value, "rotate of non-integral type");
    static_assert(sizeof(T) > 1, "rotate value byte length must be larger than 1");

    return bits::rotr(v, std::numeric_limits<unsigned char>::digits * b);
  }
}    // namespace bytes

#endif
