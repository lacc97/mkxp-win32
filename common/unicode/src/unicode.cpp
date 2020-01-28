#include "unicode.hpp"

#include <fmt/format.h>

#include <unicode/ucnv.h>
#include <unicode/utf8.h>

  unicode::UTF16Converter::UTF16Converter(unicode::CodePage enc)
  : m_Encoding{enc},
    mp_Converter{nullptr} {
  auto winCpStr = fmt::format("windows-{0}", enc);

  auto errCode = UErrorCode{U_ZERO_ERROR};
  auto encStr = ucnv_getStandardName(winCpStr.c_str(), "WINDOWS", &errCode);
  if(!encStr)
    return;

  errCode = U_ZERO_ERROR;
  mp_Converter = ucnv_open(encStr, &errCode);
}

unicode::UTF16Converter::~UTF16Converter() {
  if(mp_Converter)
    ucnv_close(mp_Converter);
}

unicode::UTF16Converter& unicode::UTF16Converter::operator=(unicode::UTF16Converter&& other) noexcept {
  if(mp_Converter)
    ucnv_close(mp_Converter);
  mp_Converter = nullptr;

  std::swap(mp_Converter, other.mp_Converter);

  return *this;
}

std::optional<unicode::string> unicode::UTF16Converter::fromBytes(std::string_view str) const {
  if(str.empty())
    return unicode::string{};

  ucnv_reset(mp_Converter);

  auto out = unicode::string(2 * str.size(), 0);
  auto outBuf = out.data();

  auto inBuf = str.data();

  auto done = false;
  auto ucharsDone = size_t{};
  do {
    auto errCode = UErrorCode{U_ZERO_ERROR};
    auto outBufStart = ucharsDone + outBuf;

    ucnv_toUnicode(mp_Converter, &outBuf, outBuf + out.size(), &inBuf, inBuf + str.length(), nullptr, TRUE, &errCode);
    ucharsDone += (outBuf - outBufStart);

    if(U_SUCCESS(errCode)) {
      done = true;
    } else {
      if(errCode == U_BUFFER_OVERFLOW_ERROR) {
        out.resize(ucharsDone + str.length());
        outBuf = out.data();
      } else {
        return {};
      }
    }
  } while(!done);

  out.resize(ucharsDone);
  return out;
}

std::optional<std::string> unicode::UTF16Converter::toBytes(unicode::string_view str) const {
  if(str.length() == 0)
    return std::string{};

  ucnv_reset(mp_Converter);

  auto out = std::string(2 * str.length(), 0);
  auto outBuf = out.data();

  auto inBuf = str.data();

  auto done = false;
  auto charsDone = size_t{};
  do {
    auto errCode = UErrorCode{U_ZERO_ERROR};
    auto outBufStart = charsDone + outBuf;

    ucnv_fromUnicode(mp_Converter, &outBuf, outBuf + out.size(), &inBuf, inBuf + str.length(), nullptr,
                     TRUE, &errCode);
    charsDone += (outBuf - outBufStart);

    if(U_SUCCESS(errCode)) {
      done = true;
    } else {
      if(errCode == U_BUFFER_OVERFLOW_ERROR) {
        out.resize(charsDone + 2 * str.length());
        outBuf = out.data();
      } else {
        return {};
      }
    }
  } while(!done);

  out.resize(charsDone);
  return out;
}
