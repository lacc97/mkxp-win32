#include "unicodestring.hpp"

#include <unicode/ustring.h>

int unicode::caseInsensitiveCompare(string_view s1, string_view s2) {
  auto ec = UErrorCode{U_ZERO_ERROR};
  return u_strCaseCompare(s1.data(), s1.length(), s2.data(), s2.length(), 0, &ec);
}

std::optional<unicode::string> unicode::fromUTF8(std::string_view utf8) {
  auto utf16 = string(utf8.length(), 0);
  auto utf16Len = int32_t{};

  auto errorCode = UErrorCode{U_ZERO_ERROR};
  u_strFromUTF8(utf16.data(), utf16.size(), &utf16Len, utf8.data(), utf8.size(), &errorCode);
  if(U_SUCCESS(errorCode)) {
    utf16.resize(utf16Len);
    utf16.shrink_to_fit();
    return utf16;
  } else if(errorCode == U_BUFFER_OVERFLOW_ERROR) {
    utf16.resize(utf16Len);

    errorCode = U_ZERO_ERROR;
    u_strFromUTF8(utf16.data(), utf16.size(), &utf16Len, utf8.data(), utf8.size(), &errorCode);
    if(U_SUCCESS(errorCode))
      return utf16;
    else
      return {};
  } else {
    return {};
  }
}

std::optional<unicode::string> unicode::toLowerCase(unicode::string_view str) {
  auto lower = string(str.length(), 0);

  auto errorCode = UErrorCode{U_ZERO_ERROR};
  auto lowerLen = u_strToLower(lower.data(), lower.size(), str.data(), str.size(), nullptr, &errorCode);
  if(U_SUCCESS(errorCode)) {
    lower.resize(lowerLen);
    return lower;
  } else if(errorCode == U_BUFFER_OVERFLOW_ERROR) {
    lower.resize(lowerLen);

    errorCode = U_ZERO_ERROR;
    lowerLen = u_strToLower(lower.data(), lower.size(), str.data(), str.size(), nullptr, &errorCode);
    if(U_SUCCESS(errorCode))
      return lower;
  }

  return {};
}

std::optional<unicode::string> unicode::toUpperCase(unicode::string_view str) {
  auto upper = string(str.length(), 0);

  auto errorCode = UErrorCode{U_ZERO_ERROR};
  auto upperLen = u_strToUpper(upper.data(), upper.size(), str.data(), str.size(), nullptr, &errorCode);
  if(U_SUCCESS(errorCode)) {
    upper.resize(upperLen);
    return upper;
  } else if(errorCode == U_BUFFER_OVERFLOW_ERROR) {
    upper.resize(upperLen);

    errorCode = U_ZERO_ERROR;
    upperLen = u_strToUpper(upper.data(), upper.size(), str.data(), str.size(), nullptr, &errorCode);
    if(U_SUCCESS(errorCode))
      return upper;
  }

  return {};
}

std::optional<std::string> unicode::toUTF8(unicode::string_view utf16) {
  auto utf8 = std::string(2 * utf16.length(), 0);
  auto utf8Len = int32_t{};

  auto errorCode = UErrorCode{U_ZERO_ERROR};
  u_strToUTF8(utf8.data(), utf8.size(), &utf8Len, utf16.data(), utf16.size(), &errorCode);
  if(U_SUCCESS(errorCode)) {
    utf8.resize(utf8Len);
    utf8.shrink_to_fit();
    return utf8;
  } else if(errorCode == U_BUFFER_OVERFLOW_ERROR) {
    utf8.resize(utf8Len);

    errorCode = U_ZERO_ERROR;
    u_strToUTF8(utf8.data(), utf8.size(), &utf8Len, utf16.data(), utf16.size(), &errorCode);
    if(U_SUCCESS(errorCode))
      return utf8;
    else
      return {};
  } else {
    return {};
  }
}