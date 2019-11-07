#include "utils.h"

namespace {
//    std::vector<std::string_view> tokenize(std::string_view str, std::string_view delim) {
//        auto tokens = std::vector<std::string_view>{};
//
//        do {
//            auto nextPatternPos = str.find(delim);
//            if(nextPatternPos == std::string_view::npos) {
//                tokens.push_back(str);
//                str = {};
//            } else {
//                tokens.push_back(str.substr(0, nextPatternPos));
//                str = str.substr(nextPatternPos + delim.size());
//            }
//        } while(!str.empty());
//
//        return tokens;
//    }
}

std::string String::replace_all(std::string_view str, std::string_view pattern, std::string_view replacement) {
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
    for(auto& subString :  subStrings)
        newStringPtr += (subString.copy(newStringPtr, newString.size() - (newStringPtr - newString.data())));

    return newString;
}
