#include "utils.h"

std::string str::replace_all(std::string_view str, std::string_view pattern, std::string_view replacement) {
  using ssize_t = std::make_signed_t<size_t>;

  auto originalStringSize = str.size();
  auto replacementStringSizeDifference = ssize_t(pattern.size()) - ssize_t(replacement.size());
  auto subStrings = std::vector<std::string_view>{};

  auto replacementsCount = 0;
  do {
    auto nextPatternPos = str.find(pattern);
    if(nextPatternPos == std::string_view::npos) {
      subStrings.push_back(str);
      str = {};
    } else {
      subStrings.push_back(str.substr(0, nextPatternPos));
      subStrings.push_back(replacement);
      str = str.substr(nextPatternPos + pattern.size());
      ++replacementsCount;
    }
  } while(!str.empty());

  auto newString = std::string(originalStringSize - replacementStringSizeDifference * replacementsCount, 0);
  auto newStringPtr = newString.data();
  for(auto& subString : subStrings)
    newStringPtr += (subString.copy(newStringPtr, newString.size() - (newStringPtr - newString.data())));

  return newString;
}

std::vector<std::string_view> str::view::tokenize(std::string_view s, std::string_view delims) {
  std::vector<std::string_view> tokens;

  while(!s.empty()) {
    auto delimPos = s.find_first_of(delims);
    if(delimPos == std::string_view::npos) {
      tokens.push_back(s);
      break;
    } else {
      auto token = s.substr(0, delimPos);
      if(!token.empty())
        tokens.push_back(token);
      s = s.substr(delimPos + 1);
    }
  }

  return tokens;
}
std::string_view str::view::ltrim(std::string_view s, std::string_view chars) noexcept {
  auto pos = s.find_first_not_of(chars);
  if(pos == std::string_view::npos)
    return {};
  return s.substr(pos);
}
std::string_view str::view::rtrim(std::string_view s, std::string_view chars) noexcept {
  auto pos = s.find_last_not_of(chars);
  if(pos == std::string_view::npos)
    return {};
  return s.substr(0, pos + 1);
}