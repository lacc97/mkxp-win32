#ifndef UNICODE_UNICODE_HPP
#define UNICODE_UNICODE_HPP

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

#include "unicodestring.hpp"

#include "private/api.hpp"

struct UConverter;

namespace unicode {
  using CodePage = uint32_t;

  class UNICODE_PUBLIC UTF16Converter {
    public:
      explicit UTF16Converter(CodePage enc);

      UTF16Converter(const UTF16Converter&) = delete;

      inline UTF16Converter(UTF16Converter&& other) noexcept
        : UTF16Converter{other.m_Encoding} {
        std::swap(mp_Converter, other.mp_Converter);
      }

      ~UTF16Converter();


      UTF16Converter& operator=(const UTF16Converter&) = delete;

      UTF16Converter& operator=(UTF16Converter&& other) noexcept;


      [[nodiscard]] inline CodePage encoding() const {
        return m_Encoding;
      }

      template<typename CharT>
      [[nodiscard]] inline std::optional<string> fromBytes(const CharT* str, size_t len) const {
        static_assert(std::is_integral_v<CharT>);
        return fromBytes(reinterpret_cast<const char*>(str), len * sizeof(CharT));
      }

      [[nodiscard]] inline std::optional<string> fromBytes(const char* str, size_t len) const {
        return fromBytes(std::string_view{str, len});
      }

      [[nodiscard]] std::optional<string> fromBytes(std::string_view str) const;

      [[nodiscard]] inline std::optional<std::string> toBytes(const string::value_type* str, size_t len) const {
        return toBytes(string_view{str, len});
      }

      [[nodiscard]] std::optional<std::string> toBytes(string_view str) const;


      inline explicit operator bool() const {
        return mp_Converter;
      }

    private:
      inline static std::optional<const char*> ucnvString(CodePage enc) {
        auto it = UCNV_STRING.find(enc);
        if(it != UCNV_STRING.end())
          return it->second;
        else
          return {};
      }


      static const std::map<CodePage, const char*> UCNV_STRING;


      CodePage m_Encoding;
      UConverter* mp_Converter;
  };

  class UNICODE_PUBLIC Converter {
    public:
      inline Converter(CodePage a, CodePage b)
        : m_A{a},
          m_B{b} {}

      Converter(const Converter&) = delete;

      Converter(Converter&&) = default;

      ~Converter() = default;


      Converter& operator=(const Converter&) = delete;

      Converter& operator=(Converter&&) = delete;


      [[nodiscard]] inline CodePage a() const {
        return m_A.encoding();
      }

      [[nodiscard]] inline CodePage b() const {
        return m_B.encoding();
      }

      template<typename CharT>
      [[nodiscard]] inline std::optional<std::string> toA(const CharT* bStr, size_t len) const {
        static_assert(std::is_integral_v<CharT>);
        return toA(reinterpret_cast<const char*>(bStr), len * sizeof(CharT));
      }

      [[nodiscard]] inline std::optional<std::string> toA(const char* bStr, size_t len) const {
        return toA(std::string_view{bStr, len});
      }

      [[nodiscard]] inline std::optional<std::string> toA(std::string_view bStr) const {
        auto tmpOpt = m_B.fromBytes(bStr);
        if(!tmpOpt)
          return {};

        return m_A.toBytes(*tmpOpt);
      }

      template<typename CharT>
      [[nodiscard]] inline std::optional<std::string> toB(const CharT* aStr, size_t len) const {
        static_assert(std::is_integral_v<CharT>);
        return toB(reinterpret_cast<const char*>(aStr), len * sizeof(CharT));
      }

      [[nodiscard]] inline std::optional<std::string> toB(const char* aStr, size_t len) const {
        return toB(std::string_view{aStr, len});
      }

      [[nodiscard]] inline std::optional<std::string> toB(std::string_view aStr) const {
        auto tmpOpt = m_A.fromBytes(aStr);
        if(!tmpOpt)
          return {};

        return m_B.toBytes(*tmpOpt);
      }


      inline explicit operator bool() const {
        return bool(m_A) && bool(m_B);
      }

    private:
      UTF16Converter m_A;
      UTF16Converter m_B;
  };
}

#endif //UNICODE_UNICODE_HPP
