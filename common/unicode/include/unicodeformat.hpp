#ifndef UNICODE_UNICODEFORMAT_HPP
#define UNICODE_UNICODEFORMAT_HPP

#include "unicode.hpp"

namespace fmt {
  namespace internal {
    template<>
    struct is_string<unicode::string> : std::integral_constant<bool, false> {
    };

    template<>
    struct is_string<unicode::string_view> : std::integral_constant<bool, false> {
    };
  }

  template<>
  struct formatter<unicode::Encoding> : formatter<std::string_view> {
    template<typename FormatContext>
    auto format(unicode::Encoding e, FormatContext& ctx) {
      using namespace std::string_view_literals;

      auto s = "<unknown>"sv;
      switch(e) {
        case unicode::Encoding::NONE:
          break;
        case unicode::Encoding::ASCII:
          s = "ASCII"sv;
          break;
        case unicode::Encoding::CP932:
          s = "CP932"sv;
          break;
        case unicode::Encoding::CP1250:
          s = "CP1250"sv;
          break;
        case unicode::Encoding::CP1251:
          s = "CP1251"sv;
          break;
        case unicode::Encoding::CP1252:
          s = "CP1252"sv;
          break;
        case unicode::Encoding::CP1253:
          s = "CP1253"sv;
          break;
        case unicode::Encoding::CP1254:
          s = "CP1254"sv;
          break;
        case unicode::Encoding::CP1257:
          s = "CP1257"sv;
          break;
        case unicode::Encoding::EUCCN:
          s = "EUC-CN"sv;
          break;
        case unicode::Encoding::EUCJP:
          s = "EUC-JP"sv;
          break;
        case unicode::Encoding::EUCKR:
          s = "EUC-KR"sv;
          break;
        case unicode::Encoding::LOCAL:
          s = "<local>"sv;
          break;
        case unicode::Encoding::SHIFTJIS:
          s = "SHIFT-JIS"sv;
          break;
        case unicode::Encoding::UCS2:
          s = "UCS-2"sv;
          break;
        case unicode::Encoding::UCS4:
          s = "UCS-4"sv;
          break;
        case unicode::Encoding::UTF7:
          s = "UTF-7"sv;
          break;
        case unicode::Encoding::UTF8:
          s = "UTF-8"sv;
          break;
        case unicode::Encoding::UTF16:
          s = "UTF-16"sv;
          break;
        case unicode::Encoding::UTF32:
          s = "UTF-32"sv;
          break;
        case unicode::Encoding::WLOCAL:
          s = "<wchar local>"sv;
          break;

        default:
          return format_to(ctx.out(), "<unknown:{0}>", (int) e);
      }

      return formatter<std::string_view>::format(s, ctx);
    }
  };

//    template <>
//    struct formatter<std::reference_wrapper<const unicode::string_view>> {
//        constexpr auto parse(format_parse_context &ctx) {
//            return ctx.begin();
//        }
//
//        template <typename FormatContext>
//        auto format(std::reference_wrapper<const unicode::string_view> sv, FormatContext &ctx) {
//            return format_to(ctx.out(), "{}", *unicode::toUTF8(sv.get()));
//        }
//    };
//
//    template <>
//    struct formatter<std::reference_wrapper<const unicode::string>> {
//        constexpr auto parse(format_parse_context &ctx) {
//            return ctx.begin();
//        }
//
//        template <typename FormatContext>
//        auto format(std::reference_wrapper<const unicode::string> str, FormatContext &ctx) {
//            return format_to(ctx.out(), "{}", *unicode::toUTF8(str.get()));
//        }
//    };

  template<>
  struct formatter<unicode::string_view> : formatter<std::string_view> {
    template<typename FormatContext>
    auto format(unicode::string_view str, FormatContext& ctx) {
      auto utf8Opt = unicode::toUTF8(str);
      if(utf8Opt)
        return formatter<std::string_view>::format(*utf8Opt, ctx);
      else
        return formatter<std::string_view>::format("<invalid utf16 string>", ctx);
    }
  };

  template<>
  struct formatter<unicode::string> : formatter<unicode::string_view> {
    template<typename FormatContext>
    auto format(const unicode::string& str, FormatContext& ctx) {
      return formatter<unicode::string_view>::format(str, ctx);
    }
  };
}

#endif //UNICODE_UNICODEFORMAT_HPP
